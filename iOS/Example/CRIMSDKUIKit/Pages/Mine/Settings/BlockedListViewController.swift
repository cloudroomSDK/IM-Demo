
import RxSwift
import CRUICore

open class BlockedListViewController: UIViewController {

    private lazy var _tableView: UITableView = {
        let v = UITableView()
        v.register(FriendListUserTableViewCell.self, forCellReuseIdentifier: FriendListUserTableViewCell.className)
        v.delegate = self
        v.rowHeight = UITableView.automaticDimension
        v.backgroundColor = .viewBackgroundColor
        
        if #available(iOS 15.0, *) {
            v.sectionHeaderTopPadding = 0
        }
        return v
    }()
    
    private let emptyImageView = UIImageView(image: UIImage(named: "mine_blocked_list_empty"))
    
    private let emptyLabel: UILabel = {
        let v = UILabel()
        v.text = "你将不再接受黑名单用户的任何消息"
        v.font = .f17
        v.textColor = .c3D3D3D
        v.textAlignment = .center
        
        return v
    }()
    private let _viewModel = SettingViewModel()
    private let _disposeBag = DisposeBag()

    override open func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        navigationItem.hidesSearchBarWhenScrolling = false
    }

    override open func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        navigationItem.hidesSearchBarWhenScrolling = true
    }

    override open func viewDidLoad() {
        super.viewDidLoad()
        navigationItem.title = "通讯录黑名单".innerLocalized()
        view.backgroundColor = .viewBackgroundColor
        replaceSystemBackBarButtonItem()
        
        initView()
        bindData()
        _viewModel.getBlockedList()
    }

    private func initView() {

        let vStack = UIStackView(arrangedSubviews: [emptyImageView, emptyLabel])
        vStack.axis = .vertical
        vStack.alignment = .center
        vStack.spacing = 8
        
        view.addSubview(vStack)
        vStack.snp.makeConstraints { make in
            make.centerX.equalToSuperview()
            make.centerY.equalToSuperview().offset(-80)
        }
        
        view.addSubview(_tableView)
        _tableView.snp.makeConstraints { make in
            make.top.equalTo(view.safeAreaLayoutGuide.snp.top).offset(8)
            make.leading.trailing.bottom.equalToSuperview()
        }
    }

    private func bindData() {
        _viewModel.blockedList.subscribe(onNext: { [weak self] infos in
            self?._tableView.isHidden = infos.isEmpty
        }).disposed(by: _disposeBag)
        
        _viewModel.blockedList.asDriver(onErrorJustReturn: []).drive(_tableView.rx.items) { tableView, _, item in
            let cell = tableView.dequeueReusableCell(withIdentifier: FriendListUserTableViewCell.className) as! FriendListUserTableViewCell
            cell.titleLabel.text = item.nickname
            cell.avatarImageView.setAvatar(url: item.faceURL, text: item.nickname, onTap: nil)
            return cell
        }.disposed(by: _disposeBag)
        
//        _tableView.rx.modelSelected(BlackInfo.self).subscribe(onNext: { [weak self] (blackInfo: BlackInfo) in
//            IMController.shared.getConversation(sessionType: .c1v1, sourceId: blackInfo.userID!) { [weak self] (conversation: ConversationInfo?) in
//                guard let conversation = conversation else { return }
//                let viewModel = MessageListViewModel(userId: blackInfo.userID, conversation: conversation)
//                let controller = MessageListViewController(viewModel: viewModel)
//                controller.hidesBottomBarWhenPushed = true
//                self?.navigationController?.pushViewController(controller, animated: true)
//            }
//        }).disposed(by: _disposeBag)
    }
    
    deinit {
        print("dealloc \(type(of: self))")
    }
}

extension BlockedListViewController: UITableViewDelegate {

    public func tableView(_ tableView: UITableView, trailingSwipeActionsConfigurationForRowAt indexPath: IndexPath) -> UISwipeActionsConfiguration? {
        return .init(actions: [.init(style: .destructive, title: "移除黑名单".innerLocalized(), handler: {[weak self] (action, view, complete) in
            let user: BlackInfo = (self?._viewModel.blockedList.value[indexPath.row])!
            self?._viewModel.removeFromBlockedList(uid: user.userID!) { r in
                complete(true)
            }
        })])
    }
    
    public func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        let name = _viewModel.blockedList.value.count > 0 ? "你将不再接受黑名单用户的任何消息" : ""
        let header = ViewUtil.createSectionHeaderWith(text: name)
        header.backgroundColor = .viewBackgroundColor
        return header
    }
    
    public func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return _viewModel.blockedList.value.count > 0 ? 33 : CGFloat.leastNormalMagnitude
    }

    public func tableView(_: UITableView, heightForFooterInSection _: Int) -> CGFloat {
        return CGFloat.leastNormalMagnitude
    }
}
