//
//  FileQuoteView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/7.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class FileQuoteView: UIView, ContainerCollectionViewCellDelegate {

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

    private lazy var imageView = UIImageView(frame: bounds)
    
    private var controller: FileController!

    private var labelWidthConstraint: NSLayoutConstraint?

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
            titleLabel.text = controller.senderNickname + controller.fileName
            let imageName = DocumentType.detectDocumentType(for: controller.fileName)?.getIconImageName()
            imageView.image = UIImage(nameInBundle: imageName ?? "chat_msg_file_unknown_normal_icon")
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
        stackView.addArrangedSubview(imageView)
        
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        labelWidthConstraint = titleLabel.widthAnchor.constraint(equalToConstant: viewPortWidth - fileIconWidth - hSpacing - 30)
        labelWidthConstraint?.isActive = true

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
            labelWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth - fileIconWidth - hSpacing - 30
            setNeedsLayout()
        }
    }

}
