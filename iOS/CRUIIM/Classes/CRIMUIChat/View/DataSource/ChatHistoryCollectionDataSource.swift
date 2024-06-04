

import ChatLayout
import Foundation
import UIKit

final class ChatHistoryCollectionDataSource: NSObject, ChatCollectionDataSource {
    
    private unowned var reloadDelegate: ReloadDelegate
        
    var sections: [Section] = [] {
        didSet {
            oldSections = oldValue
        }
    }
    
    private var oldSections: [Section] = []
    
    init(reloadDelegate: ReloadDelegate) {
        self.reloadDelegate = reloadDelegate
    }
    
    func prepare(with collectionView: UICollectionView) {
        collectionView.register(TextMessageCollectionCell.self, forCellWithReuseIdentifier: TextMessageCollectionCell.reuseIdentifier)
        collectionView.register(ImageCollectionCell.self, forCellWithReuseIdentifier: ImageCollectionCell.reuseIdentifier)
        collectionView.register(VideoCollectionCell.self, forCellWithReuseIdentifier: VideoCollectionCell.reuseIdentifier)
        collectionView.register(AudioCollectionCell.self, forCellWithReuseIdentifier: AudioCollectionCell.reuseIdentifier)
        collectionView.register(CardCollectionCell.self, forCellWithReuseIdentifier: CardCollectionCell.reuseIdentifier)
        collectionView.register(LocationCollectionCell.self, forCellWithReuseIdentifier: LocationCollectionCell.reuseIdentifier)
        collectionView.register(FileCollectionCell.self, forCellWithReuseIdentifier: FileCollectionCell.reuseIdentifier)
        collectionView.register(MergerCollectionCell.self, forCellWithReuseIdentifier: MergerCollectionCell.reuseIdentifier)

        collectionView.register(TextQuoteMessageCollectionCell.self, forCellWithReuseIdentifier: TextQuoteMessageCollectionCell.reuseIdentifier)
        collectionView.register(ImageQuoteCollectionCell.self, forCellWithReuseIdentifier: ImageQuoteCollectionCell.reuseIdentifier)
        collectionView.register(VideoQuoteCollectionCell.self, forCellWithReuseIdentifier: VideoQuoteCollectionCell.reuseIdentifier)
        collectionView.register(AudioQuoteCollectionCell.self, forCellWithReuseIdentifier: AudioQuoteCollectionCell.reuseIdentifier)
        collectionView.register(CardQuoteCollectionCell.self, forCellWithReuseIdentifier: CardQuoteCollectionCell.reuseIdentifier)
        collectionView.register(LocationQuoteCollectionCell.self, forCellWithReuseIdentifier: LocationQuoteCollectionCell.reuseIdentifier)
        collectionView.register(FileQuoteCollectionCell.self, forCellWithReuseIdentifier: FileQuoteCollectionCell.reuseIdentifier)
        collectionView.register(MergerQuoteCollectionCell.self, forCellWithReuseIdentifier: MergerQuoteCollectionCell.reuseIdentifier)

        collectionView.register(SeparatorCollectionCell.self, forCellWithReuseIdentifier: SeparatorCollectionCell.reuseIdentifier)
        collectionView.register(UserTitleCollectionCell.self, forCellWithReuseIdentifier: UserTitleCollectionCell.reuseIdentifier)
        collectionView.register(TitleCollectionCell.self, forCellWithReuseIdentifier: TitleCollectionCell.reuseIdentifier)
        collectionView.register(TypingIndicatorCollectionCell.self, forCellWithReuseIdentifier: TypingIndicatorCollectionCell.reuseIdentifier)
        collectionView.register(TextTitleView.self, forSupplementaryViewOfKind: UICollectionView.elementKindSectionHeader, withReuseIdentifier: TextTitleView.reuseIdentifier)
        collectionView.register(TextTitleView.self, forSupplementaryViewOfKind: UICollectionView.elementKindSectionFooter, withReuseIdentifier: TextTitleView.reuseIdentifier)
        if #available(iOS 13.0, *) {
            collectionView.register(URLCollectionCell.self, forCellWithReuseIdentifier: URLCollectionCell.reuseIdentifier)
        }
    }
    
    private func createTextCell(collectionView: UICollectionView,
                                messageId: String,
                                isSelected: Bool,
                                isPrivateChat: Bool,
                                isReplyMessage: Bool = false,
                                indexPath: IndexPath,
                                text: String? = nil,
                                attributedString: NSAttributedString? = nil,
                                anchor: Bool = false,
                                date: Date,
                                alignment: ChatItemAlignment,
                                user: User,
                                bubbleType: Cell.BubbleType,
                                status: MessageStatus,
                                messageType: MessageType) -> UICollectionViewCell {
        
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: TextMessageCollectionCell.reuseIdentifier, for: indexPath) as! TextMessageCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isReplyMessage: isReplyMessage)
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = TextMessageController(text: text,
                                               messageId: messageId,
                                               attributedString: attributedString,
                                               highlight: anchor,
                                               type: messageType,
                                               bubbleController: buildBlankBubbleController(bubbleView: bubbleView,
                                                                                           messageType: messageType,
                                                                                           bubbleType: bubbleType))
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView
        
        return cell
    }
    
    private func createTextQuoteCell(collectionView: UICollectionView,
                                messageId: String,
                                isSelected: Bool,
                                isPrivateChat: Bool,
                                indexPath: IndexPath,
                                text: String? = nil,
                                attributedString: NSAttributedString? = nil,
                                quoteUserName: String,
                                anchor: Bool = false,
                                date: Date,
                                alignment: ChatItemAlignment,
                                user: User,
                                bubbleType: Cell.BubbleType,
                                status: MessageStatus,
                                messageType: MessageType) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: TextQuoteMessageCollectionCell.reuseIdentifier, for: indexPath) as! TextQuoteMessageCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)

        let bubbleView = cell.customView.customView.customView
        let textController = TextMessageController(text: text,
                                                   messageId: messageId,
                                                   attributedString: attributedString,
                                                   highlight: anchor,
                                                   type: messageType,
                                                   bubbleController: buildTextBubbleController(bubbleView: bubbleView,
                                                                                               messageType: messageType,
                                                                                               bubbleType: bubbleType, isQuoted: true),
                                                   senderNickname: quoteUserName,
                                                   isQuoted: true)
        bubbleView.customView.setup(with: textController)
        textController.delegate = reloadDelegate
        textController.view = bubbleView.customView
        cell.delegate = bubbleView.customView
        
        return cell
    }
    
    @available(iOS 13, *)
    private func createURLCell(collectionView: UICollectionView, messageId: String, isSelected: Bool, isPrivateChat: Bool, indexPath: IndexPath, url: URL, date: Date, alignment: ChatItemAlignment, user: User, bubbleType: Cell.BubbleType, status: MessageStatus, messageType: MessageType) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: URLCollectionCell.reuseIdentifier, for: indexPath) as! URLCollectionCell
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = URLController(url: url,
                                       messageId: messageId,
                                       bubbleController: buildBezierBubbleController(for: bubbleView, messageType: messageType, bubbleType: bubbleType))
        
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        controller.delegate = reloadDelegate
        cell.delegate = bubbleView.customView
        
        return cell
    }
    
    private func createImageCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> ImageCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: ImageCollectionCell.reuseIdentifier, for: indexPath) as! ImageCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = ImageController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildBezierBubbleController(for: bubbleView, messageType: messageType, bubbleType: bubbleType))
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createImageQuoteCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 quoteUserName: String,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> ImageQuoteCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: ImageQuoteCollectionCell.reuseIdentifier, for: indexPath) as! ImageQuoteCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = ImageController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true), senderNickname: quoteUserName)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.quoteView = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createVideoCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> VideoCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: VideoCollectionCell.reuseIdentifier, for: indexPath) as! VideoCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = VideoController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildBezierBubbleController(for: bubbleView, messageType: messageType, bubbleType: bubbleType))
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createVideoQuoteCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 quoteUserName: String,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> VideoQuoteCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: VideoQuoteCollectionCell.reuseIdentifier, for: indexPath) as! VideoQuoteCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = VideoController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true), senderNickname: quoteUserName)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.quoteView = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createAudioCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> AudioCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: AudioCollectionCell.reuseIdentifier, for: indexPath) as! AudioCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isAudioMsg: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = AudioController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView,
                                                                                     messageType: messageType,
                                                                                     bubbleType: bubbleType),
                                         messageType: messageType)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createAudioQuoteCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 quoteUserName: String,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> AudioQuoteCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: AudioQuoteCollectionCell.reuseIdentifier, for: indexPath) as! AudioQuoteCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = AudioController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView,
                                                                                     messageType: messageType,
                                                                                     bubbleType: bubbleType, isQuoted: true), senderNickname: quoteUserName)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.quoteView = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createCardCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: CardMessageSource,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> CardCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: CardCollectionCell.reuseIdentifier, for: indexPath) as! CardCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = CardController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true))
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createCardQuoteCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: CardMessageSource,
                                 quoteUserName: String,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> CardQuoteCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: CardQuoteCollectionCell.reuseIdentifier, for: indexPath) as! CardQuoteCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = CardController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true), senderNickname: quoteUserName, isQuoted: true)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.quoteView = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createLocationCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: LocationMessageSource,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> LocationCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: LocationCollectionCell.reuseIdentifier, for: indexPath) as! LocationCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = LocationController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true))
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createLocationQuoteCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: LocationMessageSource,
                                 quoteUserName: String,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> LocationQuoteCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: LocationQuoteCollectionCell.reuseIdentifier, for: indexPath) as! LocationQuoteCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = LocationController(source: source,
                                         messageId: messageId,
                                         bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true), senderNickname: quoteUserName, isQuoted: true)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.quoteView = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createFileCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> FileCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: FileCollectionCell.reuseIdentifier, for: indexPath) as! FileCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = FileController(source: source,
                                        messageId: messageId,
                                        status: status,
                                        messageType: messageType,
                                        bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true))
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createFileQuoteCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MediaMessageSource,
                                 quoteUserName: String,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> FileQuoteCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: FileQuoteCollectionCell.reuseIdentifier, for: indexPath) as! FileQuoteCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = FileController(source: source,
                                        messageId: messageId,
                                        status: status,
                                        bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true), senderNickname: quoteUserName, isQuoted: true)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.quoteView = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createMergerCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MergerMessageSource,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> MergerCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: MergerCollectionCell.reuseIdentifier, for: indexPath) as! MergerCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = MergerController(source: source,
                                        messageId: messageId,
                                        bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true))
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createMergerQuoteCell(collectionView: UICollectionView,
                                 messageId: String,
                                 isSelected: Bool,
                                 isPrivateChat: Bool,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 user: User,
                                 source: MergerMessageSource,
                                 quoteUserName: String,
                                 forVideo: Bool = false,
                                 date: Date,
                                 bubbleType: Cell.BubbleType,
                                 status: MessageStatus,
                                 messageType: MessageType) -> MergerQuoteCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: MergerQuoteCollectionCell.reuseIdentifier, for: indexPath) as! MergerQuoteCollectionCell
        
        setupMessageContainerView(cell.customView, messageId: messageId, isSelected: isSelected, alignment: alignment, isQuoted: true)
        setupMainMessageView(cell.customView.customView, user: user, messageID: messageId, alignment: alignment, bubble: bubbleType, status: status, date: date, isPrivateChat: isPrivateChat, isQuoted: true)
        
        setupSwipeHandlingAccessory(cell.customView.customView, date: date, accessoryConnectingView: cell.customView)
        
        let bubbleView = cell.customView.customView.customView
        let controller = MergerController(source: source,
                                        messageId: messageId,
                                        bubbleController: buildTextBubbleController(bubbleView: bubbleView, messageType: messageType, bubbleType: bubbleType, isQuoted: true), senderNickname: quoteUserName, isQuoted: true)
        
        controller.delegate = reloadDelegate
        bubbleView.customView.setup(with: controller)
        controller.quoteView = bubbleView.customView
        cell.delegate = bubbleView.customView

        return cell
    }
    
    private func createTypingIndicatorCell(collectionView: UICollectionView, indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: TypingIndicatorCollectionCell.reuseIdentifier, for: indexPath) as! TypingIndicatorCollectionCell
        let alignment = ChatItemAlignment.leading
        cell.customView.alignment = alignment
        let bubbleView = cell.customView.customView.customView
        
        let controller = TypingIndicatorController(bubbleController: buildBlankBubbleController(bubbleView: bubbleView,
                                                                                                messageType: .incoming,
                                                                                                bubbleType: .normal))
        bubbleView.customView.setup(with: controller)
        controller.view = bubbleView.customView
        cell.customView.accessoryView?.isHiddenSafe = true
        
        return cell
    }
    // 名字
    private func createGroupTitle(collectionView: UICollectionView, indexPath: IndexPath, alignment: ChatItemAlignment, title: String) -> UserTitleCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: UserTitleCollectionCell.reuseIdentifier, for: indexPath) as! UserTitleCollectionCell
        cell.customView.spacing = 2
        
        cell.customView.customView.customView.text = title
        cell.customView.customView.customView.preferredMaxLayoutWidth = (collectionView.collectionViewLayout as? CollectionViewChatLayout)?.layoutFrame.width ?? collectionView.frame.width
        cell.customView.customView.customView.font = .preferredFont(forTextStyle: .caption2)
        cell.customView.customView.flexibleEdges = [.top]
        cell.customView.accessoryView.isHidden = true
        cell.contentView.layoutMargins = .zero
        
        return cell
    }
    
    private func createSeparator(collectionView: UICollectionView, indexPath: IndexPath) -> SeparatorCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: SeparatorCollectionCell.reuseIdentifier, for: indexPath) as! SeparatorCollectionCell
