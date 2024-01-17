//
//  TipsTitleController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/1/8.
//

import Foundation

final class TipsTitleController {
    
    weak var delegate: ReloadDelegate?
    
    private let messageId: String
    
    init(messageId: String) {
        self.messageId = messageId
    }
    
    func action() {
        delegate?.reeditMessage(with: messageId)
    }
}
