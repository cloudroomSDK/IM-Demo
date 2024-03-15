//
//  LocationController.swift
//  CRUICoreView
//
//  Created by YunWu01 on 2024/2/2.
//

import Foundation
import UIKit

final class LocationController {
    weak var view: LocationView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: LocationQuoteView? {
        didSet {
            quoteView?.reloadData()
        }
    }

    weak var delegate: ReloadDelegate?
    
    var isQuoted: Bool = false
    
    let senderNickname: String?
    
    var longitude: Double
    var latitude: Double
    let name: String?
    let addr: String?

    private var image: UIImage?

    private let messageId: String

    private let source: LocationMessageSource

    private let bubbleController: BubbleController

    init(source: LocationMessageSource, messageId: String, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false) {
        self.longitude = source.longitude
        self.latitude = source.latitude
        self.name = source.desc?.name
        self.addr = source.desc?.addr
        self.source = source
        self.messageId = messageId
        self.bubbleController = bubbleController
        self.senderNickname = (senderNickname ?? "") + ":"
        self.isQuoted = isQuoted
    }

    func action() {
        delegate?.didTapContent(with: messageId, data: .location(source))
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview else { return }
        delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .location(source))
    }
}
