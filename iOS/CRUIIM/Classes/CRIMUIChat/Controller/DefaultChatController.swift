
import ChatLayout
import Foundation
import CRUICore

final class DefaultChatController: ChatController {
    
    weak var delegate: ChatControllerDelegate?
    
    private let dataProvider: DataProvider
    
    private var typingState: TypingState = .idle
    
    private let dispatchQueue = DispatchQueue(label: "DefaultChatController", qos: .userInteractive)
    
    private var lastReadIDs: [String] = []
    
    private var hasReadUserID: String? // 群组中已读人
    
    private var unReadCount: Int = 0 // 左上角的未读数
    
    private var lastReceivedString: String?
    
    private let receiverId: String // 接收人的uid
    
    private let senderID: String // 发送人的uid
    
    private let conversationType: ConversationType // 会话类型
    
    private let conversation: ConversationInfo
    
    private var groupInfo: GroupInfo? // 将其缓存
    
    private var groupMembers: [GroupMemberInfo]?
    
    private var otherInfo: FullUserInfo?
    
    private var messages: [MessageInfo] = []
    
    private var selecteMessages: [MessageInfo] = [] // 选中的消息id， 可转发、删除、引用消息

    private var selectedUsers: [String] = [] // 选中的成员, 可做为@成员
        
    init(dataProvider: DataProvider, senderID: String, receiverId: String, conversationType: ConversationType, conversation: ConversationInfo) {
        self.dataProvider = dataProvider
        self.receiverId = receiverId
        self.senderID = senderID
        self.conversationType = conversationType
        self.conversation = conversation
        
        switch conversationType {
        case .undefine:
            break
        case .c1v1:
            getOtherInfo { _ in }
        case .group, .group:
            getGrpInfo { _ in }
            getGroupMembers { _ in }
        case .notification:
            break
        }
        markMessageAsReaded()
        getUnReadTotalCount()
    }
    
    deinit {
        print("controller - deinit")
        markMessageAsReaded()
    }
    
    // MARK: 协议相关
    
    func loadInitialMessages(completion: @escaping ([Section]) -> Void) {
        // getBasicInfo { [weak self] in
        self.dataProvider.loadInitialMessages { [weak self] messages in
            self?.appendConvertingToMessages(messages)
            self?.markAllMessagesAsReceived { [weak self] in
                self?.markAllMessagesAsRead { [weak self] in
                    self?.propagateLatestMessages { sections in
                        completion(sections)
                    }
                }
            }
        }
        // }
    }
    
    func loadPreviousMessages(completion: @escaping ([Section]) -> Void) {
        dataProvider.loadPreviousMessages(completion: { [weak self] messages in
            self?.appendConvertingToMessages(messages)
            self?.markAllMessagesAsReceived { [weak self] in
                self?.markAllMessagesAsRead { [weak self] in
                    self?.propagateLatestMessages { [weak self]  sections in
                        completion(sections)
                    }
                }
            }
        })
    }
    
    func defaultSelecteMessage(with id: String?) {
        if let id {
            selecteMessages.removeAll()
            selecteMessage(with: id)
        } else {
            selecteMessages.removeAll()
        }
    }
    
    func defaultSelecteUsers(with usersID: [String]) {
        selectedUsers.append(contentsOf: usersID)
    }
    
    func clearSelectedStatus() {
        resetSelectedStatus()
        repopulateMessages()
    }
    
    // 重置原始消息的选中状态
    private func resetSelectedStatus() {
        messages.forEach { $0.isSelected = false }
    }
    
    func deleteMsgs() {
        deleteMsgs(messages: selecteMessages) { [weak self] messages in
            self?.messages = self?.messages.filter { msg in
                !messages.contains { delMsg in
                    msg.clientMsgID == delMsg.clientMsgID
                }
            } ?? []
            self?.repopulateMessages()
            self?.selecteMessages.removeAll()
        }
    }
    
    func deleteAllMsg() {
        messages.removeAll()
        repopulateMessages()
    }
    
    func getConversation() -> ConversationInfo {
        return conversation
    }
    
