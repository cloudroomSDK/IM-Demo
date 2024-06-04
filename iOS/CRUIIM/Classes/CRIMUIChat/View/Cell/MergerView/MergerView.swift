//
//  MergerView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/19.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class MergerView: UIView, ContainerCollectionViewCellDelegate {

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
    
    private lazy var partingView: UIView = {
        let v = UIView()
        v.backgroundColor = .cDCDCDC
        v.translatesAutoresizingMaskIntoConstraints = false
        
        return v
    }()
    
    private lazy var abstractLabel: UILabel = {
        let v = UILabel()
        v.backgroundColor = .clear
        v.textColor = .c999999
        v.translatesAutoresizingMaskIntoConstraints = false
        v.font = .preferredFont(forTextStyle: .footnote)
        v.numberOfLines = 0
        
        return v
    }()

    private lazy var imageView = UIImageView(frame: bounds)
    
    private var controller: MergerController!

    private var stackViewWidthConstraint: NSLayoutConstraint?

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
        titleLabel.text = nil
        abstractLabel.text = nil
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
            titleLabel.text = controller.title
            abstractLabel.text = controller.abstract
        }
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
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor),
            partingView.heightAnchor.constraint(equalToConstant: 1/UIScreen.main.scale)
        ])
        
        stackView.addArrangedSubview(titleLabel)
        stackView.addArrangedSubview(partingView)
        stackView.addArrangedSubview(abstractLabel)

        stackViewWidthConstraint = stackView.widthAnchor.constraint(equalToConstant: viewPortWidth - 30)
        stackViewWidthConstraint?.isActive = true
        
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
            stackViewWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth - 30
            setNeedsLayout()
        }
    }

}
