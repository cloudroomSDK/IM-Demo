
import ChatLayout
import Foundation
import UIKit
import CRUICore
import RxSwift

final class TextMessageView: UIView, ContainerCollectionViewCellDelegate {

    private var disposeBag = DisposeBag()
    
    private var viewPortWidth: CGFloat = 300

    private lazy var textView = MessageTextView()

    private var controller: TextMessageController?

    private var textViewWidthConstraint: NSLayoutConstraint?

    override init(frame: CGRect) {
        super.init(frame: frame)
        setupSubviews()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupSubviews()
    }

    func prepareForReuse() {
        textView.textContainer.maximumNumberOfLines = 0
        textView.textContainer.lineBreakMode = .byWordWrapping
        textView.resignFirstResponder()
    }

    // Uncomment this method to test the performance without calculating text cell size using autolayout
    // For the better illustration set DefaultRandomDataProvider.enableRichContent/enableNewMessages/enableRichContent
    // to false
//    func preferredLayoutAttributesFitting(_ layoutAttributes: ChatLayoutAttributes) -> ChatLayoutAttributes? {
//        viewPortWidth = layoutAttributes.layoutFrame.width
//        guard let text = controller?.text as NSString? else {
//            return layoutAttributes
//        }
//        let maxWidth = viewPortWidth * Constants.maxWidth
//        var rect = text.boundingRect(with: CGSize(width: maxWidth, height: CGFloat.greatestFiniteMagnitude),
//            options: [.usesLineFragmentOrigin, .usesFontLeading],
//            attributes: [NSAttributedString.Key.font: textView.font as Any], context: nil)
//        rect = rect.insetBy(dx: 0, dy: -8)
//        layoutAttributes.size = CGSize(width: layoutAttributes.layoutFrame.width, height: rect.height)
//        setupSize()
//        return layoutAttributes
//    }

    func apply(_ layoutAttributes: ChatLayoutAttributes) {
        viewPortWidth = layoutAttributes.layoutFrame.width
        setupSize()
    }

    public override func layoutSubviews() {
        super.layoutSubviews()
    }

    func setup(with controller: TextMessageController) {
        self.controller = controller
        reloadData()
    }

    func reloadData() {
        guard let controller else {
            return
        }
        let font = controller.isQuoted ? UIFont.preferredFont(forTextStyle: .footnote) : UIFont.preferredFont(forTextStyle: .body)
        if controller.text != nil {
            textView.font = font
            textView.text = controller.text
        } else {
            var str = NSAttributedString(string: controller.attributedString!.string, attributes: [.font: font])

            textView.attributedText = str
        }
        if controller.isQuoted {
            textView.textContainer.maximumNumberOfLines = 2
            textView.textContainer.lineBreakMode = .byTruncatingTail
        }
        UIView.performWithoutAnimation {
            if #available(iOS 13.0, *) {
                textView.textColor = controller.type.isIncoming || controller.isQuoted ? .c0C1C33 : .white
                textView.linkTextAttributes = [.foregroundColor: controller.type.isIncoming ? UIColor.systemBlue : .systemGray6,
                                               .underlineStyle: 1]
            } else {
                let color = controller.type.isIncoming || controller.isQuoted ? UIColor.black : .white
                textView.textColor = color
                textView.linkTextAttributes = [.foregroundColor: color,
                                               .underlineStyle: 1]
            }
        }
        
        if controller.highlight {
            UIView.animate(withDuration: 1, animations: { [self] in
                self.textView.backgroundColor = .systemRed
            }) { _ in
                UIView.animate(withDuration: 0.5) { [self] in
                    self.textView.backgroundColor = .clear
                }
            }
        }
    }

    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false

        textView.translatesAutoresizingMaskIntoConstraints = false
        textView.isScrollEnabled = false
        textView.isEditable = false
        textView.spellCheckingType = .no
        textView.backgroundColor = .clear
        textView.textContainerInset = .zero
        textView.textContainer.lineFragmentPadding = 0
        textView.dataDetectorTypes = .all
        textView.font = .preferredFont(forTextStyle: .body)
        textView.scrollsToTop = false
        textView.bounces = false
        textView.bouncesZoom = false
        textView.showsHorizontalScrollIndicator = false
        textView.showsVerticalScrollIndicator = false
        textView.isExclusiveTouch = true

        addSubview(textView)
        NSLayoutConstraint.activate([
            textView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            textView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            textView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            textView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        textViewWidthConstraint = textView.widthAnchor.constraint(lessThanOrEqualToConstant: viewPortWidth)
        textViewWidthConstraint?.isActive = true
        
        let longPress = TextMessageViewLongPressGestureRecognizer()
        longPress.rx.event.filter { (gesture: UILongPressGestureRecognizer) -> Bool in
            gesture.state == .began
        }.subscribe(onNext: { [weak self] _ in
            guard let controller = self?.controller else { return }
            controller.longPressAction()
        }).disposed(by: self.disposeBag)
        textView.addGestureRecognizer(longPress)
    }

    private func setupSize() {
        UIView.performWithoutAnimation {
            self.textViewWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth
            setNeedsLayout()
        }
    }
    
    @objc
    private func longTap() {
        controller?.longPressAction()
    }

}

// 解决在textView上手势冲突问题
class TextMessageViewLongPressGestureRecognizer: UILongPressGestureRecognizer {
    override func canBePrevented(by otherGestureRecognizer: UIGestureRecognizer) -> Bool {
        return false
    }
}

/// UITextView with hacks to avoid selection
private final class MessageTextView: UITextView {

    override var isFocused: Bool {
        false
    }

    override var canBecomeFirstResponder: Bool {
        false
    }

    override var canBecomeFocused: Bool {
        false
    }

    override func canPerformAction(_ action: Selector, withSender sender: Any?) -> Bool {
        false
    }

}