    func getGroupMembers(completion: @escaping ([GroupMemberInfo]) -> Void) {
        if groupMembers == nil {
            IMController.shared.getGrpMemberList(groupId: receiverId,
                                                   filter: .all,
                                                   offset: 0,
                                                   count: 10000) { [weak self] ms in
                completion(ms)
                self?.groupMembers = ms
            }
        } else {
            completion(groupMembers!)
        }
    }
    
    func getMentionUsers(completion: @escaping ([GroupMemberInfo]) -> Void) {
        getGroupMembers { ms in
            var us = ms.filter({ $0.userID != IMController.shared.userID })
            let metionAll = GroupMemberInfo()
            metionAll.userID = "-1"
            metionAll.nickname = "所有人"
            us.insert(metionAll, at: 0)
            
            completion(us)
        }
    }
    
    func getMessageInfo(ids: [String]) -> [MessageInfo] {
        return messages.filter({ ids.contains($0.clientMsgID) })
    }
    
    private func getBasicInfo(completion: @escaping () -> Void) {
        if conversationType == .c1v1 {
            getOtherInfo { _ in
                completion()
            }
        } else {
            getGrpInfo { _ in
                completion()
            }
        }
    }
    
    func getOtherInfo(completion: @escaping (FullUserInfo) -> Void) {
        if otherInfo == nil {
            IMController.shared.getFriendsInfo(userIDs: [receiverId]) { [weak self] info in
                guard let r = info.first else { return }
                completion(r)
                self?.otherInfo = r
            }
        } else {
            completion(otherInfo!)
        }
    }

    func getGrpInfo(requirePull: Bool = false, completion: @escaping (GroupInfo) -> Void) {
        guard (groupInfo == nil || requirePull) else {
            completion(groupInfo!)
            return
        }
        dataProvider.getGrpInfo { [weak self] group in
            completion(group)
            self?.groupInfo = group
        }
    }
    
    func getSelectedMessages() -> [MessageInfo] {
        return selecteMessages
    }
    
    func getSelfInfo() -> UserInfo? {
        return IMController.shared.currentUserRelay.value
    }
    
    func addFriend(onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: @escaping CallBack.ErrorOptionalReturnVoid) {
        let reqMsg = "\(IMController.shared.currentUserRelay.value!.nickname)请求添加你为好友"
        IMController.shared.addFriend(uid: receiverId, reqMsg: reqMsg, onSuccess: onSuccess, onFailure: onFailure)
    }
    
    func isMsgRevokedExpired(timestamp: TimeInterval) -> Bool {
        let currentTimestamp = Date().timeIntervalSince1970
        let timeInterval = currentTimestamp - timestamp
        
        if timeInterval > 120 {
            return true
        } else {
            return false
        }
    }
    
    // 主动撤回
    func revokeMsg(with id: String) {
        if var msg = messages.first(where: { $0.clientMsgID == id}) {
            
            if isMsgRevokedExpired(timestamp: msg.createTime/1000) == true {
                return
            }
            
            IMController.shared.revokeMsg(conversationID: conversation.conversationID, clientMsgID: msg.clientMsgID) { [weak self] r in
                msg.contentType = .revoke
                
                let info = MessageRevoked()
                info.clientMsgID = msg.clientMsgID
                info.revokerNickname = IMController.shared.currentUserRelay.value?.nickname
                info.revokerID = IMController.shared.currentUserRelay.value?.userID
                info.sourceMessageSendID = msg.sendID
                info.sourceMessageSendTime = msg.sendTime
                info.sourceMessageSenderNickname = msg.senderNickname
                info.revokeTime = NSDate().timeIntervalSince1970
                info.sessionType = msg.sessionType
                msg.content = JsonTool.toJson(fromObject: info)
                
                self?.repopulateMessages(requiresIsolatedProcess: true)
            }
        }
    }
    
