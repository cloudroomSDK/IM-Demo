
import CRUICore
import RxCocoa
import RxSwift

class GroupChatSettingViewModel {
    private let _disposeBag = DisposeBag()
    private(set) var conversation: ConversationInfo

    let membersRelay: BehaviorRelay<[GroupMemberInfo]> = .init(value: [])
    let combileMembersRelay: BehaviorRelay<[GroupMemberInfo]> = .init(value: [])
    let membersCountRelay: BehaviorRelay<Int> = .init(value: 0)
    let noDisturbRelay: BehaviorRelay<Bool> = .init(value: false)
    let setTopContactRelay: BehaviorRelay<Bool> = .init(value: false)
    var groupInfoRelay: BehaviorRelay<GroupInfo?> = .init(value: nil)
    let myInfoInGroup: BehaviorRelay<GroupMemberInfo?> = .init(value: nil)
    let mutedAllRelay: BehaviorRelay<Bool> = .init(value: false)
    let canViewProfileRelay: BehaviorRelay<Bool> = .init(value: false)
    let canAddFriendRelay: BehaviorRelay<Bool> = .init(value: false)
    
    private(set) var allMembers: [String] = []

    init(conversation: ConversationInfo) {
        self.conversation = conversation
    }

    func getConversationInfo() {
        guard let gid = conversation.groupID else { return }

        IMController.shared.getConversation(sessionType: conversation.conversationType, sourceId: gid) { [weak self] (chat: ConversationInfo?) in
            guard let sself = self else { return }
            if let chat = chat {
                self?.conversation = chat
                self?.noDisturbRelay.accept(sself.conversation.recvMsgOpt != .receive)
                self?.setTopContactRelay.accept(sself.conversation.isPinned)
            }
        }

        IMController.shared.getGrpMemberList(groupId: gid, filter: .all, offset: 0, count: 8) { [weak self] (members: [GroupMemberInfo]) in
            var users = members
            self?.membersRelay.accept(users)
            self?.combileMembersRelay.accept(users)
        }
        
        IMController.shared.getGrpInfo(groupIds: [gid]) { [weak self] (groupInfos: [GroupInfo]) in
            guard let sself = self else { return }
            guard let groupInfo = groupInfos.first else { return }
            self?.groupInfoRelay.accept(groupInfo)
            self?.membersCountRelay.accept(groupInfo.memberCount)
            self?.mutedAllRelay.accept(groupInfo.status == .muted)
            sself.canAddFriendRelay.accept((groupInfo.applyMemberFriend != 0))
            sself.canViewProfileRelay.accept((groupInfo.lookMemberInfo != 0))
            IMController.shared.getGrpMemberList(groupId: groupInfo.groupID, offset: 0, count: groupInfo.memberCount) { (members: [GroupMemberInfo]) in
                self?.allMembers = members.compactMap { $0.userID }
            }
        }
        // 获取自己的组内信息
        IMController.shared.getGrpMembersInfo(groupId: gid, uids: [IMController.shared.userID]) { [weak self] (members: [GroupMemberInfo]) in
            for member in members {
                if member.isSelf {
                    self?.myInfoInGroup.accept(member)
                }
            }
        }
        
        let combinedRelay = BehaviorRelay.combineLatest(combileMembersRelay, myInfoInGroup) { members, member in
            if let member {
                var users = members
                let fakeUser = GroupMemberInfo()
                fakeUser.isAddButton = true
                fakeUser.nickname = "增加".innerLocalized()
                users.append(fakeUser)
                
                if member.roleLevel != .member {
                    let fakeUser2 = GroupMemberInfo()
                    fakeUser2.isRemoveButton = true
                    fakeUser2.nickname = "移除".innerLocalized()
                    users.append(fakeUser2)
                }
                return users
            }
            
            return members
        }
        combinedRelay.subscribe(onNext: { [weak self] users in
            self?.membersRelay.accept(users)
        }).disposed(by: _disposeBag)
    }

