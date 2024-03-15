//
//  CardController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/1/31.
//

import Foundation
import UIKit

final class CardController {

    weak var view: CardView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: CardQuoteView? {
        didSet {
            quoteView?.reloadData()
        }
    }

    weak var delegate: ReloadDelegate?
    
    var isQuoted: Bool = false
    
    let senderNickname: String?
    
    let cardNickname: String?
    var faceURL: String?

    private var image: UIImage?

    private let messageId: String

    private let source: CardMessageSource

    private let bubbleController: BubbleController

    init(source: CardMessageSource, messageId: String, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false) {
        self.cardNickname = source.user.name
        self.faceURL = source.user.faceURL
        self.source = source
        self.messageId = messageId
        self.bubbleController = bubbleController
        self.senderNickname = (senderNickname ?? "") + ":"
        self.isQuoted = isQuoted
    }

    func action() {
        delegate?.didTapContent(with: messageId, data: .card(source))
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview else { return }
        delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .card(source))
    }
}