    func markMessageAsReaded(messageID: String? = nil, completion: (() -> Void)? = nil) {
        IMController.shared.markConversationMsg(byConID: conversation.conversationID, msgIDList: messageID == nil ? [] : [messageID!]) { [weak self] r in
            if messageID != nil {
                self?.messages.first(where: { $0.clientMsgID == messageID })?.hasReadTime = Date().timeIntervalSince1970 * 1000
                self?.repopulateMessages(requiresIsolatedProcess: true)
            }
            completion?()
        }
    }
    
    func markAudio(messageId: String, isPlaying: Bool) {
        var changed = false
        for (index, item) in messages.enumerated() {
            if item.clientMsgID == messageId {
                if item.isPlaying != isPlaying {
                    item.isPlaying = isPlaying
                    changed = true
                }
            } else {
                item.isPlaying = false
            }
        }
        if changed {
            repopulateMessages(requiresIsolatedProcess: true)
        }
    }
    
    // MARK: 发送消息
    
    func typing(doing: Bool) {
        IMController.shared.typingStatusUpdate(recvID: receiverId, msgTips: doing ? "yes" : "no")
    }
    
    func sendForwardMsg(_ contacts: [ContactInfo], completion: @escaping ([Section]) -> Void) {
        guard let forwardMsg = selecteMessages.first else { return }
        contacts.forEach { contact in
            if let recvID = contact.ID {
                IMController.shared.sendForwardMsg(forwardMessage: forwardMsg,
                                                   to: recvID, 
                                                   groupName: groupInfo?.groupName,
                                                   conversationType: contact.type == .groups ? .group : .c1v1) { [weak self] msg in
                    
                } onComplete: { [weak self] msg in
                    if recvID == self?.conversation.userID {
                        self?.appendMessage(msg, completion: completion)
                    }
                }
            }
        }
        selecteMessages.removeAll()
    }
    
    func sendMergeForwardMsg(_ contacts: [ContactInfo], completion: @escaping ([Section]) -> Void) {
        
    }
    
    func sendCardMsg(_ contact: ContactInfo, completion: @escaping ([Section]) -> Void) {
        let cardElem = CardElem(userID: contact.ID!, nickname: contact.name!, faceURL: contact.faceURL)
        IMController.shared.sendCardMsg(card: cardElem, 
                                        to: receiverId,
                                        groupName: groupInfo?.groupName,
                                        conversationType: self.conversationType) { [weak self] msg in
            
        } onComplete: { [weak self] msg in
            self?.appendMessage(msg, completion: completion)
        }
    }
    
    func sendLocationMsg(_ latitude: Double, _ longitude: Double, _ desc: String, completion: @escaping ([Section]) -> Void) {
        IMController.shared.sendLocation(latitude: latitude, 
                                         longitude: longitude,
                                         desc: desc,
                                         to: receiverId,
                                         groupName: groupInfo?.groupName,
                                         conversationType: self.conversationType) { [weak self] msg in
            
        } onComplete: { [weak self] msg in
            self?.appendMessage(msg, completion: completion)
        }
    }
    
    func sendMsg(_ data: Message.Data, completion: @escaping ([Section]) -> Void) {
        switch data {
        case .text(let source):
            // 如果有选中的消息，说明是引用消息
            let quoteMsg = selecteMessages.first
            sendText(text: source.text, quoteMessage: quoteMsg, completion: completion)
            
        case .url(let url, isLocallyStored: _):
            break
            
        case .image(let source, isLocallyStored: _):
            sendImage(source: source, completion: completion)
            
        case .video(let source, isLocallyStored: _):
            // 发送的时候，图片选择器选择以后，传入的是路径
            sendVideo(source: source, completion: completion)
            
        case .file(let source, isLocallyStored: _):
            sendFile(source: source, completion: completion)
            
        case .audio(let source, isLocallyStored: _):
            sendAudio(source: source, completion: completion)
            
        case .attributeText(_), .custom(_), .quote(_), .card(_), .location(_):
            break
        }
    }
    
