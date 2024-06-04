//
//  GroupMemberDetailViewController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2023/12/5.
//

import CRUICore
import ProgressHUD
import RxSwift


class GroupMemberDetailViewController: UIViewController {
    private let avatarView = AvatarView()
    
    private let nameLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f17
        v.textColor = UIColor.c0C1C33
        
        return v
    }()
    
    private lazy var IDLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f14
        v.textColor = UIColor.c8E9AB0
        v.isUserInteractionEnabled = true
        
        let tap = UITapGestureRecognizer()
        v.addGestureRecognizer(tap)
        
        tap.rx.event.subscribe(onNext: { [weak self] _ in
            UIPasteboard.general.string = self?._viewModel.userId
            ProgressHUD.showSuccess("ID已复制".innerLocalized())
        }).disposed(by: _disposeBag)
        
        return v
    }()
    
    private lazy var sendMsgBtn: UIButton = {
        let v = UIButton(type: .system)
        v.setImage(UIImage(nameInBundle: "profile_message_button_icon"), for: .normal)
        v.setTitle("发消息".innerLocalized(), for: .normal)
        v.tintColor = .white
        v.titleLabel?.font = UIFont.f17
        v.backgroundColor = UIColor.c0089FF
        v.layer.cornerRadius = 6
        v.titleEdgeInsets = UIEdgeInsets(top: 8, left: 8, bottom: 8, right: 8)
        v.contentHorizontalAlignment = .center
        
        v.rx.tap.subscribe(onNext: { [weak self] _ in
            self?._viewModel.createSingleChat(onComplete: { [weak self] info in
                let vc = ChatViewControllerBuilder().build(info)
                vc.hidesBottomBarWhenPushed = true
                self?.navigationController?.pushViewController(vc, animated: true)
                if let root = self?.navigationController?.viewControllers.first {
                    self?.navigationController?.viewControllers.removeAll(where: { controller in
                        controller != root && controller != vc
                    })
                }
            })
        }).disposed(by: _disposeBag)
        return v
    }()

    private lazy var addFriendBtn: UIButton = {
        let v = UIButton(type: .system)
        v.setTitle("  加好友  ".innerLocalized(), for: .normal)
        v.tintColor = .white
        v.backgroundColor = UIColor.c5AD439
        v.titleLabel?.font = UIFont.f14
        v.layer.cornerRadius = 6
        
        v.rx.tap.subscribe(onNext: { [weak self] _ in
            guard let viewModel = self?._viewModel else { return }
            let vc = SendFriendReqViewController(viewModel: viewModel)
            self?.navigationController?.pushViewController(vc, animated: true)
        }).disposed(by: _disposeBag)
        return v
    }()

    private let _disposeBag = DisposeBag()
    private let _viewModel: UserDetailViewModel
    
    private lazy var _tableView: UITableView = {
        let v = UITableView()
        v.backgroundColor = .clear
        v.separatorStyle = .singleLine
        let headerView: UIView = {
            let v = UIView(frame: CGRect(x: 0, y: 0, width: kScreenWidth, height: 96))
            v.backgroundColor = .tertiarySystemBackground
            
            let vStack: UIStackView = {
                let v = UIStackView(arrangedSubviews: [nameLabel])
                v.axis = .vertical
                v.distribution = .equalCentering
                v.spacing = 4
                
                return v
            }()
            
            let hStack: UIStackView = UIStackView(arrangedSubviews: [avatarView, vStack, UIView(), addFriendBtn])
            hStack.alignment = .center
            hStack.spacing = 8
            
            v.addSubview(hStack)
            hStack.snp.makeConstraints { make in
                make.leading.top.trailing.bottom.equalToSuperview().inset(8)
            }
            
            return v
        }()
        
        v.tableHeaderView = headerView
        v.register(OptionTableViewCell.self, forCellReuseIdentifier: OptionTableViewCell.className)
        v.register(MultipleTextLineCell.self, forCellReuseIdentifier: MultipleTextLineCell.className)
        v.register(SpacerCell.self, forCellReuseIdentifier: SpacerCell.className)
        v.delegate = self
        v.dataSource = self
        v.estimatedSectionHeaderHeight = 0
        v.tableFooterView = UIView(frame: CGRect(x: 0, y: 0, width: kScreenWidth, height: .leastNormalMagnitude))
        
        if #available(iOS 15.0, *) {
            v.sectionHeaderTopPadding = 0
        }
        return v
    }()
    
    private var rowItems: [[RowType]] = [[.spacer]]
    
    init(userId: String, groupId: String?, groupInfo: GroupInfo? = nil, userDetailFor: UserDetailFor = .groupMemberInfo) {
        _viewModel = UserDetailViewModel(userId: userId, groupId: groupId, groupInfo: groupInfo, userDetailFor: userDetailFor)
        super.init(nibName: nil, bundle: nil)
    }
    
    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .viewBackgroundColor
        replaceSystemBackBarButtonItem()

        initView()
        bindData()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        _viewModel.getUserOrMemberInfo()
    }
    private func initView() {
        view.addSubview(_tableView)
        _tableView.snp.makeConstraints { make in
            make.edges.equalToSuperview()
        }
        
        if _viewModel.userId == IMController.shared.userID {
            // 搜索的是自己，需要把几个功能按钮屏蔽掉
            return
        }
        
        let hStack: UIStackView = {
            
            var v: UIStackView = UIStackView(arrangedSubviews: [sendMsgBtn])
            
            if let groupInfo = _viewModel.groupInfo, groupInfo.applyMemberFriend != 0 {
                v = UIStackView(arrangedSubviews: [sendMsgBtn])
            }
            v.spacing = 16
            v.distribution = .fillEqually
            v.alignment = .center
            
            return v
        }()
        
        sendMsgBtn.snp.makeConstraints { make in
            make.height.equalTo(46)
        }

        if let configHandler = CRIMApi.queryConfigHandler {
            configHandler { [weak self] result in
                if let `self` = self,  result != nil, let allowSendMsgNotFriend = result["allowSendMsgNotFriend"] as? Int, allowSendMsgNotFriend != 1 {
                    hStack.removeArrangedSubview(self.sendMsgBtn)
                    self.sendMsgBtn.removeFromSuperview()
                }
            }
        }
        
        view.addSubview(hStack)
        hStack.snp.makeConstraints { make in
            make.bottom.equalTo(self.view.safeAreaLayoutGuide.snp.bottom).offset(-40)
            make.leading.trailing.equalToSuperview().inset(16)
        }
    }
    
    @objc private func rightButtonAction() {
        let vc = UserProfileTableViewController(userId: self._viewModel.userId)
        navigationController?.pushViewController(vc, animated: true)
    }
    
    private func bindData() {
        _viewModel.userInfoRelay.subscribe(onNext: { [weak self] (userInfo: FullUserInfo?) in
            guard let userInfo, let sself = self else {
                self?.addFriendBtn.isHidden = false
                self?.sendMsgBtn.isHidden = true
                return
            }
            
            sself.avatarView.setAvatar(url: userInfo.faceURL, text: nil, placeHolder: "contact_my_friend_icon") { [weak self] in
                guard let self else { return }
                let vc = UserProfileTableViewController.init(userId: self._viewModel.userId, groupId: self._viewModel.groupId)
                self.navigationController?.pushViewController(vc, animated: true)
            }
            var name: String? = userInfo.showName
            if let remark = userInfo.friendInfo?.remark, !remark.isEmpty {
                name = name?.append(string: "(\(remark))")
            }
            sself.nameLabel.text = name
            sself.addFriendBtn.isHidden = userInfo.friendInfo != nil
            //sself.IDLabel.text = userInfo.userID

            if userInfo.userID == IMController.shared.userID {
                sself.addFriendBtn.isHidden = true
                sself.sendMsgBtn.isHidden = true
            } else if userInfo.friendInfo != nil, !sself.rowItems.contains([.profile]) {
                //是好友可以查看详细
                //sself.rowItems.append([.spacer])
                sself.rowItems.append([.profile])
                sself.navigationItem.rightBarButtonItem = UIBarButtonItem(image: UIImage(systemName: "ellipsis"), style: .plain, target: self, action: #selector(sself.rightButtonAction))
            }
            sself._tableView.reloadData()
        }).disposed(by: _disposeBag)
        
        _viewModel.strangerInfoRelay.subscribe(onNext: { [weak self] (userInfo: UserInfo?) in
            guard let userInfo, let sself = self else {
                self?.addFriendBtn.isHidden = true
                self?.sendMsgBtn.isHidden = true
                return
            }
            
            sself.avatarView.setAvatar(url: userInfo.faceURL, text: nil, placeHolder: "contact_my_friend_icon") { [weak self] in
                guard let self else { return }
                let vc = UserProfileTableViewController.init(userId: self._viewModel.userId, groupId: self._viewModel.groupId)
                self.navigationController?.pushViewController(vc, animated: true)
            }
            var name: String? = userInfo.nickname
            sself.nameLabel.text = name
            sself.addFriendBtn.isHidden = false
            sself.IDLabel.text = userInfo.userID

            if userInfo.userID == IMController.shared.userID {
                sself.addFriendBtn.isHidden = true
                sself.sendMsgBtn.isHidden = true
            }
            sself._tableView.reloadData()
        }).disposed(by: _disposeBag)
        
        _viewModel.memberInfoRelay.subscribe(onNext: { [weak self] (memberInfo: GroupMemberInfo?) in
            guard let memberInfo, let sself = self else { return }
            sself.avatarView.setAvatar(url: memberInfo.faceURL, text: nil, placeHolder: "contact_my_friend_icon", onTap: nil)
            sself.nameLabel.text = memberInfo.nickname
            sself.IDLabel.text = memberInfo.userID
            sself.addFriendBtn.isHidden = true
            
            guard sself._viewModel.groupId != nil else { return }
            // 群聊才有以下信息
            guard sself.rowItems.count < 2 else { return }
            sself.rowItems = sself.rowItems.count > 1 ? sself.rowItems : [[.identifier]]
            
            var rowItems: [RowType] = [.nickName]
            if sself._viewModel.showSetAdmin == true {
                rowItems.append(.showSetAdmin)
            }
            if sself._viewModel.showMute == true {
                rowItems.append(.showMute)
            }
            sself.rowItems.append(rowItems)
            
        }).disposed(by: _disposeBag)
    }
    
}

