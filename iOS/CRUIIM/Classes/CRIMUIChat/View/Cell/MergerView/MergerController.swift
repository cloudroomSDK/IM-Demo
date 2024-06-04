//
//  MergerController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/19.
//

import Foundation
import UIKit
import CRUICore

final class MergerController {

    weak var view: MergerView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: MergerQuoteView? {
        didSet {
            quoteView?.reloadData()
        }
    }

    weak var delegate: ReloadDelegate?
    
    var isQuoted: Bool = false
    
    let senderNickname: String
    
    let title: String
    
    let abstract: String
    
    private let messageId: String

    private let source: MergerMessageSource

    private let bubbleController: BubbleController
    
    init(source: MergerMessageSource, messageId: String, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false) {
        self.source = source
        self.messageId = messageId
        self.bubbleController = bubbleController
        self.senderNickname = (senderNickname ?? "") + ":" + " " + "[聊天记录]".innerLocalized() + (source.title ?? "")
        self.isQuoted = isQuoted
        self.title = source.title ?? ""
        var combinedString = ""
        if let abstract = source.abstract {
            let maxCount = min(2, abstract.count ?? 0)
            combinedString = abstract.prefix(maxCount).joined(separator: "\n")
        }
        self.abstract = combinedString
    }

    func action() {
        delegate?.didTapContent(with: messageId, data: .merger(source))
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview else { return }
        delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .merger(source))
    }
}