    private func sendText(text: String, to: String? = nil, conversationType: ConversationType? = nil, quoteMessage: MessageInfo? = nil, completion: (([Section]) -> Void)?) {
        IMController.shared.sendTextMsg(text: text,
                                        quoteMessage: quoteMessage,
                                        to: to ?? receiverId,
                                        groupName: groupInfo?.groupName,
                                        conversationType: conversationType ?? self.conversationType) { [weak self] msg in
//            self?.appendMessage(msg, completion: completion) // 刷新太快，界面不好看
        } onComplete: { [weak self] msg in
            guard let completion else { return }
            self?.appendMessage(msg, completion: completion)
            self?.selecteMessages.removeAll() // 移除选中的引用消息
            self?.selectedUsers.removeAll()
        }
    }
    
    private func sendImage(source: MediaMessageSource, completion: @escaping ([Section]) -> Void) {
        
        let path = source.source.url!.path
        let image = UIImage(contentsOfFile: path)
        
        if let image {
            cacheImage(image: image, path: path)
        }
        
        IMController.shared.sendImageMsg(path: path,
                                         to: receiverId,
                                         groupName: groupInfo?.groupName,
                                         conversationType: conversationType) { [weak self] msg in
            self?.appendMessage(msg, completion: completion)
        } onComplete: { [weak self] msg in
            if let image, let url = msg.pictureElem?.sourcePicture?.url {
                self?.cacheImage(image: image, path: url)
            }
            self?.replaceMessage(msg)
            self?.repopulateMessages()
        }
    }
    
    private func sendVideo(source: MediaMessageSource, completion: @escaping ([Section]) -> Void) {
         
        let sourcPath = source.source.url?.path ?? ""
        let thumbPath = source.thumb!.url.path
        let image = UIImage(contentsOfFile: thumbPath)
        
        if let image {
            cacheImage(image: image, path: thumbPath)
        }
        
        IMController.shared.sendVideoMsg(path: sourcPath,
                                         duration: source.duration!,
                                         snapshotPath: thumbPath,
                                         to: receiverId,
                                         groupName: groupInfo?.groupName,
                                         conversationType: conversationType) { [weak self] msg in
            self?.appendMessage(msg, completion: completion)
        } onComplete: { [weak self] msg in
            if let image, let snapshotUrl = msg.videoElem?.snapshotUrl {
                self?.cacheImage(image: image, path: snapshotUrl)
            }
            self?.replaceMessage(msg)
            self?.repopulateMessages()
        }
    }
    
    func sendFile(source: MediaMessageSource, completion: @escaping ([Section]) -> Void) {
        
        let sourcPath = source.source.url?.path ?? ""
        let relativePath = source.source.relativePath ?? ""
        let fileName = source.fileName ?? ""
        let fileMagager = FileManager.default
        if fileMagager.fileExists(atPath: sourcPath) {
            cacheFile(filePath: relativePath, path: sourcPath)
        }
        
        IMController.shared.sendFileMsg(filePath: sourcPath,
                                        fileName: fileName,
                                        to: receiverId,
                                        groupName: groupInfo?.groupName,
                                        conversationType: self.conversationType) { [weak self] msg in
            self?.appendMessage(msg, completion: completion)
        } onComplete: { [weak self] msg in
            if let fileUrl = msg.fileElem?.sourceUrl {
                self?.cacheFile(filePath: relativePath, path: fileUrl)
            }
            self?.replaceMessage(msg)
            self?.repopulateMessages()
        }
    }
    
    func sendAudio(source: MediaMessageSource, completion: @escaping ([Section]) -> Void) {
        let sourcPath = source.source.url?.path ?? ""
        let relativePath = source.source.relativePath ?? ""
        let fileMagager = FileManager.default
        if fileMagager.fileExists(atPath: sourcPath) {
            cacheFile(filePath: relativePath, path: sourcPath)
        }
        
        IMController.shared.sendAudioMsg(path: sourcPath,
                                         duration: source.duration!,
                                         to: receiverId,
                                         groupName: groupInfo?.groupName,
                                         conversationType: self.conversationType) { [weak self] msg in
            self?.appendMessage(msg, completion: completion)
        } onComplete: { [weak self] msg in
            if let fileUrl = msg.soundElem?.sourceUrl {
                self?.cacheFile(filePath: relativePath, path: fileUrl)
            }
            self?.replaceMessage(msg)
            self?.repopulateMessages()
        }
    }
    
