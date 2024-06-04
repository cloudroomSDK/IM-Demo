
import CRUICore

protocol DataProviderDelegate: AnyObject {

    func received(message: MessageInfo)
    
    func typingStateChanged(to state: TypingState)

    func lastReadIdsChanged(to ids: [String], readUserID: String?)

    func lastReceivedIdChanged(to id: String)
    
    func isInGroup(with isIn: Bool)
    
    func receivedRevokedInfo(info: MessageRevoked)
    
    // 处理转发消息到当前会话的刷新
    func appendSentMessage(message: MessageInfo)
}
