//
//  ConfigAuthenTypeView.swift
//  CRIMSDKUIKit_Example
//
//  Created by YunWu01 on 2023/9/22.
//  Copyright © 2023 tom. All rights reserved.
//

import UIKit
import RxSwift
import CRUICore

public typealias CheckOptionReturnVoid = (_ index: Int, _ itemTitle: String) -> Void

class ConfigPickerView: UIView {
    static let shared: ConfigPickerView = .init()
    public static func show(onWindowOf view: UIView, alertTitle: String, currentItem: String, options: [String], cancelTitle: String = "取消".innerLocalized(), confirmTitle: String = "确定".innerLocalized(), confirmAction: @escaping CheckOptionReturnVoid) {
        guard let window = view.window else { return }
        window.addSubview(ConfigPickerView.shared)
        ConfigPickerView.shared.confirmBtn.setTitle(confirmTitle, for: .normal)
        shared.titleLabel.text = alertTitle
        shared.confirmBtn.setTitle(confirmTitle, for: .normal)
        shared.selectItem = currentItem
        shared.disposeBag = DisposeBag()
        shared.items.removeAll()
        shared.items.append(contentsOf: options)
        shared.setupContentView()
        shared.frame = window.bounds
        
        shared.confirmBtn.rx.tap.subscribe(onNext: { [weak shared] in
            guard let shared else { return }
            let index = options.firstIndex(of: shared.selectItem) ?? 0
            confirmAction(index, shared.selectItem)
            shared.removeFromSuperview()
            shared.disposeBag = DisposeBag()
        }).disposed(by: shared.disposeBag)
        shared.cancelBtn.rx.tap.subscribe(onNext: { [weak shared] in
            shared?.removeFromSuperview()
            shared?.disposeBag = DisposeBag()
        }).disposed(by: shared.disposeBag)
        
        
        shared.updateViewItemsState()
    }

    private var disposeBag = DisposeBag()
    private var selectItem = ""
    private var items = [String]()
    private var itemViews = [UIButton]()
    private lazy var cancelBtn: UIButton = {
        let v = UIButton()
        v.setTitle("取消".innerLocalized(), for: .normal)
        v.setTitleColor(StandardUI.color_333333, for: .normal)
        v.titleLabel?.font = UIFont.systemFont(ofSize: 16)
        return v
    }()

    private lazy var confirmBtn: UIButton = {
        let v = UIButton()
        v.setTitle("确定".innerLocalized(), for: .normal)
        v.setTitleColor(.c0089FF, for: .normal)
        v.titleLabel?.font = UIFont.systemFont(ofSize: 16)
        return v
    }()

    private let titleLabel: UILabel = {
        let v = UILabel()
        v.textAlignment = .center
        v.numberOfLines = 0
        return v
    }()

    override init(frame: CGRect) {
        super.init(frame: frame)
        backgroundColor = UIColor(white: 0, alpha: 0.6)
    }

    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private func setupContentView() {
        itemViews.removeAll()
        subviews.forEach { view in
            view.removeFromSuperview()
        }
        
        let container: UIView = {
            let v = UIView()
            v.backgroundColor = .white
            v.layer.cornerRadius = 6
            v.clipsToBounds = true
            return v
        }()
        
        addSubview(container)
        container.snp.makeConstraints { make in
            make.left.bottom.right.equalToSuperview().inset(0)
        }
        
        let hStack: UIStackView = {
            let v = UIStackView(arrangedSubviews: [cancelBtn, titleLabel, confirmBtn])
            v.axis = .horizontal
            return v
        }()
        container.addSubview(hStack)
        hStack.snp.makeConstraints { make in
            make.top.equalToSuperview()
            make.left.equalToSuperview().offset(12)
            make.right.equalToSuperview().offset(-12)
            make.height.equalTo(46)
        }
        cancelBtn.snp.makeConstraints { make in
            make.width.equalTo(confirmBtn.snp_width)
        }
        
        // 根据数据源创建UI
        items.forEach { title in
            let itemBtn: UIButton = {
                let v = UIButton()
                v.setTitle(title, for: .normal)
                v.setTitleColor(StandardUI.color_999999, for: .normal)
                v.titleLabel?.font = UIFont.systemFont(ofSize: 16)
                v.rx.tap.subscribe(onNext: { [weak self] in
                    guard let self else { return }
                    self.selectItem = v.title(for: .normal) ?? ""
                    self.updateViewItemsState()
                }).disposed(by: disposeBag)
                
                return v
            }()
            
            itemViews.append(itemBtn)
        }

        let vStack: UIStackView = {
            let v = UIStackView(arrangedSubviews: itemViews)
            v.axis = .vertical
            return v
        }()
        container.addSubview(vStack)
        vStack.snp.makeConstraints { make in
            make.top.equalTo(hStack.snp_bottom).offset(8)
            make.left.right.equalToSuperview()
            make.height.equalTo(46*items.count)
            make.bottom.equalToSuperview().offset(-30)
        }
        itemViews.forEach { itemButton in
            itemButton.snp.makeConstraints { make in
                make.height.equalTo(46)
            }
        }
    }
    
    private func updateViewItemsState() {
        for itemButton in itemViews {
            if let i = itemViews.firstIndex(of: itemButton), i < items.count {
                let item = items[i]
                itemButton.setTitle(item, for: .normal)
            }
            
            if selectItem == itemButton.title(for: .normal) {
                itemButton.setTitleColor(StandardUI.color_333333, for: .normal)
            } else {
                itemButton.setTitleColor(StandardUI.color_999999, for: .normal)
            }
        }
    }
}
