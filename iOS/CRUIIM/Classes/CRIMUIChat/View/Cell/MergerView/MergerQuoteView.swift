//
//  MergerQuoteView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/19.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class MergerQuoteView: UIView, ContainerCollectionViewCellDelegate {

    private lazy var stackView = UIStackView(frame: bounds)
    
    private lazy var titleLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.textColor = .c353535
        v.translatesAutoresizingMaskIntoConstraints = false
        v.numberOfLines = 2
        v.lineBreakMode = .byTruncatingMiddle
        v.font = .preferredFont(forTextStyle: .footnote)
        
        return v
    }()
    
    private var controller: MergerController!

    private var labelWidthConstraint: NSLayoutConstraint?

    private var viewPortWidth: CGFloat = 300
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupSubviews()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupSubviews()
    }

    func prepareForReuse() {
        
    }

    func apply(_ layoutAttributes: ChatLayoutAttributes) {
        viewPortWidth = layoutAttributes.layoutFrame.width
        setupSize()
    }

    func setup(with controller: MergerController) {
        self.controller = controller
    }

    func reloadData() {
        UIView.performWithoutAnimation {
            titleLabel.text = controller.senderNickname
        }
    }

    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false

        addSubview(stackView)
        stackView.spacing = 5
        stackView.alignment = .top
        stackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        stackView.addArrangedSubview(titleLabel)
        
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        labelWidthConstraint = titleLabel.widthAnchor.constraint(lessThanOrEqualToConstant: viewPortWidth)
        labelWidthConstraint?.isActive = true
        
        let tap = UITapGestureRecognizer(target: self, action: #selector(tap))
        isUserInteractionEnabled = true
        addGestureRecognizer(tap)
        
        let longPress = UILongPressGestureRecognizer(target: self, action: #selector(longPress))
        addGestureRecognizer(longPress)
    }
    
    @objc
    private func tap() {
        controller?.action()
    }
    
    @objc
    private func longPress() {
        controller?.longPressAction()
    }

    private func setupSize() {
        UIView.performWithoutAnimation {
            labelWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth
            setNeedsLayout()
        }
    }

}
