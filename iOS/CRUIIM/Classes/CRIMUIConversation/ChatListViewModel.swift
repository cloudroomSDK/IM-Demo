
import CRUICore
import RxRelay
import RxSwift
import CRUICalling

class ChatListViewModel {
    var conversationsRelay: BehaviorRelay<[ConversationInfo]> = .init(value: [])
    let loginUserPublish: PublishSubject<UserInfo?> = .init()

    private func getSelfInfo(onSuccess: @escaping CallBack.UserInfoOptionalReturnVoid) {
        IMController.shared.getSelfInfo(onSuccess: onSuccess)
    }

    func getSelfInfo() {
        IMController.shared.getSelfInfo { [weak self] (userInfo: UserInfo?) in
            self?.loginUserPublish.onNext(userInfo)
        }
    }

    func getAllConversations() {
        IMController.shared.getAllConversationList { [weak self] (conversations: [ConversationInfo]) in
            guard let sself = self else { return }
            sself.sortConversations(conversations)
        }
    }

    func setConversation(id: String, status: ReceiveMessageOpt) {
        IMController.shared.setConversationRecvMsgOpt(conversationID: id, status: status, completion: nil)
    }

    func pinConversation(id: String, isPinned: Bool, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        IMController.shared.pinConversation(id: id, isPinned: isPinned) { [weak self] (resp: String?) in
            self?.getAllConversations()
            onSuccess(resp)
        }
    }
    
    func markReaded(id: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        IMController.shared.markConversationMsgAsRead(byConID: id) { [weak self] msg in
            self?.getAllConversations()
            onSuccess(msg)
        }
    }

    /// 删除指定会话（本地删除）
    func deleteConversation(conversationID: String, completion: ((String?) -> Void)?) {
        IMController.shared.deleteConversation(conversationID: conversationID) { [weak self] r in
            self?.getAllConversations()
            completion?(r)
        }
    }
    
    func hideConversation(conversationID: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        IMController.shared.hideConversation(conversationID: conversationID) { [weak self] r in
            self?.getAllConversations()
            onSuccess(r)
        }
    }

    init() {
        IMController.shared.newConversationSubject.subscribe(onNext: { [weak self] (conversations: [ConversationInfo]) in
            guard let sself = self else { return }
            var origin = sself.conversationsRelay.value
                
            for (index, item) in conversations.enumerated() {
                if !origin.contains(where: { info in
                    return info.conversationID == item.conversationID
                }) {
                    origin.append(item)
                }
            }
            
            self?.sortConversations(origin)
        }).disposed(by: _disposeBag)

        IMController.shared.conversationChangedSubject.subscribe(onNext: { [weak self] (conversations: [ConversationInfo]) in
            guard let sself = self else { return }
            let changedIds: [String] = conversations.compactMap { $0.conversationID }
            var origin = sself.conversationsRelay.value
            var ret = origin.filter { (chat: ConversationInfo) -> Bool in
                !changedIds.contains(chat.conversationID)
            }
            ret.append(contentsOf: conversations)
            self?.sortConversations(ret)
        })

        JNNotificationCenter.shared.observeEvent { [weak self] (_: EventLoginSucceed) in
            self?.getAllConversations()
            self?.getSelfInfo(onSuccess: { (userInfo: UserInfo?) in
                self?.loginUserPublish.onNext(userInfo)
                if let nickname = userInfo?.nickname {
                    CRUICalling.CallingManager.manager.start(nickname: nickname)
                }
            })
        }.disposed(by: _disposeBag)
        
        JNNotificationCenter.shared.observeEvent { (_: EventLoginSucceed) in
            CRUICalling.CallingManager.manager.end()
        }.disposed(by: _disposeBag)

        JNNotificationCenter.shared.observeEvent { [weak self] (_: EventRecordClear) in
            self?.getAllConversations()
        }
        
        
        JNNotificationCenter.shared.observeEvent { [weak self] (event: EventGroupDismissed) in
            guard let sself = self else { return }
            sself.removeConversation(event.conversationId)
        }
    }
    
    private func removeConversation(_ conversationID: String) {
        var origin = conversationsRelay.value
        
        var changed = false
        for (index, item) in conversationsRelay.value.enumerated() {
            if item.conversationID == conversationID {
                origin.remove(at: index)
                changed = true
            }
        }
        
        if changed {
            conversationsRelay.accept(origin)
        }
    }

    private func sortConversations(_ conversations: [ConversationInfo]) {
        var sorted = conversations.sorted { (lhs: ConversationInfo, rhs: ConversationInfo) in
            lhs.latestMsgSendTime > rhs.latestMsgSendTime
        }
        var pinned: [ConversationInfo] = []
        var normal: [ConversationInfo] = []
        for conversation in sorted {
            if conversation.isPinned {
                pinned.append(conversation)
            } else {
                normal.append(conversation)
            }
        }
        pinned.append(contentsOf: normal)
        conversationsRelay.accept(pinned)
    }

    private let _disposeBag = DisposeBag()
}
