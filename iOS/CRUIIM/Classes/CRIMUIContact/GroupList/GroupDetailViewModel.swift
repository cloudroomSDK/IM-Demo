
import CRUICore
import RxSwift
import RxRelay

class GroupDetailViewModel {
    let groupId: String
    let joinSource: JoinSource
    
    let membersRelay: BehaviorRelay<[GroupMemberInfo]> = .init(value: [])
    let membersCountRelay: BehaviorRelay<Int> = .init(value: 0)
    let groupInfoRelay: BehaviorRelay<GroupInfo?> = .init(value: nil)
    let isInGroupSubject: PublishSubject<Bool> = .init()
    var allMembers: [String] = []

    init(groupId: String, joinSource: JoinSource) {
        self.groupId = groupId
        self.joinSource = joinSource
    }

    func getGrpInfo() {
        IMController.shared.getGrpMemberList(groupId: groupId, filter: .all, offset: 0, count: 6) { [weak self] (members: [GroupMemberInfo]) in
            self?.membersRelay.accept(members)
        }

        IMController.shared.getGrpInfo(groupIds: [groupId]) { [weak self] (groupInfos: [GroupInfo]) in
            guard let sself = self else { return }
            guard let groupInfo = groupInfos.first else { return }
            self?.groupInfoRelay.accept(groupInfo)
            self?.membersCountRelay.accept(groupInfo.memberCount)
            IMController.shared.getGrpMemberList(groupId: sself.groupId, offset: 0, count: groupInfo.memberCount) { (members: [GroupMemberInfo]) in
                self?.allMembers = members.compactMap { $0.userID }
            }
        }
        // 获取自己的组内信息
        IMController.shared.getGrpMembersInfo(groupId: groupId, uids: [IMController.shared.userID]) { [weak self] (members: [GroupMemberInfo]) in
            self?.isInGroupSubject.onNext(!members.isEmpty)
        }
    }

    func joinCurrentGroup(onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: @escaping CallBack.ErrorOptionalReturnVoid) {
        IMController.shared.joinGrp(id: groupId, reqMsg: nil, joinSource: joinSource, onSuccess: onSuccess, onFailure: onFailure)
    }
}
