//
//  FileView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/7.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class FileView: UIView, ContainerCollectionViewCellDelegate {

    private lazy var stackView = UIStackView(frame: bounds)
    
    private lazy var titleLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.textColor = .c353535
        v.translatesAutoresizingMaskIntoConstraints = false
        v.numberOfLines = 2
        v.lineBreakMode = .byTruncatingMiddle
        v.font = .preferredFont(forTextStyle: .callout)
        
        return v
    }()
    
    private lazy var detailLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.textColor = .c999999
        v.translatesAutoresizingMaskIntoConstraints = false
        v.font = .preferredFont(forTextStyle: .footnote)
        
        return v
    }()
    
    private lazy var progressView: UIProgressView = {
        let v = UIProgressView()
        v.trackTintColor = .c999999
        v.progressTintColor = .c5AD439
        v.progress = 0
        v.transform = CGAffineTransformMakeScale(1.0, 0.3)
        
        return v
    }()

    private lazy var imageView = UIImageView(frame: bounds)
    
    private var controller: FileController!

    private var vStackViewWidthConstraint: NSLayoutConstraint?

    private var viewPortWidth: CGFloat = 300
    private let hSpacing: CGFloat = 8
    private let fileIconWidth: CGFloat = 44
    
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
        progressView.isHidden = true
    }

    func apply(_ layoutAttributes: ChatLayoutAttributes) {
        viewPortWidth = layoutAttributes.layoutFrame.width
        setupSize()
    }

    func setup(with controller: FileController) {
        self.controller = controller
    }

    func reloadData() {
        UIView.performWithoutAnimation {
            titleLabel.text = controller.fileName
            if controller.messageType == .outgoing && (controller.status == .sent || controller.status == .read) {
                detailLabel.text = controller.fileSizeStr + " / " + "已发送".innerLocalized()
            } else if controller.messageType == .incoming && (controller.status == .received || controller.status == .read), controller.fileExist {
                detailLabel.text = controller.fileSizeStr + " / " + "已下载".innerLocalized()
            } else {
                detailLabel.text = controller.fileSizeStr
            }
            let imageName = DocumentType.detectDocumentType(for: controller.fileName)?.getIconImageName()
            imageView.image = UIImage(nameInBundle: imageName ?? "chat_msg_file_unknown_normal_icon")
            progressView.alpha = 0
        }
    }
    
    func updateProgress() {
        if progressView.alpha == 0 {
            progressView.alpha = 1
        }
        progressView.progress = Float(controller.progress)/100.0
    }

    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false

        addSubview(stackView)
        stackView.axis = .vertical
        stackView.spacing = 3
        stackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        let vStackView = UIStackView(arrangedSubviews: [titleLabel, detailLabel])
        vStackView.axis = .vertical
        vStackView.spacing = 5
        
        let hStackView = UIStackView(arrangedSubviews: [vStackView, imageView])
        hStackView.alignment = .top
        hStackView.spacing = 8
        
        stackView.addArrangedSubview(hStackView)
        stackView.addArrangedSubview(progressView)
        
        vStackView.translatesAutoresizingMaskIntoConstraints = false
        vStackViewWidthConstraint = vStackView.widthAnchor.constraint(equalToConstant: viewPortWidth - fileIconWidth - hSpacing - 30)
        vStackViewWidthConstraint?.isActive = true

        imageView.translatesAutoresizingMaskIntoConstraints = false
        imageView.contentMode = .scaleAspectFill
        
        let imageHeightConstraint = imageView.heightAnchor.constraint(equalToConstant: fileIconWidth)
        imageHeightConstraint.priority = UILayoutPriority(999)
        imageHeightConstraint.isActive = true
        let imageWidthConstraint = imageView.widthAnchor.constraint(equalTo: imageView.heightAnchor, multiplier: 114.0/132)
        imageWidthConstraint.priority = UILayoutPriority(999)
        imageWidthConstraint.isActive = true
        
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
            vStackViewWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth - fileIconWidth - hSpacing - 30
            setNeedsLayout()
        }
    }

}

