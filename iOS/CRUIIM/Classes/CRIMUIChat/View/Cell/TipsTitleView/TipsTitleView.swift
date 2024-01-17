//
//  TipsTitleView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/1/8.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore
import RxSwift

class TipsTitleView: UILabel {
    
    private var controller: TipsTitleController?

    override init(frame: CGRect) {
        super.init(frame: frame)
        setupSubviews()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupSubviews()
    }
    
    func setup(with controller: TipsTitleController) {
        self.controller = controller
    }
    
    private func setupSubviews() {
        isUserInteractionEnabled = true
        numberOfLines = 0
        font = .preferredFont(forTextStyle: .caption2)
        
        let tapGesture = UITapGestureRecognizer(target: self, action: #selector(handleTap(_:)))
        addGestureRecognizer(tapGesture)
    }
    
    @objc func handleTap(_ gestureRecognizer: UITapGestureRecognizer) {
        guard let label = gestureRecognizer.view as? UILabel,
              let text = label.text else { return }
        
        let range = (text as NSString).range(of: "重新编辑".innerLocalized())
        if gestureRecognizer.didTapAttributedTextInLabel(label: label, inRange: range) {
            // 在这里执行“重新编辑”文本的响应操作
            print("重新编辑被点击")
            controller?.action()
        }
    }

}

extension UITapGestureRecognizer {
    func didTapAttributedTextInLabel(label: UILabel, inRange targetRange: NSRange) -> Bool {
        guard let attributedText = label.attributedText else { return false }
        
        let layoutManager = NSLayoutManager()
        let textContainer = NSTextContainer(size: .zero)
        let textStorage = NSTextStorage(attributedString: attributedText)
        
        layoutManager.addTextContainer(textContainer)
        textStorage.addLayoutManager(layoutManager)
        
        textContainer.lineFragmentPadding = 0.0
        textContainer.lineBreakMode = label.lineBreakMode
        textContainer.maximumNumberOfLines = label.numberOfLines
        textContainer.size = label.bounds.size
        
        let locationOfTouchInLabel = self.location(in: label)
        let textBoundingBox = layoutManager.usedRect(for: textContainer)
        let textContainerOffset = CGPoint(x: (label.bounds.width - textBoundingBox.width) * 0.5 - textBoundingBox.minX,
                                          y: (label.bounds.height - textBoundingBox.height) * 0.5 - textBoundingBox.minY)
        
        let locationOfTouchInTextContainer = CGPoint(x: locationOfTouchInLabel.x - textContainerOffset.x,
                                                     y: locationOfTouchInLabel.y - textContainerOffset.y)
        let indexOfCharacter = layoutManager.characterIndex(for: locationOfTouchInTextContainer,
                                                            in: textContainer,
                                                            fractionOfDistanceBetweenInsertionPoints: nil)
        
        return NSLocationInRange(indexOfCharacter, targetRange)
    }
}
