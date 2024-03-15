//
//  PaddedUILabel.swift
//  CRUICore
//
//  Created by YunWu01 on 2024/3/5.
//

import UIKit

public class PaddedUILabel: UILabel {
    // 定义文本内边距
    public var textInsets = UIEdgeInsets.zero {
        didSet {
            // 文本内边距发生变化时重新绘制文本
            invalidateIntrinsicContentSize()
        }
    }

    public override func textRect(forBounds bounds: CGRect, limitedToNumberOfLines numberOfLines: Int) -> CGRect {
        // 考虑文本内边距计算文本绘制区域
        let insetRect = bounds.inset(by: textInsets)
        let textRect = super.textRect(forBounds: insetRect, limitedToNumberOfLines: numberOfLines)
        let invertedInsets = UIEdgeInsets(top: -textInsets.top,
                                          left: -textInsets.left,
                                          bottom: -textInsets.bottom,
                                          right: -textInsets.right)
        return textRect.inset(by: invertedInsets)
    }

    public override func drawText(in rect: CGRect) {
        // 考虑文本内边距绘制文本
        super.drawText(in: rect.inset(by: textInsets))
    }
}
