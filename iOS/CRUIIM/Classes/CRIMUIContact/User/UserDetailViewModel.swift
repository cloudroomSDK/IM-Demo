
import CRUICore
import RxSwift
import RxRelay

class UserDetailViewModel {
    let userId: String
    let groupId: String?
    let groupInfo: GroupInfo?
    
    let userInfoRelay: BehaviorRelay<FullUserInfo?> = .init(value: nil)
    //let userInfoRelay: BehaviorRelay<UserInfo?> = .init(value: nil)
    let loginUserPublish: PublishSubject<UserInfo?> = .init()
    let strangerInfoRelay: BehaviorRelay<UserInfo?> = .init(value: nil)
    var memberInfoRelay: BehaviorRelay<GroupMemberInfo?> = .init(value: nil)
    var showSetAdmin: Bool = false
    var showJoinSource: Bool = false
    var showMute: Bool = false
    var userDetailFor = UserDetailFor.groupMemberInfo
    let isInBlackListRelay: PublishSubject<Bool> = .init()

    private let _disposeBag = DisposeBag()
    init(userId: String, groupId: String?, groupInfo: GroupInfo? = nil, userDetailFor: UserDetailFor) {
        self.userId = userId
        self.groupId = groupId
        self.groupInfo = groupInfo
        self.userDetailFor = userDetailFor
        
        IMController.shared.friendInfoChangedSubject.subscribe { [weak self] (friendInfo: FriendInfo?) in
            guard let sself = self else { return }
            guard friendInfo?.userID == sself.userId else { return }
            let user = sself.userInfoRelay.value?.friendInfo
            user?.nickname = friendInfo?.nickname
            if let gender = friendInfo?.gender {
                user?.gender = gender
            }
            user?.phoneNumber = friendInfo?.phoneNumber
            if let birth = friendInfo?.birth {
                user?.birth = birth
            }
            user?.email = friendInfo?.email
            user?.remark = friendInfo?.remark
            let fullUser = self?.userInfoRelay.value
            fullUser?.friendInfo = user
            self?.userInfoRelay.accept(fullUser)
        }.disposed(by: _disposeBag)
        
        IMController.shared.getBlackList {[weak self] blackUsers in
            if blackUsers.contains(where: { info in
                info.userID == self?.userId
            }) {
                self?.isInBlackListRelay.onNext(true)
            }
        }
    }
    
    func getSelfInfo() {
        IMController.shared.getSelfInfo { [weak self] (userInfo: UserInfo?) in
            self?.loginUserPublish.onNext(userInfo)
        }
    }

    func getUserOrMemberInfo() {

        let group = DispatchGroup()
        
        var userInfo: FullUserInfo?
        var strangerInfo: UserInfo?
        var memberInfo: GroupMemberInfo?
        
        IMController.shared.checkFriendBy(userID: userId).subscribe { [weak self] (result: Bool) in
            guard let userID = self?.userId else {
                return
            }
            
            print("checkFriendBy \(result)")
            if result {
                group.enter()
                IMController.shared.getFriendsInfo(userIDs: [userID]) { [weak self] users in
                    userInfo = users.first
                    
                    if let userID = self?.userId, let handler = CRIMApi.queryUsersInfoWithCompletionHandler {
                        handler([userID], { [weak self] users in
                            if let u = users.first {
                                userInfo?.friendInfo?.birth = u.birth ?? 0
                                userInfo?.friendInfo?.phoneNumber = u.phoneNumber
                            }
                            group.leave()
                        })
                    } else {
                        group.leave()
                    }
                    
                }
            } else {
                group.enter()
                IMController.shared.getUserInfo(uids: [userID]) { users in
                    strangerInfo = users?.first
                    group.leave()
                }
            }
            
            if let groupId = self?.groupId, !groupId.isEmpty, self?.userDetailFor == .groupMemberInfo {
                group.enter()
                // 如果群聊点击的是自己
                var isSelf = IMController.shared.userID == userID
                IMController.shared.getGrpMembersInfo(groupId: groupId,
                                                        uids: isSelf ? [userID] : [IMController.shared.userID, userID]) { [weak self] (members: [GroupMemberInfo]) in
                    guard let mine = members.first(where: { $0.userID == IMController.shared.userID}), let sself = self else {
                        group.leave()
                        return
                    }
                    memberInfo = members.first(where: { $0.userID == sself.userId})
                    
                    if !isSelf {
                        if mine.roleLevel == .owner {
                            sself.showMute = true
                            sself.showSetAdmin = true
                            sself.showJoinSource = true
                        } else if mine.roleLevel == .admin {
                            sself.showMute = memberInfo?.roleLevel != .owner && memberInfo?.roleLevel != .admin
                            sself.showJoinSource = true
                        }
                        
                        IMController.shared.getUserInfo(uids: [memberInfo!.inviterUserID!]) { users in
                            memberInfo!.inviterUserName = users?.first?.nickname
                            group.leave()
                        }
                        
                    } else {
                        group.leave()
                    }
                }
            }
            group.notify(queue: .main) { [weak self] in
                self?.memberInfoRelay.accept(memberInfo)
                self?.userInfoRelay.accept(userInfo)
                self?.strangerInfoRelay.accept(strangerInfo)
            }
            
        }.disposed(by: _disposeBag)
        
    }