//        cell.customView.spacing = 2
//        
//        cell.customView.customView.customView.text = title
//        cell.customView.customView.customView.preferredMaxLayoutWidth = (collectionView.collectionViewLayout as? CollectionViewChatLayout)?.layoutFrame.width ?? collectionView.frame.width
//        cell.customView.customView.customView.font = .preferredFont(forTextStyle: .caption2)
//        cell.customView.customView.flexibleEdges = [.top]
//        cell.customView.accessoryView.isHidden = true
        cell.contentView.layoutMargins = .zero
        
        return cell
    }
    
    // 设置时间组cell/系统提示cell
    private func createTipsTitle(collectionView: UICollectionView,
                                 indexPath: IndexPath,
                                 alignment: ChatItemAlignment,
                                 messageId: String,
                                 title: String? = nil,
                                 attributeTitle: NSAttributedString? = nil) -> TitleCollectionCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: TitleCollectionCell.reuseIdentifier, for: indexPath) as! TitleCollectionCell
        
        let controller = TipsTitleController(messageId: messageId)
        controller.delegate = reloadDelegate
        
        cell.customView.setup(with: controller)
        cell.customView.preferredMaxLayoutWidth = (collectionView.collectionViewLayout as? CollectionViewChatLayout)?.layoutFrame.width ?? collectionView.frame.width
        if title != nil {
            cell.customView.text = title
            cell.customView.textColor = .gray
        } else {
            cell.customView.attributedText = attributeTitle
        }
        
        cell.contentView.layoutMargins = UIEdgeInsets(top: 2, left: 0, bottom: 2, right: 0)
        return cell
    }
    // 设置编辑状态
    private func setupMessageContainerView(_ messageContainerView: MessageContainerView<EditingAccessoryView, some Any>, messageId: String, isSelected: Bool, alignment: ChatItemAlignment, isQuoted: Bool = false) {
        messageContainerView.alignment = .fullWidth
        if let accessoryView = messageContainerView.accessoryView {
            accessoryView.customView.isHiddenSafe = true
            accessoryView.customView.alpha = 0
            
            accessoryView.setIsEditing(false)
        }
    }

    // 设置头像
    private func setupMainMessageView(_ cellView: MainContainerView<ChatAvatarView, some Any, ChatAvatarView>,
                                      user: User,
                                      messageID: String,
                                      alignment: ChatItemAlignment,
                                      bubble: Cell.BubbleType,
                                      status: MessageStatus,
                                      date: Date,
                                      isPrivateChat: Bool = false,
                                      isReplyMessage: Bool = false,
                                      isQuoted: Bool = false,
                                      isAudioMsg: Bool = false) {
        cellView.containerView.alignment = .top
        cellView.containerView.leadingView?.isHiddenSafe = false
        cellView.containerView.leadingView?.alpha = 1
        cellView.containerView.trailingView?.isHiddenSafe = true
        cellView.containerView.trailingView?.alpha = 0
        cellView.leadingCountdownLabel.isHiddenSafe = alignment.isIncoming
        cellView.trailingCountdownLabel.isHiddenSafe = !alignment.isIncoming
        cellView.trailingAudioRedLabel.isHiddenSafe = true// !alignment.isIncoming || (!isAudioMsg || status == .read)
        cellView.trailingDateLabel.isHiddenSafe = false
        cellView.trailingDateLabel.text = ChatHistoryDateFormatter.shared.string(from: date)
        cellView.containerView.customView.statusLabel.isHiddenSafe = true
        cellView.containerView.customView.statusLabel.alpha = 0
        cellView.containerView.customView.setup(with: status)
        let nicknameLabel = cellView.containerView.customView.nicknameLabel
        nicknameLabel.isHiddenSafe = isQuoted
        nicknameLabel.text = nicknameLabel.isHiddenSafe ? nil : user.name
        
        if let avatarView = cellView.containerView.leadingView {
            avatarView.customView.isHiddenSafe = isQuoted || bubble == .tailed
            
            if !isQuoted && bubble == .normal {
                let avatarViewController = AvatarViewController(user: user, bubble: bubble)
                avatarViewController.delegate = reloadDelegate
                avatarView.setup(with: avatarViewController)
                avatarViewController.view = avatarView
            }
        }
        
//        if let avatarView = cellView.containerView.trailingView {
//            avatarView.customView.isHiddenSafe = isQuoted
//            
//            if !isQuoted {
//                let avatarViewController = AvatarViewController(user: user, bubble: bubble)
//                avatarView.setup(with: avatarViewController)
//                avatarViewController.view = avatarView
//            }
//        }
    }

    // 设置右滑显示时间
    private func setupSwipeHandlingAccessory(_ cellView: MainContainerView<ChatAvatarView, some Any, ChatAvatarView>,
                                             date: Date,
                                             accessoryConnectingView: UIView) {
       
    }
    
    private func buildBlankBubbleController(bubbleView: BezierMaskedView<some Any>,
                                           messageType: MessageType,
                                           bubbleType: Cell.BubbleType) -> BubbleController {
        let textBubbleController = BlankBubbleController(bubbleView: bubbleView, type: messageType, bubbleType: bubbleType)
        let bubbleController = BezierBubbleController(bubbleView: bubbleView, controllerProxy: textBubbleController, type: messageType, bubbleType: bubbleType)
        return bubbleController
    }
    
    private func buildTextBubbleController(bubbleView: BezierMaskedView<some Any>,
                                           messageType: MessageType,
                                           bubbleType: Cell.BubbleType,
                                           isQuoted: Bool = false) -> BubbleController {
        let textBubbleController = TextBubbleController(bubbleView: bubbleView, type: messageType, bubbleType: bubbleType, isQuoted: isQuoted)
        let bubbleController = BezierBubbleController(bubbleView: bubbleView, controllerProxy: textBubbleController, type: messageType, bubbleType: bubbleType)
        return bubbleController
    }
    
    private func buildBezierBubbleController(for bubbleView: BezierMaskedView<some Any>,
                                             messageType: MessageType,
                                             bubbleType: Cell.BubbleType) -> BubbleController {
        let contentBubbleController = FullCellContentBubbleController(bubbleView: bubbleView)
        let bubbleController = BezierBubbleController(bubbleView: bubbleView, controllerProxy: contentBubbleController, type: messageType, bubbleType: bubbleType)
        return bubbleController
    }
}

