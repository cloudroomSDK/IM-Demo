//
//  ImageQuoteView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/1/17.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class ImageQuoteView: UIView, ContainerCollectionViewCellDelegate {

    private lazy var titleLabel = UILabel(frame: bounds)
    
    private lazy var stackView = UIStackView(frame: bounds)
    private lazy var imageStackView = UIStackView(frame: bounds)

    private lazy var loadingIndicator = UIActivityIndicatorView(style: .gray)

    private lazy var imageView = UIImageView(frame: bounds)
    
    private var controller: ImageController!

    private var labelWidthConstraint: NSLayoutConstraint?
    
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
        imageView.image = nil
    }

    func apply(_ layoutAttributes: ChatLayoutAttributes) {
        viewPortWidth = layoutAttributes.layoutFrame.width
        setupSize()
    }

    func setup(with controller: ImageController) {
        self.controller = controller
    }

    func reloadData() {
        UIView.performWithoutAnimation {
            titleLabel.text = controller.senderNickname
            switch controller.state {
            case .loading:
                loadingIndicator.isHidden = false
                imageView.isHidden = true
                imageView.image = nil
                imageStackView.removeArrangedSubview(imageView)
                imageStackView.addArrangedSubview(loadingIndicator)
                if !loadingIndicator.isAnimating {
                    loadingIndicator.startAnimating()
                }
                if #available(iOS 13.0, *) {
                    backgroundColor = .systemGray5
                } else {
                    backgroundColor = UIColor(red: 200 / 255, green: 200 / 255, blue: 200 / 255, alpha: 1)
                }
                setupSize()
            case let .image(image):
                loadingIndicator.isHidden = true
                loadingIndicator.stopAnimating()
                imageView.isHidden = false
                imageView.image = image
                imageStackView.removeArrangedSubview(loadingIndicator)
                imageStackView.addArrangedSubview(imageView)
                setupSize()
                backgroundColor = .clear
            }
        }
    }

    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false
        
        titleLabel.font = .preferredFont(forTextStyle: .footnote)
        titleLabel.textColor = .black
        
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
        stackView.addArrangedSubview(imageStackView)
        
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        labelWidthConstraint = titleLabel.widthAnchor.constraint(lessThanOrEqualToConstant: viewPortWidth - 50 - quoteItemSpace)
        labelWidthConstraint?.priority = UILayoutPriority(999)
        labelWidthConstraint?.isActive = true

        imageView.translatesAutoresizingMaskIntoConstraints = false
        imageView.contentMode = .scaleAspectFill
        imageView.isHidden = true
        imageView.clipsToBounds = true
        
        let tap = UITapGestureRecognizer(target: self, action: #selector(tap))
        imageView.isUserInteractionEnabled = true
        imageView.addGestureRecognizer(tap)
        
        let longPress = UILongPressGestureRecognizer(target: self, action: #selector(longPress))
        imageView.addGestureRecognizer(longPress)

        loadingIndicator.translatesAutoresizingMaskIntoConstraints = false
        loadingIndicator.isHidden = true

        let loadingWidthConstraint = loadingIndicator.widthAnchor.constraint(equalToConstant: loadingIndicatorWidth)
        loadingWidthConstraint.priority = UILayoutPriority(999)
        loadingWidthConstraint.isActive = true

        let loadingHeightConstraint = loadingIndicator.heightAnchor.constraint(equalTo: loadingIndicator.widthAnchor, multiplier: 1)
        loadingHeightConstraint.priority = UILayoutPriority(999)
        loadingHeightConstraint.isActive = true

        imageWidthConstraint = imageView.widthAnchor.constraint(equalToConstant: quoteImageViewWidth)
        imageWidthConstraint?.priority = UILayoutPriority(999)
        imageWidthConstraint?.isActive = true

        imageHeightConstraint = imageView.heightAnchor.constraint(equalTo: imageView.widthAnchor, multiplier: 1)
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
            labelWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth - quoteImageViewWidth - quoteItemSpace
            setNeedsLayout()
        }
    }

}
