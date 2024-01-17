//
//  VideoQuoteView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/1/17.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class VideoQuoteView: UIView, ContainerCollectionViewCellDelegate {
    
    private lazy var titleLabel = UILabel(frame: bounds)
    
    private lazy var stackView = UIStackView(frame: bounds)
    private lazy var videoStackView = UIStackView(frame: bounds)
    
    private lazy var loadingIndicator = UIActivityIndicatorView(style: .gray)
    
    private lazy var imageView = UIImageView(frame: bounds)
    
    private lazy var playImageView = UIImageView(image: UIImage(systemName: "play.circle")?.withRenderingMode(.alwaysTemplate))
    
    private var controller: VideoController!
    
    private var labelWidthConstraint: NSLayoutConstraint?
    
    private var imageWidthConstraint: NSLayoutConstraint?
    
    private var imageHeightConstraint: NSLayoutConstraint?
    
    private var viewPortWidth: CGFloat = 300
    private let quoteImageViewWidth: CGFloat = 44
    private let loadingIndicatorWidth: CGFloat = 20
    private let quoteItemSpace: CGFloat = 5
    
    private lazy var durationLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.textColor = .white
        v.translatesAutoresizingMaskIntoConstraints = false
        v.font = .f12
        
        return v
    }()
    
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
    
    func setup(with controller: VideoController) {
        self.controller = controller
    }
    
    func reloadData() {
        UIView.performWithoutAnimation {
            titleLabel.text = controller.senderNickname
            durationLabel.text = controller.duration
            switch controller.state {
            case .loading:
                loadingIndicator.isHidden = false
                imageView.isHidden = true
                imageView.image = nil
                videoStackView.removeArrangedSubview(imageView)
                videoStackView.addArrangedSubview(loadingIndicator)
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
                videoStackView.removeArrangedSubview(loadingIndicator)
                videoStackView.addArrangedSubview(imageView)
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
        stackView.addArrangedSubview(videoStackView)
        
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
        
        imageView.addSubview(playImageView)
        playImageView.translatesAutoresizingMaskIntoConstraints = false
        playImageView.tintColor = .white
        
        NSLayoutConstraint.activate([
            playImageView.centerYAnchor.constraint(equalTo: imageView.centerYAnchor),
            playImageView.centerXAnchor.constraint(equalTo: imageView.centerXAnchor),
            playImageView.widthAnchor.constraint(equalToConstant: loadingIndicatorWidth),
            playImageView.heightAnchor.constraint(equalToConstant: loadingIndicatorWidth),
        ])
        
        let loadingWidthConstraint = loadingIndicator.widthAnchor.constraint(equalToConstant: loadingIndicatorWidth)
        loadingWidthConstraint.priority = UILayoutPriority(999)
        loadingWidthConstraint.isActive = true
        
        let loadingHeightConstraint = loadingIndicator.heightAnchor.constraint(equalToConstant: quoteImageViewWidth)
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
