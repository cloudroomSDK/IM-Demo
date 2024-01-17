import Foundation
import UIKit

final class VideoController {

    weak var view: VideoView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: VideoQuoteView? {
        didSet {
            quoteView?.reloadData()
        }
    }

    weak var delegate: ReloadDelegate?
    
    var state: VideoViewState {
        guard let image else {
            return .loading
        }
        return .image(image)
    }
    
    var isQuoted: Bool = false
    
    let senderNickname: String?

    private var image: UIImage?

    private let messageId: String
    
    var duration: String?

    private let source: MediaMessageSource

    private let bubbleController: BubbleController

    init(source: MediaMessageSource, messageId: String, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false) {
        self.source = source
        self.messageId = messageId
        self.bubbleController = bubbleController
        self.duration = #"\#(source.duration!)s"#
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
            guard let url = source.thumb?.url else { return }
            if let image = try? imageCache.getEntity(for: .init(url: url)) {
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
        delegate?.didTapContent(with: messageId, data: .video(source, isLocallyStored: true))
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview else { return }
        delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .video(source, isLocallyStored: false))
    }
}
