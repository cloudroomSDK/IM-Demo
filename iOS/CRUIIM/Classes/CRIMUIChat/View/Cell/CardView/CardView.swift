//
//  CardView.swift
//  Pods
//
//  Created by YunWu01 on 2024/1/30.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class CardView: UIView, ContainerCollectionViewCellDelegate {
    
    private lazy var titleLabel = UILabel(frame: bounds)
    
    private lazy var stackView = UIStackView(frame: bounds)

    private lazy var loadingIndicator = UIActivityIndicatorView(style: .gray)

    private lazy var avatarView = RoundedCornersContainerView<AvatarView>(frame: bounds)
    
    private var controller: CardController!

    private var titleLabelWidthConstraint: NSLayoutConstraint?
    
    private var imageWidthConstraint: NSLayoutConstraint?

    private var imageHeightConstraint: NSLayoutConstraint?

    private var viewPortWidth: CGFloat = 300
    
    private let avatarImageViewWidth: CGFloat = 44
    private let loadingIndicatorWidth: CGFloat = 20
    private let itemSpace: CGFloat = 5
    
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
            titleLabel.text = controller.cardNickname
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
        
        stackView.spacing = itemSpace

        addSubview(stackView)
        stackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        stackView.addArrangedSubview(avatarView)
        stackView.addArrangedSubview(titleLabel)
        
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        titleLabelWidthConstraint = titleLabel.widthAnchor.constraint(equalToConstant: viewPortWidth - itemSpace - avatarImageViewWidth - 30)
        titleLabelWidthConstraint?.priority = UILayoutPriority(999)
        titleLabelWidthConstraint?.isActive = true

        avatarView.translatesAutoresizingMaskIntoConstraints = false
        
        avatarView.customView.isUserInteractionEnabled = false
        let tap = UITapGestureRecognizer(target: self, action: #selector(tap))
        isUserInteractionEnabled = true
        addGestureRecognizer(tap)
        
        let longPress = UILongPressGestureRecognizer(target: self, action: #selector(longPress))
        addGestureRecognizer(longPress)

        loadingIndicator.translatesAutoresizingMaskIntoConstraints = false
        loadingIndicator.isHidden = true

        let loadingWidthConstraint = loadingIndicator.widthAnchor.constraint(equalToConstant: loadingIndicatorWidth)
        loadingWidthConstraint.priority = UILayoutPriority(999)
        loadingWidthConstraint.isActive = true

        let loadingHeightConstraint = loadingIndicator.heightAnchor.constraint(equalTo: loadingIndicator.widthAnchor, multiplier: 1)
        loadingHeightConstraint.priority = UILayoutPriority(999)
        loadingHeightConstraint.isActive = true

        imageWidthConstraint = avatarView.widthAnchor.constraint(equalToConstant: avatarImageViewWidth)
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
            titleLabelWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth - itemSpace - avatarImageViewWidth - 30
            setNeedsLayout()
        }
    }

}