extension ChatHistoryCollectionDataSource: UICollectionViewDataSource {
    
    public func numberOfSections(in collectionView: UICollectionView) -> Int {
        sections.count
    }
    
    public func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        sections[section].cells.count
    }
    
    public func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = sections[indexPath.section].cells[indexPath.item]
        switch cell {
            
        case let .date(group):
            let cell = createTipsTitle(collectionView: collectionView, indexPath: indexPath, alignment: cell.alignment, messageId: group.id, title: group.value)
            
            return cell
        case let .systemMessage(group):
            let cell = createTipsTitle(collectionView: collectionView, indexPath: indexPath, alignment: cell.alignment, messageId: group.id, attributeTitle: group.value)
            
            return cell
        case let .messageGroup(group):
            let cell = createGroupTitle(collectionView: collectionView, indexPath: indexPath, alignment: cell.alignment, title: group.title)
            
            return cell
        case let .message(message, bubbleType: bubbleType):
            let privateChat = message.sessionType == .c1v1
            
            switch message.data {
            case let .text(source):
                let cell = createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, text: source.text, anchor: message.isAnchor, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .attributeText(text):
                let cell = createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, attributedString: text, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
            case let .custom(source):
                let cell = createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, attributedString: source.attributedString, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .url(url, isLocallyStored: _):
                if #available(iOS 13.0, *) {
                    return createURLCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, url: url, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                } else {
                    return createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, text: url.absoluteString, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                }
            case let .image(source, isLocallyStored: _):
                let cell = createImageCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
            case let .video(source, isLocallyStored: _):
                let cell = createVideoCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .audio(source, isLocallyStored: _):
                let cell = createAudioCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .card(source):
                let cell = createCardCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .location(source):
                let cell = createLocationCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .file(source, isLocallyStored: _):
                let cell = createFileCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .merger(source):
                let cell = createMergerCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            case let .quote(source):
                switch source.quoteData {
                case let .text(textSource):
                    let cell = createTextQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, text: textSource.text, quoteUserName: source.quoteUser, anchor: message.isAnchor, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                default:
                    fatalError()
                }
            }
        case let .replyMessage(message, bubbleType: bubbleType):
            let privateChat = message.sessionType == .c1v1
            
            switch message.data {
            case let .quote(source):
                let cell = createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, isReplyMessage: true, indexPath: indexPath, text: source.text, anchor: message.isAnchor, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                
                return cell
                
            default:
                fatalError()
            }
            
        case let .quoteMessage(message, bubbleType: bubbleType):
            let privateChat = message.sessionType == .c1v1
            switch message.data {
            case let .quote(quoteSource):
                switch quoteSource.quoteData {
                case let .text(source):
                    let cell = createTextQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, text: source.text, quoteUserName: quoteSource.quoteUser, anchor: message.isAnchor, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                case let .attributeText(text):
                    let cell = createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: false, indexPath: indexPath, attributedString: text, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                case let .custom(source):
                    let cell = createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: false, indexPath: indexPath, attributedString: source.attributedString, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                case let .url(url, isLocallyStored: _):
                    if #available(iOS 13.0, *) {
                        return createURLCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: false, indexPath: indexPath, url: url, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    } else {
                        return createTextCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: false, indexPath: indexPath, text: url.absoluteString, date: message.date, alignment: cell.alignment, user: message.owner, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    }
                case let .image(source, isLocallyStored: _):
                    let cell = createImageQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, quoteUserName: quoteSource.quoteUser, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                case let .video(source, isLocallyStored: _):
                    let cell = createVideoQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, quoteUserName: quoteSource.quoteUser, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                case let .audio(source, isLocallyStored: _):
                    let cell = createAudioQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, quoteUserName: quoteSource.quoteUser, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                case let .card(source):
                    let cell = createCardQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, quoteUserName: quoteSource.quoteUser, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                case let .location(source):
                    let cell = createLocationQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, quoteUserName: quoteSource.quoteUser, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                case let .file(source, isLocallyStored: _):
                    let cell = createFileQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, quoteUserName: quoteSource.quoteUser, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                case let .merger(source):
                    let cell = createMergerQuoteCell(collectionView: collectionView, messageId: message.id, isSelected: message.isSelected, isPrivateChat: privateChat, indexPath: indexPath, alignment: cell.alignment, user: message.owner, source: source, quoteUserName: quoteSource.quoteUser, date: message.date, bubbleType: bubbleType, status: message.status, messageType: message.type)
                    
                    return cell
                    
                default:
                    fatalError()
                }
            default:
                fatalError()
            }
        case .typingIndicator:
            return createTypingIndicatorCell(collectionView: collectionView, indexPath: indexPath)
        case .separator:
            return createSeparator(collectionView: collectionView, indexPath: indexPath)
        default:
            fatalError()
        }
    }
    
    public func collectionView(_ collectionView: UICollectionView, viewForSupplementaryElementOfKind kind: String, at indexPath: IndexPath) -> UICollectionReusableView {
        switch kind {
        case UICollectionView.elementKindSectionHeader:
            let view = collectionView.dequeueReusableSupplementaryView(ofKind: kind,
                                                                       withReuseIdentifier: TextTitleView.reuseIdentifier,
                                                                       for: indexPath) as! TextTitleView
            view.customView.text = sections[indexPath.section].title
            view.customView.preferredMaxLayoutWidth = 300
            view.customView.textColor = .lightGray
            view.customView.numberOfLines = 0
            view.customView.font = .preferredFont(forTextStyle: .caption2)
            return view
        case UICollectionView.elementKindSectionFooter:
            let view = collectionView.dequeueReusableSupplementaryView(ofKind: kind,
                                                                       withReuseIdentifier: TextTitleView.reuseIdentifier,
                                                                       for: indexPath) as! TextTitleView
            view.customView.text = nil
            return view
        default:
            fatalError()
        }
    }
}