    private func cacheImage(image: UIImage, path: String) {
        guard let url = URL(string: path) else { return }
        
        do {
            try imageCache.store(entity: image, for: CacheableImageKey(url: url))
        } catch (let e) {
            print("cache image failure:\(e)")
        }
    }
    
    private func cacheFile(filePath: String, path: String) {
        guard let path = path.addingPercentEncoding(withAllowedCharacters: .urlQueryAllowed), let url = URL(string: path) else { return }
        
        do {
            try fileCache.store(entity: filePath, for: CacheableFileKey(url: url))
        } catch (let e) {
            print("cache file failure:\(e)")
        }
    }
    
    // MARK: 更新数据源
    private func appendMessage(_ message: MessageInfo, completion: @escaping ([Section]) -> Void) {
        // 刷新数据源
        var exist = false
        
        for (i, item) in messages.enumerated() {
            if item.clientMsgID == message.clientMsgID {
                messages[i] = message
                exist = true
                break
            }
        }
        
        if !exist {
            messages.append(message)
        }
        
        // 刷新界面
        propagateLatestMessages(completion: completion)
    }
    
    private func replaceMessage(_ message: MessageInfo) {
        for (i, item) in messages.enumerated() {
            if item.clientMsgID == message.clientMsgID {
                messages[i] = message
                break
            }
        }
    }
    
    private func appendConvertingToMessages(_ rawMessages: [MessageInfo]) {
        var messages = messages
        messages.append(contentsOf: rawMessages)
        self.messages = messages.sorted(by: { $0.sendTime < $1.sendTime })
    }
    
    private func propagateLatestMessages(completion: @escaping ([Section]) -> Void) {

        var lastMessageStorage: Message?
        
        dispatchQueue.async { [weak self] in
            guard let self else {
                return
            }
            
            let messagesSplitByDay = self.messages
                .map { self.convertMessage($0) }
                .reduce(into: [[Message]]()) { result, message in
                    guard var section = result.last,
                          let prevMessage = section.last else {
                        let section = [message]
                        result.append(section)
                        return
                    }
                    if Calendar.current.isDate(prevMessage.date, equalTo: message.date, toGranularity: .hour) {
                        section.append(message)
                        result[result.count - 1] = section
                    } else {
                        let section = [message]
                        result.append(section)
                    }
                }
            
            let cells = messagesSplitByDay.enumerated().map { index, messages -> [Cell] in // 按天划分
                var cells: [Cell] = Array(messages.enumerated().map { index, message -> [Cell] in // 按发送者划分
                    
                    if message.contentType == .system, case .attributeText(let value) = message.data {
                        
                        let systemCell = Cell.systemMessage(SystemGroup(id: message.id, value: value))
                        return [systemCell]
                    }
                    
                    let bubble: Cell.BubbleType
                    if index < messages.count - 1 {
                        let nextMessage = messages[index + 1]
                        bubble = nextMessage.owner == message.owner ? .normal : .tailed
                    } else {
                        bubble = .tailed
                    }
                    
                    var messageCells = [Cell]()
                    switch message.data {
                    case let .quote(quoteSource):
                        // 处理引用消息类型
                        messageCells.append(.replyMessage(message, bubbleType: bubble))
                        messageCells.append(.quoteMessage(message, bubbleType: bubble))
                    default:
                        messageCells.append(.message(message, bubbleType: bubble))
                    }
                    
                    guard message.type != .outgoing else {
                        lastMessageStorage = message
                        return messageCells
                    }
                    
                    if message.sessionType == .c1v1 {
                        return messageCells
                    }
                    
                    //let titleCell = Cell.messageGroup(MessageGroup(id: message.id, title: "\(message.owner.name)", type: message.type))
                    //messageCells.insert(titleCell, at: 0)
                    return messageCells
                }.joined())
                
                if let firstMessage = messages.first {
                    let dateCell = Cell.date(DateGroup(id: firstMessage.id, date: firstMessage.date))
                    cells.insert(dateCell, at: 0)
                }
                
                if self.typingState == .typing,
                   index == messagesSplitByDay.count - 1 {
                    cells.append(.typingIndicator)
                }
                
                return cells // Section(id: sectionTitle.hashValue, title: sectionTitle, cells: cells)
            }.joined()
            
            DispatchQueue.main.async { [weak self] in
                guard self != nil else {
                    return
                }
                completion([Section(id: 0, title: "", cells: Array(cells))])
            }
        }
        
    }
    
