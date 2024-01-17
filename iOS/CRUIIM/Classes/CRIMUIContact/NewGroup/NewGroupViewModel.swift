
import CRUICore
import RxRelay

class NewGroupViewModel {
    
    var groupName: String?
    var users: [UserInfo] = []
    
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
        self.membersRelay.accept(users)
    }
    
    func updateMembers(_ users: [UserInfo]) {
        self.users.insert(contentsOf: users, at: self.users.count - 1)
        self.membersCountRelay.accept(self.users.count)
        self.membersRelay.accept(self.users)
    }
    
    func createGroup(onSuccess: @escaping CallBack.ConversationInfoOptionalReturnVoid) {
        guard let groupName = groupName else {
            return
        }

        IMController.shared.createGrpConversation(users: users,
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
}