extension ChatHistoryCollectionDataSource: ChatLayoutDelegate {
    
    public func shouldPresentHeader(_ chatLayout: CollectionViewChatLayout, at sectionIndex: Int) -> Bool {
        true
    }
    
    public func shouldPresentFooter(_ chatLayout: CollectionViewChatLayout, at sectionIndex: Int) -> Bool {
        true
    }
    
    public func sizeForItem(_ chatLayout: CollectionViewChatLayout, of kind: ItemKind, at indexPath: IndexPath) -> ItemSize {
        switch kind {
        case .cell:
            let item = sections[indexPath.section].cells[indexPath.item]
            switch item {
            case let .message(message, bubbleType: _):
                switch message.data {
                case .text, .attributeText, .custom(_):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 36))
                case let .image(_, isLocallyStored: isDownloaded):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: isDownloaded ? 180 : 80))
                case let .url(_, isLocallyStored: isDownloaded):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: isDownloaded ? 60 : 36))
                case let .video(_, isLocallyStored: isDownloaded):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: isDownloaded ? 120 : 80))
                case let .audio(_, isLocallyStored: isDownloaded):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                case let .card(_):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                case let .location(_):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                case let .file(_, _):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                case let .merger(_):
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                default:
                    return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 36))
                }
            case let .replyMessage(message, bubbleType: _):
                return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 36))
            case .date, .systemMessage:
                return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 18))
            case .typingIndicator:
                return .estimated(CGSize(width: 60, height: 36))
            case .separator:
                return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 12))
            case .messageGroup:
                return .estimated(CGSize(width: min(85, chatLayout.layoutFrame.width / 3), height: 18))
            case let .quoteMessage(message, bubbleType: _):
                switch message.data {
                case let .quote(quoteSource):
                    switch quoteSource.quoteData {
                    case .text, .attributeText, .custom(_):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 36))
                    case let .image(_, isLocallyStored: isDownloaded):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: isDownloaded ? 180 : 80))
                    case let .url(_, isLocallyStored: isDownloaded):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: isDownloaded ? 60 : 36))
                    case let .video(_, isLocallyStored: isDownloaded):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: isDownloaded ? 120 : 80))
                    case let .audio(_, isLocallyStored: isDownloaded):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                    case let .file(_, isLocallyStored: isDownloaded):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                    case let .card(_):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                    case let .location(_):
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 120))
                    default:
                        return .estimated(CGSize(width: chatLayout.layoutFrame.width, height: 36))
                    }
                default:
                    fatalError()
                }
            }
        case .footer, .header:
            return .auto
        }
    }
    
    public func alignmentForItem(_ chatLayout: CollectionViewChatLayout, of kind: ItemKind, at indexPath: IndexPath) -> ChatItemAlignment {
        switch kind {
        case .header:
            return .center
        case .cell:
            let item = sections[indexPath.section].cells[indexPath.item]
            switch item {
            case .date, .systemMessage:
                return .center
            case .message, .replyMessage, .quoteMessage, .separator:
                return .fullWidth
            case .messageGroup, .typingIndicator:
                return .leading
            }
        case .footer:
            return .trailing
        }
    }
    
    public func initialLayoutAttributesForInsertedItem(_ chatLayout: CollectionViewChatLayout, of kind: ItemKind, at indexPath: IndexPath, modifying originalAttributes: ChatLayoutAttributes, on state: InitialAttributesRequestType) {
        originalAttributes.alpha = 0
        guard state == .invalidation,
              kind == .cell else {
            return
        }
        switch sections[indexPath.section].cells[indexPath.item] {
            // Uncomment to see the effect
            //        case .messageGroup:
            //            originalAttributes.center.x -= originalAttributes.frame.width
            //        case let .message(message, bubbleType: _):
            //            originalAttributes.transform = .init(scaleX: 0.9, y: 0.9)
            //            originalAttributes.transform = originalAttributes.transform.concatenating(.init(rotationAngle: message.type == .incoming ? -0.05 : 0.05))
            //            originalAttributes.center.x += (message.type == .incoming ? -20 : 20)
        case .typingIndicator:
            originalAttributes.transform = .init(scaleX: 0.1, y: 0.1)
            originalAttributes.center.x -= originalAttributes.bounds.width / 5
        default:
            break
        }
    }
    
    public func finalLayoutAttributesForDeletedItem(_ chatLayout: CollectionViewChatLayout, of kind: ItemKind, at indexPath: IndexPath, modifying originalAttributes: ChatLayoutAttributes) {
        originalAttributes.alpha = 0
        guard kind == .cell else {
            return
        }
        switch oldSections[indexPath.section].cells[indexPath.item] {
            // Uncomment to see the effect
//                    case .messageGroup:
//                        originalAttributes.center.x -= originalAttributes.frame.width
//                    case let .message(message, bubbleType: _):
//                        originalAttributes.transform = .init(scaleX: 0.9, y: 0.9)
//                        originalAttributes.transform = originalAttributes.transform.concatenating(.init(rotationAngle: message.type == .incoming ? -0.05 : 0.05))
//                        originalAttributes.center.x += (message.type == .incoming ? -20 : 20)
        case .typingIndicator:
            originalAttributes.transform = .init(scaleX: 0.1, y: 0.1)
            originalAttributes.center.x -= originalAttributes.bounds.width / 5
        default:
            break
        }
    }
    
    public func interItemSpacing(_ chatLayout: CollectionViewChatLayout, of kind: ItemKind, after indexPath: IndexPath) -> CGFloat? {
        guard sections.count > 0, sections[indexPath.section].cells.count > 0 else { return nil }
        let item = sections[indexPath.section].cells[indexPath.item]
        switch item {
        case .messageGroup:
            return 2
        case .replyMessage:
            return 2
        default:
            return nil
        }
    }

//    public func interSectionSpacing(_ chatLayout: CollectionViewChatLayout, after sectionIndex: Int) -> CGFloat? {
//        100
//    }

}
