
import CRUICore
import RxRelay

class NewFriendListViewModel {
    let newFriendReqs: BehaviorRelay<[FriendApplication]> = .init(value: [])

    func getNewFriendReqs() {
        IMController.shared.getFriendReqList { [weak self] (applications: [FriendApplication]) in
            self?.newFriendReqs.accept(applications)
        }
    }

    func acceptFriendWith(uid: String) {
        IMController.shared.acceptFriendReq(uid: uid, completion: { [weak self] (_: String?) in
            self?.getNewFriendReqs()
            // 发送通知，告诉列表入群申请或者好友申请数量发生改变
            NotificationCenter.default.post(name: ContactsViewModel.NotificationApplicationCountChanged, object: nil)
        })
    }
}