    private func convertMessage(_ msg: MessageInfo) -> Message {
        
        var contentType = msg.contentType.rawValue > MessageContentType.face.rawValue ? MessageRawType.system : MessageRawType.normal
        if msg.contentType == .custom {
            if msg.customElem?.type == .blockedByFriend || msg.customElem?.type == .mutedInGroup {
                contentType = MessageRawType.system
            }
        }
        
        var status: MessageStatus?
        if msg.isOutgoing {
            switch msg.status {
            case .sending:
                status = .sending
            case .sendFailure:
                status = .sendFailure
            default:
                status = msg.isRead ? .read : .sent
            }
        } else {
            status = msg.isRead ? .read : .received
        }
        
        return Message(id: msg.clientMsgID,
                       date: Date(timeIntervalSince1970: msg.sendTime / 1000),
                       contentType: contentType,
                       data: convert(msg),
                       owner: User(id: msg.sendID, name: msg.senderNickname ?? "", faceURL: msg.senderFaceUrl),
                       type: msg.isOutgoing ? .outgoing : .incoming,
                       status: status!,
                       isSelected: msg.isSelected,
                       isAnchor: msg.isAnchor,
                       sessionType: msg.sessionType)
    }
    
    private func convert(_ msg: MessageInfo) -> Message.Data {
        var isLocalPath = msg.serverMsgID == nil // 本地发送，先把消息渲染到界面上；发送成功以后，再替换原消息
        
        switch msg.contentType {
            
        case .text:
            let textElem = msg.textElem!
            
            let source = TextMessageSource(text: textElem.content)
            
            return .text(source)
            
        case .image:
            let pictureElem = msg.pictureElem!
            let url = isLocalPath ? URL(string: pictureElem.sourcePath!)! : URL(string: pictureElem.snapshotPicture!.url!)!
            let isPresentLocally = imageCache.isEntityCached(for: CacheableImageKey(url: url))
            let source = MediaMessageSource(source: MediaMessageSource.Info(url: url))
            
            return .image(source, isLocallyStored: isPresentLocally)
            
        case .video:
            let videoElem = msg.videoElem!
            let url = isLocalPath ? URL(string: videoElem.videoPath!) : URL(string: videoElem.videoUrl!)!
            let thumbURL = isLocalPath ? URL(string: videoElem.snapshotPath!)! : URL(string: videoElem.snapshotUrl!)!
            let isPresentLocally = imageCache.isEntityCached(for: CacheableImageKey(url: thumbURL)) // 这个值，可以用来判断差分是否有效
            let duration = msg.videoElem!.duration
            
            let source = MediaMessageSource(source: MediaMessageSource.Info(url: url),
                                            thumb: MediaMessageSource.Info(url: thumbURL),
                                            duration: duration)
            
            return .video(source, isLocallyStored: isPresentLocally)
            
        case .audio:
            let soundElem = msg.soundElem!
            let url = isLocalPath ? URL(string: soundElem.soundPath!)! : URL(string: soundElem.sourceUrl!)!
            let isPresentLocally = fileCache.isEntityCached(for: CacheableFileKey(url: url))
            let duration = msg.soundElem!.duration
            
            let source = MediaMessageSource(source: MediaMessageSource.Info(url: url),
                                            duration: duration,
                                            isPlaying: msg.isPlaying)
            
            return .audio(source, isLocallyStored: isPresentLocally)
            
        case .card:
            let cardElem = msg.cardElem!
            let user = User(id: cardElem.userID, name: cardElem.nickname ?? "", faceURL: cardElem.faceURL)
            
            let source = CardMessageSource(user: user)
            
            return .card(source)
            
        case .location:
            let locationElem = msg.locationElem!
            var desc = LocationMessageSource.Desc(name: locationElem.desc)
            if let descStr = locationElem.desc, let obj = JsonTool.fromJson(descStr ?? "", toClass: LocationMessageSource.Desc.self) {
                desc = obj
            }
            let source = LocationMessageSource(longitude: locationElem.longitude, latitude: locationElem.latitude, desc: desc)
            
            return .location(source)
            
        case .file:
            let fileElem = msg.fileElem!
            var urlStr = isLocalPath ? fileElem.filePath! : fileElem.sourceUrl!
            urlStr = urlStr.addingPercentEncoding(withAllowedCharacters: .urlQueryAllowed) ?? ""
            let fileURL = URL(string: urlStr)!
            let isPresentLocally = fileCache.isEntityCached(for: CacheableFileKey(url: fileURL))
            let source = MediaMessageSource(source: MediaMessageSource.Info(url: fileURL),
                                            fileSize: fileElem.fileSize,
                                            fileName: fileElem.fileName)
            
            return .file(source, isLocallyStored: isPresentLocally)
            
        case .quote:
            let quoteElem = msg.quoteElem!
            let quoteMessage = quoteElem.quoteMessage!
            let contentType = quoteMessage.contentType
            // 这里对未知的引用消息类型加以保护
            guard contentType == .text || contentType == .image || contentType == .video || contentType == .audio || contentType == .card || contentType == .location || contentType == .quote || contentType == .revoke || contentType == .file else {
                let value = MessageHelper.getSystemNotificationOf(message: msg, isSingleChat: msg.sessionType == .c1v1)
                
                return .attributeText(value!)
            }
            
            // ⚠️ convert(quoteMessage) 是转换被引用消息
            let quotedSource = (contentType == .revoke) ? .text(TextMessageSource(text: "引用的消息已撤回".innerLocalized())) : convert(quoteMessage)
            let source = QuoteMessageSource(text: quoteElem.text!, quoteUser: quoteMessage.senderNickname!, quoteData: quotedSource)
            
            return .quote(source)

        case .groupAnnouncement:
            let noti = msg.notificationElem!

            let source = TextMessageSource(text: noti.group?.notification ?? "", type: .notice)
            
            return .text(source)
            
        case .custom:
            let value = MessageHelper.getCustomMessageValueOf(message: msg)
            if msg.customElem?.type == .blockedByFriend || msg.customElem?.type == .mutedInGroup {
                return .attributeText(value)
            }
            
            let source = CustomMessageSource(data: msg.customElem?.data, attributedString: value)
            
            return .custom(source)
        default:
            let value = MessageHelper.getSystemNotificationOf(message: msg, isSingleChat: msg.sessionType == .c1v1)
            
            return .attributeText(value!)
        }
    }
        
