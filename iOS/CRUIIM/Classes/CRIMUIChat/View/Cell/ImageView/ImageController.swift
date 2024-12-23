import Foundation
import UIKit

final class ImageController {

    weak var view: ImageView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: ImageQuoteView? {
        didSet {
            quoteView?.reloadData()
        }
    }

    weak var delegate: ReloadDelegate?
    
    var state: ImageViewState {
        guard let image else {
            return .loading
        }
        return .image(image)
    }
    
    var isQuoted: Bool = false
    
    let senderNickname: String?

    private var image: UIImage?

    private let messageId: String

    private let source: MediaMessageSource

    private let bubbleController: BubbleController

    init(source: MediaMessageSource, messageId: String, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false) {
        self.source = source
        self.messageId = messageId
        self.bubbleController = bubbleController
        self.senderNickname = (senderNickname ?? "") + ":"
        self.isQuoted = isQuoted
        loadImage()
    }
    
    private func loadImage() {
        if let image = source.image {
            self.image = image
            view?.reloadData()
            quoteView?.reloadData()
        } else {
            guard let url = source.source.url else { return }
            if let image = try? imageCache.getEntity(for: CacheableImageKey(url: url)) {
                self.image = image
                view?.reloadData()
                quoteView?.reloadData()
            } else {
                loader.loadImage(from: url) { [weak self] _ in
                    guard let self else {
                        return
                    }
                    
                    self.delegate?.reloadMessage(with: self.messageId)
                }
            }
        }
    }

    func action() {
        delegate?.didTapContent(with: messageId, data: .image(source, isLocallyStored: true))
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview else { return }
        delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .image(source, isLocallyStored: false))
    }
}
