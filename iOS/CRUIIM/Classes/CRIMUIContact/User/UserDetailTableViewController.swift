
import CRUICore
import ProgressHUD
import RxSwift

enum UserDetailFor {
    case groupMemberInfo
    case card
}

class SpacerCell: UITableViewCell {
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        backgroundColor = .clear
        contentView.backgroundColor = UIColor.cF8F9FA
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class UserDetailTableViewController: UIViewController {
    private let avatarView = AvatarView()
    
    private let nameLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f17
        v.textColor = UIColor.c0C1C33
        
        return v
    }()
    
    private let sendMsgLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f17
        v.textColor = UIColor.c0C1C33
        v.textAlignment = .center
        v.text = "发信息".innerLocalized()
        
        return v
    }()
    
    private lazy var sendMsgBtn: UIButton = {
        let v = UIButton(type: .custom)
        v.setImage(UIImage(nameInBundle: "profile_message_button_icon"), for: .normal)
        
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
    
    private lazy var sendMsgStack: UIStackView = {
        let v = UIStackView(arrangedSubviews: [sendMsgBtn, sendMsgLabel])
        v.axis = .vertical
        v.distribution = .equalCentering
        v.spacing = 4
        
        return v
    }()
    
    private let unfriendLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f17
        v.textColor = UIColor.cFF381F
        v.textAlignment = .center
        v.text = "解除好友关系".innerLocalized()
        
        return v
    }()
    
    private lazy var unfriendBtn: UIButton = {
        let v = UIButton(type: .custom)
        v.setImage(UIImage(nameInBundle: "profile_unfriend_button_icon"), for: .normal)
        
        v.rx.tap.subscribe(onNext: { [weak self] _ in
            self?.deleteFriend()
        }).disposed(by: _disposeBag)
        return v
    }()
    
    private lazy var unfriendStack: UIStackView = {
        let v = UIStackView(arrangedSubviews: [unfriendBtn, unfriendLabel])
        v.axis = .vertical
        v.distribution = .equalCentering
        v.spacing = 4
        
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
    private var user: FullUserInfo?
    private var strangerInfo: UserInfo?
    private var inBlackList = false
    
    private lazy var _tableView: UITableView = {
        let v = UITableView(frame: .zero, style: .plain)
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
            
            addFriendBtn.setContentHuggingPriority(.required, for: .horizontal)
            addFriendBtn.setContentCompressionResistancePriority(.required, for: .horizontal)
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
        v.register(IdentifierCopyCell.self, forCellReuseIdentifier: IdentifierCopyCell.className)
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
    
    private var rowItems: [[RowType]] = [[.identifier]]
    
    init(userId: String, groupId: String? = nil, groupInfo: GroupInfo? = nil, userDetailFor: UserDetailFor = .card) {
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
            
            var v: UIStackView = UIStackView(arrangedSubviews: [sendMsgStack, unfriendStack])
            
            if let groupInfo = _viewModel.groupInfo, groupInfo.applyMemberFriend != 0 {
                v = UIStackView(arrangedSubviews: [sendMsgStack])
            }
            v.spacing = 16
            v.distribution = .fillEqually
            v.alignment = .center
            
            return v
        }()
        
        sendMsgBtn.snp.makeConstraints { make in
            make.height.equalTo(60)
        }
        if let superview = unfriendBtn.superview {
            unfriendBtn.snp.makeConstraints { make in
                make.height.equalTo(sendMsgBtn)
            }
        }
        
        let spacer = UIView()

        let vStack: UIStackView = {
            let v = UIStackView(arrangedSubviews: [spacer, hStack])
            v.frame =  CGRect(x: 0, y: 0, width: kScreenWidth, height: 134)
            v.axis = .vertical
            v.distribution = .equalCentering
            v.spacing = 4
            
            return v
        }()
        _tableView.tableFooterView = vStack
    }
    
    @objc private func rightButtonAction() {
        let vc = UserProfileTableViewController(userId: self._viewModel.userId)
        navigationController?.pushViewController(vc, animated: true)
    }
    
    private func bindData() {
        _viewModel.userInfoRelay.subscribe(onNext: { [weak self] (userInfo: FullUserInfo?) in
            guard let userInfo, let sself = self else {
                self?.addFriendBtn.isHidden = true
                self?.sendMsgStack.isHidden = true
                self?.unfriendStack.isHidden = true
                return
            }
            
            sself.avatarView.setAvatar(url: userInfo.faceURL, text: userInfo.showName) { [weak self] in
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
            sself.user = userInfo

            if userInfo.userID == IMController.shared.userID {
                sself.addFriendBtn.isHidden = true
                sself.sendMsgStack.isHidden = true
                sself.unfriendStack.isHidden = true
            } else if userInfo.friendInfo != nil, sself.rowItems.count < 3 {
                //是好友可以查看详细
                sself.addFriendBtn.isHidden = true
                sself.sendMsgStack.isHidden = false
                sself.unfriendStack.isHidden = false
                
                sself.rowItems.append([.remark, .gender, .birthday, .phone])
                sself.rowItems.append([.blocked])
                
                sself.navigationItem.title = "好友名片"
            }
            sself._tableView.reloadData()
        }).disposed(by: _disposeBag)
        
        _viewModel.strangerInfoRelay.subscribe(onNext: { [weak self] (userInfo: UserInfo?) in
            guard let userInfo, let sself = self else {
                return
            }
            
            // 陌生人
            self?.sendMsgStack.isHidden = true
            self?.unfriendStack.isHidden = true
            
            sself.avatarView.setAvatar(url: userInfo.faceURL, text: userInfo.nickname) { [weak self] in
                guard let self else { return }
                let vc = UserProfileTableViewController.init(userId: self._viewModel.userId, groupId: self._viewModel.groupId)
                self.navigationController?.pushViewController(vc, animated: true)
            }
            var name: String? = userInfo.nickname
            sself.nameLabel.text = name
            sself.addFriendBtn.isHidden = false
            sself.strangerInfo = userInfo

            if userInfo.userID == IMController.shared.userID {
                sself.addFriendBtn.isHidden = true
            }
            sself._tableView.reloadData()
        }).disposed(by: _disposeBag)
        
        _viewModel.isInBlackListRelay.subscribe(onNext: {[weak self] isIn in
            self?.inBlackList = isIn
            self?._tableView.reloadData()
        }).disposed(by: _disposeBag)
    }
    
    @objc func blockedUser() {
        _viewModel.blockUser(blocked: !inBlackList) { r in
        }
    }
    
    @objc func deleteFriend() {
        ProgressHUD.show()
        _viewModel.deleteFriend {[weak self] r in
            ProgressHUD.dismiss()
            
            let navController = self?.tabBarController?.children.first as? UINavigationController;
            let vc: ChatListViewController? = navController?.viewControllers.first(where: { vc in
                return vc is ChatListViewController
            }) as? ChatListViewController
           
            if vc != nil {
                vc!.refreshConversations()
                self?.navigationController?.popToRootViewController(animated: true)
            }
        }
    }
    
    func modifyRemark() {
        let alertController = UIAlertController(title: "", message: "", preferredStyle: .alert)

        let saveAction = UIAlertAction(title: "保存".innerLocalized(), style: .default, handler: { [self] alert -> Void in
            let firstTextField = alertController.textFields![0] as UITextField
            
            if let remark = firstTextField.text?.trimmingCharacters(in: .whitespacesAndNewlines) {
                
                /*
                _viewModel.saveRemark(remark: remark) { r in
                    let index = rowItems.index(of: .remark)
                    let cell = _tableView.cellForRow(at: .init(row: index!, section: 0)) as! OptionTableViewCell
                    cell.subtitleLabel.text = remark
                }
                */
            }
        })
        let cancelAction = UIAlertAction(title: "取消".innerLocalized(), style: .default, handler: {
            (action : UIAlertAction!) -> Void in })
        
        alertController.addTextField { (textField : UITextField!) -> Void in
            textField.placeholder = "请输入备注".innerLocalized()
        }

        alertController.addAction(saveAction)
        alertController.addAction(cancelAction)

        self.present(alertController, animated: true, completion: nil)
    }
}

extension UserDetailTableViewController: UITableViewDataSource, UITableViewDelegate {
    func numberOfSections(in tableView: UITableView) -> Int {
        return rowItems.count
    }
    
    func tableView(_: UITableView, numberOfRowsInSection section: Int) -> Int {
        return rowItems[section].count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let rowType: RowType = rowItems[indexPath.section][indexPath.row]
        
        if rowType == .spacer {
            return tableView.dequeueReusableCell(withIdentifier: SpacerCell.className, for: indexPath)
        }
        
        if rowType == .identifier {
            let cell = tableView.dequeueReusableCell(withIdentifier: IdentifierCopyCell.className, for: indexPath) as! IdentifierCopyCell
            if let user = user, !(user.userID?.isEmpty ?? true) {
                cell.subtitleLabel.text = user.userID
            } else if let user = strangerInfo, !(user.userID?.isEmpty ?? true) {
                cell.subtitleLabel.text = user.userID
            }
            cell.accessoryType = .none
            cell.titleLabel.text = rowType.title
            cell.titleLabel.textColor = cell.subtitleLabel.textColor
            cell.copyBtn.isHidden = false
            
            cell.copyButtonTappedHandler = { [weak self, weak cell] in
                guard let userID = cell?.subtitleLabel.text else { return }
                UIPasteboard.general.string = userID
                ProgressHUD.showSuccess("ID复制成功")
            }
            
            return cell
        }
        
        let cell = tableView.dequeueReusableCell(withIdentifier: OptionTableViewCell.className, for: indexPath) as! OptionTableViewCell
        
        cell.accessoryType = .none
        cell.titleLabel.text = rowType.title
        cell.titleLabel.textColor = cell.subtitleLabel.textColor

        switch rowType {
        
        case .remark:
            cell.subtitleLabel.text = (user?.friendInfo?.remark?.isEmpty ?? true) ? user?.showName : user?.friendInfo?.remark
            cell.accessoryType = .disclosureIndicator
            
        case .gender:
            cell.subtitleLabel.text = user?.friendInfo?.gender.description
            cell.accessoryType = .none

        case .birthday:
            let birth = user?.friendInfo?.birth != nil ? ((user?.friendInfo?.birth)!) : 0
            cell.subtitleLabel.text = FormatUtil.getFormatDate(of: birth)
            cell.accessoryType = .none

        case .phone:
            cell.subtitleLabel.text = user?.friendInfo?.phoneNumber
            cell.accessoryType = .none
            
        case .blocked:
            cell.switcher.isHidden = false
            cell.switcher.isOn = inBlackList
            cell.switcher.addTarget(self, action: #selector(blockedUser), for: .valueChanged)
  
        default:
            break
        }
        
        /*
        guard let info = _viewModel.memberInfoRelay.value else { return cell }
        cell.titleLabel.text = rowType.title
        
        if rowType == .nickName {
            cell.subtitleLabel.text = info.nickname
        } else if rowType == .joinTime {
            cell.subtitleLabel.text = FormatUtil.getFormatDate(formatString: "yyyy年MM月dd日", of: info.joinTime/1000)
        } else if rowType == .joinSource {
            cell.subtitleLabel.text = info.joinWay
        }
        */
                
        return cell
    }
    
    func tableView(_: UITableView, didSelectRowAt indexPath: IndexPath) {
        let rowType: RowType = rowItems[indexPath.section][indexPath.row]
        switch rowType {
        case .profile:
            print("跳转个人资料页")
            let vc = ProfileTableViewController(userID: _viewModel.userId)
            navigationController?.pushViewController(vc, animated: true)
        case .nickName, .joinTime, .joinSource: break
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
    
    /*
    // 设置分割线
    func tableView(_ tableView: UITableView, willDisplay cell: UITableViewCell, forRowAt indexPath: IndexPath) {
        // 获取当前组的行数
        let rowCount = tableView.numberOfRows(inSection: indexPath.section)
        
        // 隐藏第一行和最后一行的分割线
        cell.separatorInset = UIEdgeInsets(top: 0, left: 0, bottom: 0, right: tableView.bounds.width)
        if indexPath.row == 0 {
            cell.separatorInset = UIEdgeInsets(top: 0, left: 0, bottom: 0, right: tableView.bounds.width)
        } else if indexPath.row == rowCount - 1 {
            cell.separatorInset = UIEdgeInsets(top: 0, left: 0, bottom: 0, right: tableView.bounds.width)
        }
    }
    */
    
    enum RowType {
        case nickName
        case joinTime
        case joinSource
        case profile
        case spacer
        case identifier
        case remark
        case gender
        case birthday
        case phone
        case blocked
        
        var title: String {
            switch self {
            case .nickName:
                return "群昵称".innerLocalized()
            case .joinTime:
                return "入群时间".innerLocalized()
            case .joinSource:
                return "入群方式".innerLocalized()
            case .profile:
                return "个人资料".innerLocalized()
            case .identifier:
                return "ID号".innerLocalized()
            case .remark:
                return "备注".innerLocalized()
            case .gender:
                return "性别".innerLocalized()
            case .birthday:
                return "生日".innerLocalized()
            case .phone:
                return "手机号码".innerLocalized()
            case .blocked:
                return "加入黑名单".innerLocalized()
            case .spacer:
                return ""
            }
        }
    }
}
