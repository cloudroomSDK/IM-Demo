import ChatLayout
import Foundation
import UIKit
import CRUICore

protocol BubbleController {}

final class TextBubbleController<CustomView: UIView>: BubbleController {

    private let type: MessageType

    private let bubbleType: Cell.BubbleType
    
    let isQuoted: Bool

    public weak var bubbleView: UIView? {
        didSet {
            setupBubbleView()
        }
    }

    init(bubbleView: UIView, type: MessageType, bubbleType: Cell.BubbleType, isQuoted: Bool = false) {
        self.type = type
        self.bubbleType = bubbleType
        self.bubbleView = bubbleView
        self.isQuoted = isQuoted
        setupBubbleView()
    }

    private func setupBubbleView() {
        guard let bubbleView else {
            return
        }
        UIView.performWithoutAnimation {
            let marginOffset: CGFloat = type.isIncoming ? -StandardUI.tailSize : StandardUI.tailSize
            let edgeInsets = UIEdgeInsets(top: 8, left: 12 - marginOffset, bottom: 8, right: 12 + marginOffset)
            bubbleView.layoutMargins = edgeInsets

            if #available(iOS 13.0, *) {
                let incomingGray = UIColor(red: 0 / 255, green: 0 / 255, blue: 0 / 255, alpha: 0.06)
                bubbleView.backgroundColor = (type.isIncoming || isQuoted) ? incomingGray : .c0584FE
            } else {
                let systemGray5 = UIColor(red: 200 / 255, green: 200 / 255, blue: 200 / 255, alpha: 1)
                bubbleView.backgroundColor = (type.isIncoming || isQuoted) ? systemGray5 : .systemBlue
            }
        }
    }
}

final class FullCellContentBubbleController<CustomView: UIView>: BubbleController {

    weak var bubbleView: BezierMaskedView<CustomView>? {
        didSet {
            setupBubbleView()
        }
    }

    init(bubbleView: BezierMaskedView<CustomView>) {
        self.bubbleView = bubbleView
        setupBubbleView()
    }

    private func setupBubbleView() {
        guard let bubbleView else {
            return
        }

        UIView.performWithoutAnimation {
            bubbleView.backgroundColor = .clear
            bubbleView.customView.layoutMargins = .zero
        }
    }
}

final class BlankBubbleController<CustomView: UIView>: BubbleController {

    private let type: MessageType

    private let bubbleType: Cell.BubbleType

    weak var bubbleView: UIView? {
        didSet {
            setupBubbleView()
        }
    }

    init(bubbleView: UIView, type: MessageType, bubbleType: Cell.BubbleType) {
        self.type = type
        self.bubbleType = bubbleType
        self.bubbleView = bubbleView
        setupBubbleView()
    }

    private func setupBubbleView() {
        guard let bubbleView else {
            return
        }
        UIView.performWithoutAnimation {
            let edgeInsets = type.isIncoming ? UIEdgeInsets(top: 0, left: 2 + StandardUI.tailSize, bottom: 0, right:  2) : UIEdgeInsets(top: 0, left: 2, bottom: 0, right: 2 + StandardUI.tailSize)
            bubbleView.layoutMargins = edgeInsets
            
            bubbleView.backgroundColor = .clear
        }
    }
}

public class BezierBubbleController<CustomView: UIView>: BubbleController {

    private let controllerProxy: BubbleController

    private let type: MessageType

    private let bubbleType: Cell.BubbleType

    weak var bubbleView: BezierMaskedView<CustomView>? {
        didSet {
            setupBubbleView()
        }
    }

    init(bubbleView: BezierMaskedView<CustomView>, controllerProxy: BubbleController, type: MessageType, bubbleType: Cell.BubbleType) {
        self.controllerProxy = controllerProxy
        self.type = type
        self.bubbleType = bubbleType
        self.bubbleView = bubbleView
        setupBubbleView()
    }

    private func setupBubbleView() {
        guard let bubbleView else {
            return
        }

        bubbleView.messageType = type
        bubbleView.bubbleType = bubbleType
    }
}
