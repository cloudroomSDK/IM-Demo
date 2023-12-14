//
//  ConfigAuthenTypeView.swift
//  CRIMSDKUIKit_Example
//
//  Created by YunWu01 on 2023/9/22.
//  Copyright © 2023 rentsoft. All rights reserved.
//

import UIKit
import RxSwift
import CRUICore

class ConfigAuthenTypeView: UIView {
    static let shared: ConfigAuthenTypeView = .init()
    public static func show(onWindowOf view: UIView, alertTitle: String, currentItem: String, options: [String], confirmTitle: String, confirmAction: @escaping CallBack.StringOptionalReturnVoid) {
        guard let window = view.window else { return }
        window.addSubview(ConfigAuthenTypeView.shared)
        ConfigAuthenTypeView.shared.confirmBtn.setTitle(confirmTitle, for: .normal)
        shared.titleLabel.text = alertTitle
        shared.confirmBtn.setTitle(confirmTitle, for: .normal)
        shared.selectItem = currentItem
        shared.disposeBag = DisposeBag()
        shared.confirmBtn.rx.tap.subscribe(onNext: { [weak shared] in
            confirmAction(shared?.selectItem)
            shared?.removeFromSuperview()
            shared?.disposeBag = DisposeBag()
        }).disposed(by: shared.disposeBag)
        shared.cancelBtn.rx.tap.subscribe(onNext: { [weak shared] in
            shared?.removeFromSuperview()
            shared?.disposeBag = DisposeBag()
        }).disposed(by: shared.disposeBag)
        shared.itemBtn_1.rx.tap.subscribe(onNext: { [weak shared] in
            shared?.selectItem = shared?.itemBtn_1.title(for: .normal) ?? ""
            shared?.itemBtn_1.setTitleColor(StandardUI.color_333333, for: .normal)
            shared?.itemBtn_2.setTitleColor(StandardUI.color_999999, for: .normal)
        }).disposed(by: shared.disposeBag)
        shared.itemBtn_2.rx.tap.subscribe(onNext: { [weak shared] in
            shared?.selectItem = shared?.itemBtn_2.title(for: .normal) ?? ""
            shared?.itemBtn_2.setTitleColor(StandardUI.color_333333, for: .normal)
            shared?.itemBtn_1.setTitleColor(StandardUI.color_999999, for: .normal)
        }).disposed(by: shared.disposeBag)
        ConfigAuthenTypeView.shared.frame = window.bounds
        
        for itemButton in shared.itemViews {
            if let i = shared.itemViews.firstIndex(of: itemButton), i < options.count {
                let item = options[i]
                itemButton.setTitle(item, for: .normal)
            }
            
            if currentItem == itemButton.title(for: .normal) {
                itemButton.setTitleColor(StandardUI.color_333333, for: .normal)
            } else {
                itemButton.setTitleColor(StandardUI.color_999999, for: .normal)
            }
        }
    }

    private var disposeBag = DisposeBag()
    private var selectItem = ""
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
    
    private lazy var itemBtn_1: UIButton = {
        let v = UIButton()
        v.setTitle("item".innerLocalized(), for: .normal)
        v.setTitleColor(StandardUI.color_999999, for: .normal)
        v.titleLabel?.font = UIFont.systemFont(ofSize: 16)
        
        return v
    }()
    
    private lazy var itemBtn_2: UIButton = {
        let v = UIButton()
        v.setTitle("item".innerLocalized(), for: .normal)
        v.setTitleColor(StandardUI.color_999999, for: .normal)
        v.titleLabel?.font = UIFont.systemFont(ofSize: 16)
        v.rx.tap.subscribe(onNext: { [weak self] in
            self?.selectItem = v.title(for: .normal) ?? ""
            v.setTitleColor(StandardUI.color_333333, for: .normal)
            self?.itemBtn_1.setTitleColor(StandardUI.color_999999, for: .normal)
        }).disposed(by: disposeBag)
        
        return v
    }()

    override init(frame: CGRect) {
        super.init(frame: frame)
        backgroundColor = UIColor(white: 0, alpha: 0.6)
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

        itemViews = [itemBtn_1, itemBtn_2]
        let vStack: UIStackView = {
            let v = UIStackView(arrangedSubviews: itemViews)
            v.axis = .vertical
            return v
        }()
        container.addSubview(vStack)
        vStack.snp.makeConstraints { make in
            make.top.equalTo(hStack.snp_bottom).offset(8)
            make.left.right.equalToSuperview()
            make.height.equalTo(46*2)
            make.bottom.equalToSuperview().offset(-30)
        }
        itemBtn_1.snp.makeConstraints { make in
            make.height.equalTo(46)
        }
        itemBtn_2.snp.makeConstraints { make in
            make.height.equalTo(46)
        }
    }

    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}