extension GroupMemberDetailViewController: UITableViewDataSource, UITableViewDelegate {
    func numberOfSections(in tableView: UITableView) -> Int {
        return rowItems.count
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return rowItems[section].count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let rowType: RowType = rowItems[indexPath.section][indexPath.row]
        
        if rowType == .spacer {
            return tableView.dequeueReusableCell(withIdentifier: SpacerCell.className, for: indexPath)
        }
        
        let cell = tableView.dequeueReusableCell(withIdentifier: OptionTableViewCell.className, for: indexPath) as! OptionTableViewCell
        
        cell.accessoryType = .none
        cell.subtitleLabel.textAlignment = .left
        
        if rowType == .profile {
            cell.titleLabel.isHidden = true
            cell.subtitleLabel.text = rowType.title
            cell.accessoryType = .disclosureIndicator
            
        }
        
        guard let info = _viewModel.memberInfoRelay.value else { return cell }
        cell.titleLabel.text = rowType.title
        
        if rowType == .identifier {
            cell.subtitleLabel.text = info.userID
            cell.subtitleLabel.textAlignment = .right
        } else if rowType == .nickName {
            cell.subtitleLabel.text = _viewModel.groupInfo?.groupName
            cell.subtitleLabel.textAlignment = .right
        } else if rowType == .showSetAdmin {
            cell.switcher.isHidden = false
            _viewModel.memberInfoRelay.map{
                $0?.roleLevel == .admin
            }.bind(to: cell.switcher.rx.isOn).disposed(by: cell.disposeBag)
            cell.switcher.rx.controlEvent(.valueChanged).subscribe(onNext: { [weak self] in
                guard let self, let roleLevel = self._viewModel.memberInfoRelay.value?.roleLevel else { return }
                self._viewModel.toggleSetAdmin(toAdmin: roleLevel != .admin) { _ in
                    
                }
            }).disposed(by: cell.disposeBag)
        } else if rowType == .showMute {
            cell.accessoryType = .disclosureIndicator
            cell.subtitleLabel.textAlignment = .right
            cell.subtitleLabel.text = (_viewModel.memberInfoRelay.value?.muteEndTime ?? 0) > Date().timeIntervalSince1970 ? "禁言中" : nil
        }
        
        return cell
    }
    
