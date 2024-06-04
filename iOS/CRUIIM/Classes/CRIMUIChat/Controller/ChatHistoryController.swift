
import ChatLayout
import Foundation
import CRUICore

final class ChatHistoryController: ChatHisotryController {

    weak var delegate: ChatControllerDelegate?
            
    private let dispatchQueue = DispatchQueue(label: "ChatHistoryController", qos: .userInteractive)
    
    private let title: String
    private var messages: [MessageInfo] = []
    
    private var selecteMessages: [MessageInfo] = [] // 选中的消息id， 可转发、删除、引用消息

    init(title: String, messages: [MessageInfo]) {
        self.title = title
        self.appendConvertingToMessages(messages)
    }
    
    deinit {
        print("controller - deinit")
    }
    
    // MARK: 协议相关
    
    func loadInitialMessages(completion: @escaping ([Section]) -> Void) {
        propagateLatestMessages { sections in
            completion(sections)
        }
    }
    
    func getMessageInfo(ids: [String]) -> [MessageInfo] {
        return messages.filter({ ids.contains($0.clientMsgID) })
    }
    
    func getMergerMessageTitle() -> String {
        return title
    }
    
    func defaultSelecteMessage(with id: String?) {
        if let id {
            selecteMessages.removeAll()
            selecteMessage(with: id)
        } else {
            selecteMessages.removeAll()
        }
    }
    
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
    }
    
    func sendForwardMsg(_ contacts: [ContactInfo], completion: @escaping ([Section]) -> Void) {
        guard selecteMessages.isEmpty == false else { return }
        contacts.forEach { contact in
            if let recvID = contact.ID {
                selecteMessages.forEach { message in
                    IMController.shared.sendForwardMsg(forwardMessage: message,
                                                       to: recvID,
                                                       groupName: nil,
                                                       conversationType: contact.type == .groups ? .group : .c1v1) { [weak self] msg in
                        
                    } onComplete: { [weak self] msg in
//                        if recvID == self?.conversation.userID {
//                            self?.appendMessage(msg, completion: completion)
//                        }
                        IMController.shared.syncLocalMsgSentSubject.onNext(msg)
                    }
                }
            }
        }
        selecteMessages.removeAll()
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
            
            let cells = messagesSplitByDay.enumerated().map { index, message -> [Cell] in
                let bubble: Cell.BubbleType
                if index > 0 {
                    let preMessage = messagesSplitByDay[index - 1]
                    bubble = preMessage.owner == message.owner ? .tailed : .normal // 跟前一个相同，都是跟随的
                } else {
                    bubble = .normal
                }
                
                var messageCells = [Cell]()
                switch message.data {
                case let .quote(quoteSource):
                    // 处理引用消息类型
                    messageCells.append(.replyMessage(message, bubbleType: bubble))
                    messageCells.append(.quoteMessage(message, bubbleType: bubble))
                    if index < messagesSplitByDay.count - 1 {
                        messageCells.append(.separator)
                    }
                default:
                    messageCells.append(.message(message, bubbleType: bubble))
                    if index < messagesSplitByDay.count - 1 {
                        messageCells.append(.separator)
                    }
                }
                
                guard message.type != .outgoing else {
                    lastMessageStorage = message
                    return messageCells
                }
                
                if message.sessionType == .c1v1 {
                    return messageCells
                }
                
                return messageCells
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
                       type: .incoming,
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
            let duration = msg.soundElem!.duration
            let content = "[语音]".innerLocalized() + " " + #"\#(duration)""#
            
            let source = TextMessageSource(text: content)
            
            return .text(source)
            
        case .card:
            let cardElem = msg.cardElem!
            let content = "[名片]".innerLocalized() + " " + (cardElem.nickname ?? "")
            
            let source = TextMessageSource(text: content)
            
            return .text(source)
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
            
        case .merge:
            let mergeElem = msg.mergeElem!
            let source = MergerMessageSource(title: mergeElem.title, abstract: mergeElem.abstractList)
            
            return .merger(source)
            
        case .quote:
            let quoteElem = msg.quoteElem!
            let quoteMessage = quoteElem.quoteMessage!
            let contentType = quoteMessage.contentType
            // 这里对未知的引用消息类型加以保护
            guard contentType == .text || contentType == .image || contentType == .video || contentType == .audio || contentType == .card || contentType == .location || contentType == .quote || contentType == .revoke || contentType == .file || contentType == .merge else {
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
}


extension ChatHistoryController: ReloadDelegate {
    
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
        
    }
    
    func reeditMessage(with id: String) {
        
    }
}
