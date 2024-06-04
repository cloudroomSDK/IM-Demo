
import Foundation
import CRUICore

protocol ChatController {

    func loadInitialMessages(completion: @escaping ([Section]) -> Void)
    func loadPreviousMessages(completion: @escaping ([Section]) -> Void)
    func sendMsg(_ data: Message.Data, completion: @escaping ([Section]) -> Void)
    func sendForwardMsg(_ contacts: [ContactInfo], completion: @escaping ([Section]) -> Void)
    func sendMergeForwardMsg(_ contacts: [ContactInfo], completion: @escaping ([Section]) -> Void)
    func sendCardMsg(_ contact: ContactInfo, completion: @escaping ([Section]) -> Void)
    func sendLocationMsg(_ latitude: Double, _ longitude: Double, _ desc: String, completion: @escaping ([Section]) -> Void)

    func defaultSelecteMessage(with id: String?)
    func clearSelectedStatus()
    func deleteMsgs()
    func clearChatHistory()
    func getConversation() -> ConversationInfo
    func getGroupInfo() -> GroupInfo?
    func getGroupMembers(completion: @escaping ([GroupMemberInfo]) -> Void)
    func getGrpInfo(requirePull: Bool, completion: @escaping (GroupInfo) -> Void)
    func getOtherInfo(completion: @escaping (FullUserInfo) -> Void)
    func getSelfInfo() -> UserInfo?
    func getMessageInfo(ids: [String]) -> [MessageInfo]
    
    func addFriend(onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: @escaping CallBack.ErrorOptionalReturnVoid)
    func revokeMsg(with id: String)
    
    func markAudio(messageId: String, isPlaying: Bool)
    func isMsgRevokedExpired(timestamp: TimeInterval) -> Bool
    
    func call()
}

extension ChatController {
    func getMessageInfo( _: [String]) -> [MessageInfo] { [] }
    func getSelfInfo() -> UserInfo? { nil }
}


protocol ChatHisotryController {
    
    func loadInitialMessages(completion: @escaping ([Section]) -> Void)
    func getMessageInfo(ids: [String]) -> [MessageInfo]
    func getMergerMessageTitle() -> String
    
    func defaultSelecteMessage(with id: String?)
    func sendForwardMsg(_ contacts: [ContactInfo], completion: @escaping ([Section]) -> Void)
}
