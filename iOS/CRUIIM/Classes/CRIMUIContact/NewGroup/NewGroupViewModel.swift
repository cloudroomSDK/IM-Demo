
import CRUICore
import RxRelay

class NewGroupViewModel {
    
    var groupName: String?
    var users: [UserInfo] = []
    var groupFaceURL: String?
    
    var membersRelay: BehaviorRelay<[UserInfo]> = .init(value: [])
    let membersCountRelay: BehaviorRelay<Int> = .init(value: 0)
    
    init(users: [UserInfo]) {
        self.users = users
    }

    func getMembers() {
        self.membersCountRelay.accept(users.count)
        let fakeUser = UserInfo(userID: "")
        fakeUser.isAddButton = true
        users.append(fakeUser)
        let fakeUser_2 = UserInfo(userID: "")
        fakeUser_2.isRemoveButton = true
        users.append(fakeUser_2)
        self.membersRelay.accept(users)
    }
    
    func updateMembers(_ users: [UserInfo]) {
        self.users.insert(contentsOf: users, at: self.users.count - 2)
        self.membersCountRelay.accept(self.users.count - 2)
        self.membersRelay.accept(self.users)
    }
    
    func deleteMembers(_ users: [UserInfo]) {
        self.users.removeAll { element1 in
            users.contains { element2 in
                element1.userID == element2.userID
            }
        }
        self.membersCountRelay.accept(self.users.count - 2)
        self.membersRelay.accept(self.users)
    }
    
    func createGroup(onSuccess: @escaping CallBack.ConversationInfoOptionalReturnVoid) {
        guard let groupName = groupName else {
            return
        }

        IMController.shared.createGrpConversation(faceURL: groupFaceURL ?? "", users: users,
                                                    groupName: groupName) { [weak self] groupInfo in
            
            guard let groupInfo = groupInfo, let sself = self else {
                onSuccess(nil)
                return
            }
            
            IMController.shared.getConversation(sessionType: .group,
                                                sourceId: groupInfo.groupID) { [weak self] (conversation: ConversationInfo?) in
                onSuccess(conversation)
                
                // 修改进群验证方式
                IMController.shared.setGrpVerification(groupId: groupInfo.groupID, type: .directly) { _ in
                    
                }
            }
        }
    }
    
    func uploadFile(fullPath: String, onProgress: @escaping (CGFloat) -> Void, onComplete: @escaping () -> Void, onFailure: @escaping () -> Void) {
        IMController.shared.uploadFile(fullPath: fullPath, onProgress: onProgress) { [weak self] url in
            if let url = url {
                self?.groupFaceURL = url
                onComplete()
            }
        } onFailure: { errCode, errMsg in
            onFailure()
        }
    }
}