    private func repopulateMessages(requiresIsolatedProcess: Bool = false) {
        propagateLatestMessages { [weak self] sections in
            self?.delegate?.update(with: sections, requiresIsolatedProcess: requiresIsolatedProcess)
        }
    }
    
    // MARK: 操作消息
    
    private func deleteMsgs(messages: [MessageInfo], completion: @escaping ([MessageInfo]) -> Void) {
        var result: [MessageInfo] = []
        var count = 0
        
        for (i, msg) in messages.enumerated() {
            IMController.shared.deleteMsg(conversation: conversation.conversationID,
                                              clientMsgID: msg.clientMsgID) { r in
                result.append(msg)
                count += 1
                
                if count == messages.count {
                    completion(result)
                }
            } onFailure: { errCode, errMsg in
                count += 1
                
                if count == messages.count {
                    completion(result)
                }
            }
        }
    }
    
    // MARK: 其它操作
    private func getUnReadTotalCount() {
        IMController.shared.getTotalUnreadMsgCount { [weak self] count in
            guard count > 0 else { return }
            let unReadCount = count - (self?.conversation.unreadCount ?? 0)
            self?.unReadCount = unReadCount
            guard unReadCount > 0 else { return }
            //self?.delegate?.updateUnreadCount(count: unReadCount)
            IMController.shared.totalUnreadSubject.onNext(unReadCount)
        }
    }
}

