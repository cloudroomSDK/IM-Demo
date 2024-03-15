//
//  VerticalContentContainerView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/2/24.
//

import Foundation
import UIKit
import CRUICore

final class VerticalContentContainerView<CustomView: UIView>: UIView {

    lazy var customView = BezierMaskedView<CustomView>()
    
    lazy var nicknameLabel: PaddedUILabel = {
        let v = PaddedUILabel()
        v.textColor = .c0C1C33
        v.font = .preferredFont(forTextStyle: .footnote)
        v.textInsets = UIEdgeInsets(top: 0, left: 5, bottom: 0, right: 5)
        
        return v
    }()
    
    lazy var statusLabel: PaddedUILabel = {
        let v = PaddedUILabel()
        v.textColor = .c999999
        v.font = .preferredFont(forTextStyle: .footnote)
        v.textInsets = UIEdgeInsets(top: 0, left: 5, bottom: 0, right: 5)
        v.textAlignment = .right
        
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

    override func layoutSubviews() {
        super.layoutSubviews()

    }

    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false
        preservesSuperviewLayoutMargins = false
        
        nicknameLabel.translatesAutoresizingMaskIntoConstraints = false
        customView.translatesAutoresizingMaskIntoConstraints = false
        
        let containerStack = UIStackView(arrangedSubviews: [nicknameLabel, customView, statusLabel])
        containerStack.spacing = 5
        containerStack.axis = .vertical
        containerStack.translatesAutoresizingMaskIntoConstraints = false
        
        addSubview(containerStack)
        
        NSLayoutConstraint.activate([
            containerStack.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            containerStack.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor),
            containerStack.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            containerStack.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
        ])
    }
    
    public func setup(with status: MessageStatus) {
        switch status {
        case .sent:
            statusLabel.text = "未读"
            statusLabel.textColor = .c0089FF
        case .received:
            statusLabel.text = "未读"
            statusLabel.textColor = .c0089FF
        case .read:
            statusLabel.text = "已读"
            statusLabel.textColor = .c999999
        default:
            statusLabel.text = nil
        }
    }
}
