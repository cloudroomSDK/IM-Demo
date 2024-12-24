//
//  ProcessApplicationViewController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2023/12/4.
//

import CRUICore
import ProgressHUD

class ProcessApplicationViewController: UIViewController {

    lazy var nickNameLabel: UILabel = {
        let v = UILabel()
        v.font = .f14
        v.textColor = .c999999
        v.text = "设置备注"
        
        return v
    }()
    
    lazy var descTextView: UITextView = {
        let v = UITextView()
        v.font = .f17
        v.textColor = .c0C1C33
        v.backgroundColor = .cE8EAEF
        v.layer.cornerRadius = 5
        v.isEditable = true
        v.contentInset = UIEdgeInsets(top: 8, left: 8, bottom: 8, right: 8)
        
        return v
    }()
    
    lazy var reqButton: UIButton = {
        let v = UIButton(type: .system)
        v.setTitle("完成".innerLocalized(), for: .normal)
        v.backgroundColor = .c0089FF
        v.tintColor = .white
        v.layer.cornerRadius = 5
        
        v.rx.tap.subscribe { [weak self] _ in
            self?.sendFriendReq()
        }
        
        return v
    }()
    
    private let _viewModel: ApplicationViewModel
    
    init(viewModel: ApplicationViewModel) {
        _viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        replaceSystemBackBarButtonItem()
        navigationItem.title = "通过朋友验证".innerLocalized()
        view.backgroundColor = .viewBackgroundColor
        
        // 名字区域
        let bStack = UIStackView(arrangedSubviews: [nickNameLabel])
        bStack.axis = .vertical
        bStack.spacing = 4
        
        let temp1 = UIView()
        let temp2 = UIView()
        let btnStack = UIStackView(arrangedSubviews: [temp1, reqButton, temp2])
        btnStack.alignment = .center
        btnStack.spacing = 8
        btnStack.distribution = .fillEqually
        
        let vStack = UIStackView(arrangedSubviews: [bStack, descTextView, btnStack])
        vStack.spacing = 16
        vStack.axis = .vertical
        
        let bgView = UIView()
        bgView.backgroundColor = .cellBackgroundColor
        view.addSubview(bgView)
        
        bgView.snp.makeConstraints { make in
            make.leading.trailing.equalToSuperview()
            make.top.equalTo(view.safeAreaLayoutGuide.snp.top).offset(Int.margin8)
        }
        
        bgView.addSubview(vStack)
        vStack.snp.makeConstraints { make in
            make.edges.equalToSuperview().inset(Int.margin16)
        }
        
        descTextView.snp.makeConstraints { make in
            make.height.equalTo(80)
        }
        
        reqButton.snp.makeConstraints { make in
            make.height.equalTo(44)
        }
        
        descTextView.text = _viewModel.friendApplication.fromNickname
    }
    
    func sendFriendReq() {
        ProgressHUD.animate()
        _viewModel.accept { [weak self] r in
            if r == nil {
                ProgressHUD.dismiss()
                
                if let remark = self?.descTextView.text, self?._viewModel.friendApplication.fromNickname != remark {
                    self?._viewModel.saveRemark(remark: remark, onSuccess: { r in
                        
                    })
                }
                self?.navigationController?.popToRootViewController(animated: true)
            } else {
                ProgressHUD.error(r)
            }
        }
    }

}