    func tableView(_: UITableView, didSelectRowAt indexPath: IndexPath) {
        let rowType: RowType = rowItems[indexPath.section][indexPath.row]
        switch rowType {
        case .profile:
            print("跳转个人资料页")
            let vc = ProfileTableViewController(userID: _viewModel.userId)
            navigationController?.pushViewController(vc, animated: true)
        case .showMute:
            let vc = MuteMemberTableViewController(viewModel: _viewModel)
            navigationController?.pushViewController(vc, animated: true)
        case .nickName: break
        default:
            break
        }
    }
    
    func tableView(_ tableView: UITableView, estimatedHeightForRowAt indexPath: IndexPath) -> CGFloat {
        return 40
    }
    
    func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        let rowType: RowType = rowItems[indexPath.section][indexPath.row]
        
        if rowType == .spacer {
            return 10
        }
        return UITableView.automaticDimension
    }
    
    func tableView(_ tableView: UITableView, heightForFooterInSection section: Int) -> CGFloat {
        .leastNormalMagnitude
    }
    
    func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        10
    }
    
    func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        UIView()
    }
    
    func tableView(_ tableView: UITableView, viewForFooterInSection section: Int) -> UIView? {
        UIView()
    }
    
    enum RowType {
        case identifier
        case nickName
        case organization
        case showSetAdmin
        case showMute
        case profile
        case spacer
        
        var title: String {
            switch self {
            case .identifier:
                return "ID号".innerLocalized()
            case .organization:
                return "组织信息".innerLocalized()
            case .nickName:
                return "群昵称".innerLocalized()
            case .showSetAdmin:
                return "设为管理员".innerLocalized()
            case .showMute:
                return "设置禁言".innerLocalized()
            case .profile:
                return "个人资料".innerLocalized()
            case .spacer:
                return ""
            }
        }
    }
}
