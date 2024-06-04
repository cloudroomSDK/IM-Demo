//
//  SeparatorView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/22.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore

final class SeparatorView: UIView {
    
    private lazy var separatorView: UIView = {
        let v = UIView()
        v.backgroundColor = .cDCDCDC
        v.translatesAutoresizingMaskIntoConstraints = false
        
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
    
    private func setupSubviews() {
        addSubview(separatorView)
        NSLayoutConstraint.activate([
            separatorView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            separatorView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            separatorView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor, constant: 44),
            separatorView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor),
            separatorView.heightAnchor.constraint(equalToConstant: 1/UIScreen.main.scale)
        ])
    }
    
}