    func updateGroupName(_ name: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: @escaping () -> Void) {
        guard let group = groupInfoRelay.value else { return }
        group.groupName = name
        IMController.shared.setGrpInfo(group: group) { [weak self] resp in
            self?.groupInfoRelay.accept(group)
            onSuccess(resp)
        } onFailure: { errCode, errMsg in
            onFailure()
        }
    }

    func clearRecord(completion: @escaping CallBack.StringOptionalReturnVoid) {
        guard let groupID = conversation.groupID else { return }
        IMController.shared.clearGrpHistoryMsgs(conversationID: conversation.conversationID) { [weak self] resp in
            guard let sself = self else { return }
            let event = EventRecordClear(conversationId: sself.conversation.conversationID)
            JNNotificationCenter.shared.post(event)
            completion(resp)
        }
    }
    
    func toggleCanViewProfile() {
        guard let groupID = conversation.groupID else { return }
        IMController.shared.setGrpLookMemberInfo(id: groupID, rule: canViewProfileRelay.value ? 0 : 1, completion: { [weak self] _ in
            guard let sself = self else { return }
            sself.canViewProfileRelay.accept(!sself.canViewProfileRelay.value)
        })
    }
    
    func toggleCanAddFriend() {
        guard let groupID = conversation.groupID else { return }
        IMController.shared.setGrpApplyMemberFriend(id: groupID, rule: canAddFriendRelay.value ? 0 : 1, completion: { [weak self] _ in
            guard let sself = self else { return }
            sself.canAddFriendRelay.accept(!sself.canAddFriendRelay.value)
        })
    }

    func toggleTopContacts() {
        IMController.shared.pinConversation(id: conversation.conversationID, isPinned: setTopContactRelay.value, completion: { [weak self] _ in
            guard let sself = self else { return }
            sself.setTopContactRelay.accept(!sself.setTopContactRelay.value)
        })
    }
    
    func toggleMuteAll() {
        guard let groupID = conversation.groupID else { return }
        IMController.shared.changeGrpMute(groupID: groupID, isMute: !mutedAllRelay.value, completion: { [weak self] _ in
            guard let sself = self else { return }
            sself.mutedAllRelay.accept(!sself.mutedAllRelay.value)
        })
    }
    
    func toggleNoDisturb() {
        let receiveOpt: ReceiveMessageOpt = !noDisturbRelay.value == true ? .notNotify : .receive
        IMController.shared.setConversationRecvMsgOpt(conversationID: conversation.conversationID, status: receiveOpt, completion: { [weak self] _ in
            guard let sself = self else { return }
            self?.noDisturbRelay.accept(!sself.noDisturbRelay.value)
        })
    }

    func setNoDisturbWithNotRecieve() {
        IMController.shared.setConversationRecvMsgOpt(conversationID: conversation.conversationID, status: .notReceive, completion: { [weak self] _ in
            guard let sself = self else { return }
            self?.noDisturbRelay.accept(true)
        })
    }

    func setNoDisturbWithNotNotify() {
        IMController.shared.setConversationRecvMsgOpt(conversationID: conversation.conversationID, status: .notNotify, completion: { [weak self] _ in
            guard let sself = self else { return }
            self?.noDisturbRelay.accept(true)
        })
    }

    func setNoDisturbOff() {
        IMController.shared.setConversationRecvMsgOpt(conversationID: conversation.conversationID, status: .receive, completion: { [weak self] _ in
            guard let sself = self else { return }
            self?.noDisturbRelay.accept(false)
        })
    }

    func dismissGrp(onSuccess: @escaping CallBack.VoidReturnVoid) {
        guard let groupId = conversation.groupID else { return }
        IMController.shared.dismissGrp(id: groupId) { [weak self] _ in
            guard let sself = self else { return }
            let event = EventGroupDismissed(conversationId: sself.conversation.conversationID)
            JNNotificationCenter.shared.post(event)
            IMController.shared.deleteConversation(conversationID: (self?.conversation.conversationID)!) { r in
                onSuccess()
            }
        }
    }

