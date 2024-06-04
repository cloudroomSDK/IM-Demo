//
//  AudioQuoteView.swift
//  Alamofire
//
//  Created by YunWu01 on 2024/3/1.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore
import Lottie

final class AudioQuoteView: UIView, ContainerCollectionViewCellDelegate {
    
    private lazy var stackView = UIStackView(frame: bounds)
    
    private lazy var titleLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.font = .preferredFont(forTextStyle: .footnote)
        v.textColor = .black
        v.translatesAutoresizingMaskIntoConstraints = false
        
        return v
    }()
        
    private lazy var audioIconImageView: AnimationView = {
        let bundle = ViewControllerFactory.getBundle() ?? Bundle.main
        let v = AnimationView(name: "voice_black", bundle: bundle)
        v.loopMode = .loop
        v.currentProgress = 1
        v.setContentHuggingPriority(.required, for: .horizontal)
        v.setContentCompressionResistancePriority(.required, for: .horizontal)
        return v
    }()
        
    private var controller: AudioController!
    
    private var imageWidthConstraint: NSLayoutConstraint?
    
    private var imageHeightConstraint: NSLayoutConstraint?
    
    private var viewPortWidth: CGFloat = 300
    
    private lazy var durationLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.textColor = .black
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
        audioIconImageView.stop()
        audioIconImageView.currentProgress = 1
    }
    
    func apply(_ layoutAttributes: ChatLayoutAttributes) {
        viewPortWidth = layoutAttributes.layoutFrame.width
        setupSize()
    }
    
    func setup(with controller: AudioController) {
        self.controller = controller
    }
    
    func reloadData() {
        UIView.performWithoutAnimation {
            titleLabel.text = controller.senderNickname
            durationLabel.text = #"\#((String(format: "%d", controller.duration)))""#
        }
    }
    
    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false
        
        stackView.spacing = 5
        
        addSubview(stackView)
        stackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        stackView.addArrangedSubview(titleLabel)
        stackView.addArrangedSubview(audioIconImageView)
        stackView.addArrangedSubview(durationLabel)
        
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
            switch controller.state {
            case .loading:
                imageWidthConstraint?.isActive = false
                imageHeightConstraint?.isActive = false
                setNeedsLayout()
            case let .image(image):
                imageWidthConstraint?.isActive = true
                imageHeightConstraint?.isActive = true
                let maxWidth = min(viewPortWidth * StandardUI.maxWidth / 2, image.size.width)
                imageWidthConstraint?.constant = maxWidth
                imageHeightConstraint?.constant = image.size.height * maxWidth / image.size.width
                setNeedsLayout()
            }
        }
    }
    
}
