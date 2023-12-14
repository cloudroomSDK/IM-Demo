//
//  SendFriendReqViewController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2023/12/4.
//

import CRUICore
import RxSwift
import ProgressHUD

class SendFriendReqViewController: UIViewController {
    
    private let _disposeBag = DisposeBag()
    
    lazy var nickNameLabel: UILabel = {
        let v = UILabel()
        v.font = .f14
        v.textColor = .c999999
        v.text = "发送添加好友申请"
        
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
        
        let maxTextLength = 50
        // 监听UITextView的文本变化
        v.rx.text.orEmpty
            .map { String($0.prefix(maxTextLength)) } // 限制输入文本的长度
            .bind(to: v.rx.text) // 将限制后的文本绑定回UITextView
            .disposed(by: _disposeBag)
        
        return v
    }()
    
    lazy var reqButton: UIButton = {
        let v = UIButton(type: .system)
        v.setTitle("发送".innerLocalized(), for: .normal)
        v.backgroundColor = .c0089FF
        v.tintColor = .white
        v.layer.cornerRadius = 5
        
        v.rx.tap.subscribe { [weak self] _ in
            self?.sendFriendReq()
        }
        
        return v
    }()
    
    private let _viewModel: UserDetailViewModel
    
    init(viewModel: UserDetailViewModel) {
        _viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        replaceSystemBackBarButtonItem()
        navigationItem.title = "申请添加好友".innerLocalized()
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
        
        _viewModel.loginUserPublish.subscribe(onNext: { [weak self] (userInfo: UserInfo?) in
            let nickname = userInfo?.nickname?.isEmpty == false ? userInfo?.nickname : userInfo?.userID
            self?.descTextView.text = "我是" + (nickname ?? "")
        }).disposed(by: _disposeBag)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        _viewModel.getSelfInfo()
    }
    
    func sendFriendReq() {
        ProgressHUD.show()
        
        let reqMsg = descTextView.text ?? ""
        _viewModel.addFriend(reqMsg:reqMsg, onSuccess: { [weak self] res in
            ProgressHUD.showSuccess("添加好友请求已发送".innerLocalized())
            self?.navigationController?.popViewController(animated: true)
        }, onFailure: { (errCode, errMsg) in
            if errCode == SDKError.refuseToAddFriends.rawValue {
                ProgressHUD.showError("该用户已设置不可添加！".innerLocalized())
            }
        })
    }
}