    func quitGrp(onSuccess: @escaping CallBack.VoidReturnVoid) {
        guard let groupId = conversation.groupID else { return }
        IMController.shared.quitGrp(id: groupId) { [weak self] _ in
            guard let sself = self else { return }
            let event = EventGroupDismissed(conversationId: sself.conversation.conversationID)
            JNNotificationCenter.shared.post(event)
            IMController.shared.deleteConversation(conversationID: (self?.conversation.conversationID)!) { r in
                onSuccess()
            }
        }
    }

    func updateMyNicknameInGroup(_ nickname: String, onSuccess: @escaping CallBack.VoidReturnVoid) {
        guard let group = groupInfoRelay.value else { return }
        IMController.shared.setGrpMemberNicknameOf(userid: IMController.shared.userID, inGroupId: group.groupID, with: nickname) { [weak self] _ in
            let member = self?.myInfoInGroup.value
            member?.nickname = nickname
            self?.myInfoInGroup.accept(member)
            onSuccess()
        }
    }
    
    func transferOwner(to uid: String, onSuccess: @escaping CallBack.VoidReturnVoid) {
        guard let group = groupInfoRelay.value else { return }
        IMController.shared.transferOwner(groupId: group.groupID, to: uid) { r in
            onSuccess()
        }
    }
    
    func updateVerificationOption(type: GroupVerificationType) {
        guard let group = groupInfoRelay.value else { return }
        IMController.shared.setGrpVerification(groupId: group.groupID, type: type) {[weak self] r in
            
            if var info = self?.groupInfoRelay.value {
                info.needVerification = type
                self?.groupInfoRelay.accept(info)
            }
        }
    }
    
    func uploadFile(fullPath: String, onProgress: @escaping (CGFloat) -> Void, onComplete: @escaping () -> Void, onFailure: @escaping () -> Void) {
        IMController.shared.uploadFile(fullPath: fullPath, onProgress: onProgress) { [weak self] url in
            if let url = url {
                guard let groupInfo = self?.groupInfoRelay.value else { return }
                groupInfo.faceURL = url
                IMController.shared.setGrpInfo(group: groupInfo) { [weak self] resp in
                    self?.groupInfoRelay.accept(groupInfo)
                    onComplete()
                } onFailure: { errCode, errMsg in
                    onFailure()
                }
            }
        } onFailure: { errCode, errMsg in
            onFailure()
        }
    }
}

fileprivate var GroupMemberInfoAddButtonExtensionKey: String?
fileprivate var GroupMemberInfoRemoveButtonExtensionKey: String?

extension GroupMemberInfo {
    public func toUserInfo() -> UserInfo {
        let user = UserInfo(userID: userID!)
        user.faceURL = faceURL
        user.nickname = nickname
        
        return user
    }
    
    public var isAddButton: Bool {
        set {
            objc_setAssociatedObject(self, &GroupMemberInfoAddButtonExtensionKey, newValue, objc_AssociationPolicy.OBJC_ASSOCIATION_ASSIGN)
        }

        get {
            let value: Bool = objc_getAssociatedObject(self, &GroupMemberInfoAddButtonExtensionKey) as? Bool ?? false
            return value
        }
    }
    
    public var isRemoveButton: Bool {
        set {
            objc_setAssociatedObject(self, &GroupMemberInfoRemoveButtonExtensionKey, newValue, objc_AssociationPolicy.OBJC_ASSOCIATION_ASSIGN)
        }

        get {
            let value: Bool = objc_getAssociatedObject(self, &GroupMemberInfoRemoveButtonExtensionKey) as? Bool ?? false
            return value
        }
    }
}
