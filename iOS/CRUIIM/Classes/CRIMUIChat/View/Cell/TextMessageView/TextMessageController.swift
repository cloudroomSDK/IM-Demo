
import Foundation

final class TextMessageController {

    weak var view: TextMessageView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: TextMessageView? {
        didSet {
            quoteView?.reloadData()
        }
    }
    
    weak var delegate: ReloadDelegate?

    let text: String?
    
    let attributedString: NSAttributedString?

    let type: MessageType
    
    var highlight: Bool = false
    var isQuoted: Bool = false

    private let messageId: String
    private let bubbleController: BubbleController

    init(text: String? = nil, messageId: String, attributedString: NSAttributedString? = nil, highlight: Bool = false, type: MessageType, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false) {
        if let senderNickname {
            self.text = senderNickname + ":" + " " + (text ?? "")
        } else {
            self.text = text
        }
        self.messageId = messageId
        self.attributedString = attributedString
        self.highlight = highlight
        self.type = type
        self.bubbleController = bubbleController
        self.isQuoted = isQuoted
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview, !isQuoted else { return }
        if let text = self.text {
            delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .text(TextMessageSource(text: text)))
        }
        
        if let attributedString = self.attributedString {
            delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .attributeText(attributedString))
        }
    }
}