    func createSingleChat(onComplete: @escaping (ConversationInfo) -> Void) {
        IMController.shared.getConversation(sessionType: .c1v1, sourceId: userId) { [weak self] (conversation: ConversationInfo?) in
            guard let conversation else { return }

            onComplete(conversation)
        }
    }

    func addFriend(reqMsg: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: @escaping CallBack.ErrorOptionalReturnVoid) {
        IMController.shared.addFriend(uid: userId, reqMsg: reqMsg, onSuccess: onSuccess, onFailure: onFailure)
    }
    
    func toggleSetAdmin(toAdmin: Bool, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        IMController.shared.setGrpMemberRoleLevel(groupId: groupId!, userID: userId, roleLevel: toAdmin ? .admin : .member, onSuccess: onSuccess)
    }
    
    func setGroupMuted(isMute: Bool, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        guard let groupId = groupId else { return }
        
        IMController.shared.changeGrpMute(groupID: groupId, isMute: isMute) { r in
            onSuccess(r)
        }
    }
    
    func setMutedSeconds(seconds: Int, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        guard let groupId = groupId else { return }

        IMController.shared.changeGrpMemberMute(groupID: groupId, userID: userId, seconds: seconds) { [weak self] r in
            guard let sself = self else { return }
            
            var info = sself.memberInfoRelay.value!
            info.muteEndTime = NSDate().timeIntervalSince1970 + Double(seconds)
            sself.memberInfoRelay.accept(info)
            onSuccess(r)
        }
    }
    
    func blockUser(blocked: Bool, onSuccess: @escaping CallBack.StringOptionalReturnVoid){
        IMController.shared.blockUser(uid: userId, blocked: blocked, onSuccess: onSuccess)
    }
    
    func deleteFriend(onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        IMController.shared.deleteFriend(uid: userId) { [weak self] res in
            guard let `self` = self else { return }
            IMController.shared.getConversation(sessionType: .c1v1, sourceId: userId) { conv in
                guard let conv else { return }
                IMController.shared.deleteConversation(conversationID: conv.conversationID) { r in
                    onSuccess(res)
                }
            }
        }
    }
    
    func sendCardMsg(_ contact: ContactInfo, _ receivers : [ContactInfo], completion: @escaping (MessageInfo) -> Void) {
        let cardElem = CardElem(userID: contact.ID!, nickname: contact.name!, faceURL: contact.faceURL)
        receivers.forEach { receiver in
            IMController.shared.sendCardMsg(card: cardElem,
                                            to: receiver.ID!,
                                            groupName: groupInfo?.groupName,
                                            conversationType: receiver.type == .groups ? .group: .c1v1) { [weak self] msg in
                
            } onComplete: { [weak self] msg in
                IMController.shared.syncLocalMsgSentSubject.onNext(msg)
            }
        }
    }
    
    deinit {
        #if DEBUG
            print("dealloc \(type(of: self))")
        #endif
    }
}
