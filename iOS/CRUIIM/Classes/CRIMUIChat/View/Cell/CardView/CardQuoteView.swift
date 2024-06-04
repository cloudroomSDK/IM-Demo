//
//  CardQuoteView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/2/23.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class CardQuoteView: UIView, ContainerCollectionViewCellDelegate {

    private lazy var titleLabel = UILabel(frame: bounds)
    
    private lazy var stackView = UIStackView(frame: bounds)

    private lazy var avatarView = RoundedCornersContainerView<AvatarView>(frame: bounds)
    
    private lazy var nameLabel = UILabel(frame: bounds)
    
    private var controller: CardController!

    private var stackViewWidthConstraint: NSLayoutConstraint?
    
    private var imageWidthConstraint: NSLayoutConstraint?

    private var imageHeightConstraint: NSLayoutConstraint?

    private var viewPortWidth: CGFloat = 300
    
    private let quoteImageViewWidth: CGFloat = 44
    private let loadingIndicatorWidth: CGFloat = 20
    private let quoteItemSpace: CGFloat = 5
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupSubviews()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupSubviews()
    }

    func prepareForReuse() {
        avatarView.customView.reset()
    }

    func apply(_ layoutAttributes: ChatLayoutAttributes) {
        viewPortWidth = layoutAttributes.layoutFrame.width
        setupSize()
    }

    func setup(with controller: CardController) {
        self.controller = controller
    }

    func reloadData() {
        UIView.performWithoutAnimation {
            titleLabel.text = controller.senderNickname
            nameLabel.text = "[名片]".innerLocalized() + (controller.cardNickname ?? "")
            avatarView.customView.setAvatar(url: controller.faceURL, text: nil, placeHolder: "contact_my_friend_icon") {

            }
        }
    }

    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false
        
        titleLabel.font = .preferredFont(forTextStyle: .footnote)
        titleLabel.textColor = .black
        
        nameLabel.font = .preferredFont(forTextStyle: .footnote)
        nameLabel.textColor = .black
        
        stackView.alignment = .top
        stackView.spacing = quoteItemSpace

        addSubview(stackView)
        stackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        stackView.addArrangedSubview(titleLabel)
        stackView.addArrangedSubview(nameLabel)
        stackView.addArrangedSubview(avatarView)
        
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        nameLabel.translatesAutoresizingMaskIntoConstraints = false
        nameLabel.setContentHuggingPriority(.defaultLow, for: .horizontal)
        
        stackViewWidthConstraint = stackView.widthAnchor.constraint(lessThanOrEqualToConstant: viewPortWidth)
        stackViewWidthConstraint?.priority = UILayoutPriority(999)
        stackViewWidthConstraint?.isActive = true

        avatarView.translatesAutoresizingMaskIntoConstraints = false
        
        avatarView.customView.isUserInteractionEnabled = false
        let tap = UITapGestureRecognizer(target: self, action: #selector(tap))
        isUserInteractionEnabled = true
        addGestureRecognizer(tap)
        
        let longPress = UILongPressGestureRecognizer(target: self, action: #selector(longPress))
        addGestureRecognizer(longPress)

        imageWidthConstraint = avatarView.widthAnchor.constraint(equalToConstant: quoteImageViewWidth)
        imageWidthConstraint?.priority = UILayoutPriority(999)
        imageWidthConstraint?.isActive = true

        imageHeightConstraint = avatarView.heightAnchor.constraint(equalTo: avatarView.widthAnchor, multiplier: 1)
        imageHeightConstraint?.priority = UILayoutPriority(999)
        imageHeightConstraint?.isActive = true
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
            stackViewWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth
            setNeedsLayout()
        }
    }

}