extension DefaultChatController: DataProviderDelegate {

    func isInGroup(with isIn: Bool) {
        delegate?.isInGroup(with: isIn)
    }
    
    func received(message: MessageInfo) {
        // 收到当前界面的消息
        if (conversation.conversationType == .c1v1 && message.sessionType == .c1v1 && conversation.userID == message.sendID) ||
            (conversation.conversationType == .group && conversation.groupID == message.groupID) {
            appendConvertingToMessages([message])
            markAllMessagesAsReceived { [weak self] in
                self?.markAllMessagesAsRead { [weak self] in
                    self?.repopulateMessages()
                }
            }
        } else {
         // 左上角未读数加1
            unReadCount += 1
            //delegate?.updateUnreadCount(count: unReadCount)
        }
    }
    
    func receivedRevokedInfo(info: MessageRevoked) {
        if var msg = messages.first(where: { $0.clientMsgID == info.clientMsgID }) {
            msg.contentType = .revoke
            msg.content = JsonTool.toJson(fromObject: info)
            repopulateMessages()
        }
    }
    
    func typingStateChanged(to state: TypingState) {
        typingState = state
        repopulateMessages()
    }
    
    func lastReadIdsChanged(to ids: [String], readUserID: String?) {
        lastReadIDs = ids
        hasReadUserID = readUserID
        markAllMessagesAsRead { [weak self] in
            self?.repopulateMessages()
        }
    }
    
    func lastReceivedIdChanged(to id: String) {
        lastReceivedString = id
        markAllMessagesAsReceived { [weak self] in
            self?.repopulateMessages()
        }
    }
    
    func markAllMessagesAsReceived(completion: @escaping () -> Void) {
        guard let lastReceivedString else {
            completion()
            return
        }
        
        markMessageAsReaded(messageID: lastReceivedString, completion: completion)
    }
    
    func markAllMessagesAsRead(completion: @escaping () -> Void) {
        guard !lastReadIDs.isEmpty else {
            completion()
            return
        }
        dispatchQueue.async { [weak self] in
            guard let self else {
                return
            }
            for id in lastReadIDs {
                if let index = self.messages.firstIndex(where: { $0.clientMsgID == id}) {
                    self.messages[index].isRead = true
                }
            }
            DispatchQueue.main.async {
                completion()
            }
        }
    }
    
}

extension DefaultChatController: ReloadDelegate {
    
    func reloadMessage(with id: String) {
        repopulateMessages()
    }
    
    func didTapAvatar(with id: String) {
        delegate?.didTapAvatar(with: id)
    }
    
    func didTapContent(with id: String, data: Message.Data) {
        
        delegate?.didTapContent(with: id, data: data)
    }
    
    func didLongPressContent(with id: String, bubbleView: UIView, data: Message.Data) {
        delegate?.didLongPressContent(with: id, bubbleView: bubbleView, data: data)
    }
    
    func removeMessage(messageID: String) {
        defaultSelecteMessage(with: messageID)
        deleteMsgs()
    }
    
    func reeditMessage(with id: String) {
        delegate?.reeditMessage(with: id)
    }
}

extension DefaultChatController: EditingAccessoryControllerDelegate {

    func selecteMessage(with id: String) {
        // 将选中的消息计入，用来删除，转发等
        if let index = selecteMessages.firstIndex(where: { $0.clientMsgID == id}) {
            selecteMessages.remove(at: index)
            messages.first(where: { $0.clientMsgID == id})?.isSelected = false
        } else {
            if let item = messages.first(where: { $0.clientMsgID == id}) {
                item.isSelected = true // 多选的时候用来记录选中项，主要是cell重用问题。
                selecteMessages.append(item)
            }
        }
        
        repopulateMessages(requiresIsolatedProcess: true)
    }
}
