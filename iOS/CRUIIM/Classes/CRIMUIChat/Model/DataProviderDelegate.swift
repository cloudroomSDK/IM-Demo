
import CRUICore

protocol DataProviderDelegate: AnyObject {

    func received(message: MessageInfo)
    
    func typingStateChanged(to state: TypingState)

    func lastReadIdsChanged(to ids: [String], readUserID: String?)

    func lastReceivedIdChanged(to message: MessageInfo)
    
    func isInGroup(with isIn: Bool)
    
    func receivedRevokedInfo(info: MessageRevoked)
    
    func groupInfoChanged(groupInfo: GroupInfo?)
    
    // 处理转发消息到当前会话的刷新
    func appendSentMessage(message: MessageInfo)
}
