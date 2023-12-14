
import Foundation
import CRUICore

protocol ChatController {

    func loadInitialMessages(completion: @escaping ([Section]) -> Void)
    func loadPreviousMessages(completion: @escaping ([Section]) -> Void)
    func sendMsg(_ data: Message.Data, completion: @escaping ([Section]) -> Void)

    func deleteMsg(with id: String)
    func getConversation() -> ConversationInfo
    func getGroupMembers(completion: @escaping ([GroupMemberInfo]) -> Void)
    func getGrpInfo(requirePull: Bool, completion: @escaping (GroupInfo) -> Void)
    func getOtherInfo(completion: @escaping (FullUserInfo) -> Void)
    func getSelfInfo() -> UserInfo?
    func getMessageInfo(ids: [String]) -> [MessageInfo]
    
    func addFriend(onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: @escaping CallBack.ErrorOptionalReturnVoid)
    func revokeMsg(with id: String)
    
    func isMsgRevokedExpired(timestamp: TimeInterval) -> Bool
    func removeMessage(messageID: String)
}

extension ChatController {
    func getMessageInfo( _: [String]) -> [MessageInfo] { [] }
    func getSelfInfo() -> UserInfo? { nil }
}
