//
//  AudioView.swift
//  Alamofire
//
//  Created by YunWu01 on 2024/3/1.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore
import Lottie

final class AudioView: UIView, ContainerCollectionViewCellDelegate {
    
    private lazy var stackView = UIStackView(frame: bounds)
        
    private lazy var audioIconLeftImageView: AnimationView = {
        let bundle = ViewControllerFactory.getBundle() ?? Bundle.main
        let v = AnimationView(name: "voice_black", bundle: bundle)
        v.loopMode = .loop
        v.currentProgress = 1
        v.setContentHuggingPriority(.required, for: .horizontal)
        v.setContentCompressionResistancePriority(.required, for: .horizontal)
        return v
    }()
    
    private lazy var durationLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.textColor = .c353535
        v.translatesAutoresizingMaskIntoConstraints = false
        v.font = .f12
        
        return v
    }()
    
    private lazy var audioIconRightImageView: AnimationView = {
        let bundle = ViewControllerFactory.getBundle() ?? Bundle.main
        let v = AnimationView(name: "voice_black", bundle: bundle)
        v.loopMode = .loop
        v.currentProgress = 1
        v.setContentHuggingPriority(.required, for: .horizontal)
        v.setContentCompressionResistancePriority(.required, for: .horizontal)
        return v
    }()
        
    private var controller: AudioController!
    
    private var lableMaxWidthConstraint: NSLayoutConstraint?
        
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
        audioIconLeftImageView.currentProgress = 1
        audioIconLeftImageView.stop()
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
            durationLabel.text = #"\#(FormatUtil.getMediaFormat(of: controller.duration))"#
            
            if controller.messageType.isIncoming {
                audioIconRightImageView.isHidden = true
                audioIconLeftImageView.isHidden = false
                durationLabel.textAlignment = .left
                
                if controller.isPlaying {
                    audioIconLeftImageView.play()
                } else {
                    audioIconLeftImageView.stop()
                    audioIconLeftImageView.currentProgress = 1
                }
            } else {
                audioIconRightImageView.isHidden = false
                audioIconLeftImageView.isHidden = true
                durationLabel.textAlignment = .right
                
                if controller.isPlaying {
                    audioIconRightImageView.play()
                } else {
                    audioIconRightImageView.stop()
                    audioIconRightImageView.currentProgress = 1
                }
            }

            
            
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
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor, constant: 12),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor, constant: -12),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        stackView.addArrangedSubview(audioIconLeftImageView)
        stackView.addArrangedSubview(durationLabel)
        stackView.addArrangedSubview(audioIconRightImageView)
        audioIconRightImageView.transform = CGAffineTransform(rotationAngle: CGFloat.pi)
        
        lableMaxWidthConstraint = durationLabel.widthAnchor.constraint(equalToConstant: 60)
        lableMaxWidthConstraint?.isActive = true
        
        let lableMinWidthConstraint = durationLabel.widthAnchor.constraint(greaterThanOrEqualToConstant: 60)
        //lableMinWidthConstraint.isActive = true
        
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
            let width = MessageHelper.getAudioMessageDisplayWidth(duration: controller.duration)
            lableMaxWidthConstraint?.constant = width
        }
    }
    
}
