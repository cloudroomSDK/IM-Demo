
import CRUICore
import CRUICoreView
import ProgressHUD
import RxSwift

extension UIViewController {
    
    func makeMenuItems() -> [ChatMenuView.MenuItem] {
        let scanItem = ChatMenuView.MenuItem(title: "扫一扫".innerLocalized(), icon: UIImage(nameInBundle: "chat_menu_scan_icon")) { [weak self] in
            let vc = ScanViewController()
            vc.scanDidComplete = { [weak self] (result: String) in
                if result.contains(IMController.addFriendPrefix) {
                    let uid = result.replacingOccurrences(of: IMController.addFriendPrefix, with: "")
                    let vc = UserDetailTableViewController(userId: uid, groupId: nil)
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                    self?.dismiss(animated: false)
                } else if result.contains(IMController.joinGrpPrefix) {
                    let groupID = result.replacingOccurrences(of: IMController.joinGrpPrefix, with: "")
                    let vc = GroupDetailViewController(groupId: groupID, joinSource: .QRCode)
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                    self?.dismiss(animated: false)
                } else {
                    ProgressHUD.error(result)
                }
            }
            vc.modalPresentationStyle = .fullScreen
            self?.present(vc, animated: true, completion: nil)
        }
        let addFriendItem = ChatMenuView.MenuItem(title: "添加好友".innerLocalized(), icon: UIImage(nameInBundle: "chat_menu_add_friend_icon")) { [weak self] in
            let vc = SearchFriendViewController()
            vc.navigationItem.title = "添加好友".innerLocalized()
            vc.hidesBottomBarWhenPushed = true
            self?.navigationController?.pushViewController(vc, animated: true)
            vc.didSelectedItem = { [weak self] id in
                IMController.shared.checkFriendBy(userID: id).subscribe { [weak self] (result: Bool) in
                    if result {
                        let vc = UserDetailTableViewController(userId: id, groupId: nil)
                        self?.navigationController?.pushViewController(vc, animated: true)
                        
                    } else {
                        let viewModel = UserDetailViewModel(userId: id, groupId: nil, groupInfo: nil, userDetailFor: .card)
                        let vc = SendFriendReqViewController(viewModel: viewModel)
                        self?.navigationController?.pushViewController(vc, animated: true)
                    }
                }
            }
        }

        let addGroupItem = ChatMenuView.MenuItem(title: "加入群聊".innerLocalized(), icon: UIImage(nameInBundle: "chat_menu_add_group_icon")) { [weak self] in
            let vc = AddGroupTableViewController()
            vc.navigationItem.title = "加入群聊".innerLocalized()
            vc.hidesBottomBarWhenPushed = true
            self?.navigationController?.pushViewController(vc, animated: true)
        }
        
        let createWorkGroupItem = ChatMenuView.MenuItem(title: "创建群聊".innerLocalized(), icon: UIImage(nameInBundle: "chat_menu_create_group_icon")) { [weak self] in

            let vc = SelectContactsViewController()
            vc.navigationItem.title = "创建群聊".innerLocalized()
            vc.selectedContact(blocked: [IMController.shared.userID]) { [weak self] (r: [ContactInfo]) in
                guard let sself = self else { return }
                let users = r.map {UserInfo(userID: $0.ID!, nickname: $0.name, faceURL: $0.faceURL)}
                let vc = NewGroupViewController(users: users)
                self?.navigationController?.pushViewController(vc, animated: true)
            }
            vc.hidesBottomBarWhenPushed = true
            self?.navigationController?.pushViewController(vc, animated: true)
        }
        let items = [scanItem, addFriendItem, addGroupItem, createWorkGroupItem]
        
        return items
    }
}

open class ChatListViewController: UIViewController, UITableViewDelegate {
    private lazy var _headerView: ChatListHeaderView = {
        let v = ChatListHeaderView()
        
        return v
    }()

    private lazy var _tableView: UITableView = {
        let v = UITableView()
        v.register(ChatTableViewCell.self, forCellReuseIdentifier: ChatTableViewCell.className)
        v.delegate = self
        v.separatorStyle = .none
        v.rowHeight = 68
        let refresh: UIRefreshControl = {
            let v = UIRefreshControl(frame: CGRect(x: 0, y: 0, width: 35, height: 35))
            v.rx.controlEvent(.valueChanged).subscribe(onNext: { [weak self, weak v] in
                self?._viewModel.getAllConversations()
                self?._viewModel.getSelfInfo()
                v?.endRefreshing()
            }).disposed(by: _disposeBag)
            return v
        }()
        v.refreshControl = refresh
        v.backgroundColor = .clear
        
        return v
    }()

    override open func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        navigationController?.setNavigationBarHidden(true, animated: false)
    }
    
    open override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        // 解决搜索框激活状态下右滑导航栏无法隐藏
        navigationController?.setNavigationBarHidden(true, animated: false)
    }

    override open func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        navigationController?.setNavigationBarHidden(false, animated: true)
    }

    private lazy var _menuView: ChatMenuView = {
        let v = ChatMenuView()
        v.setItems(makeMenuItems())
        
        return v
    }()

    func refreshConversations() {
        _viewModel.getAllConversations()
    }
    
    private let _disposeBag = DisposeBag()
    private let _viewModel = ChatListViewModel()

    override open func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .viewBackgroundColor
        
        initView()
        bindData()
    }

    private func initView() {
        view.addSubview(_headerView)
        _headerView.snp.makeConstraints { make in
            make.leading.top.trailing.equalToSuperview()
        }
        view.addSubview(_tableView)
        _tableView.snp.makeConstraints { make in
            make.top.equalTo(_headerView.snp.bottom).offset(16)
            make.leading.bottom.trailing.equalToSuperview()
        }
    }

    private func bindData() {
        
        IMController.shared.connectionRelay.subscribe(onNext: { [weak self] status in
            self?._headerView.updateConnectionStatus(status: status)
        })
        
        _headerView.addBtn.rx.tap.subscribe(onNext: { [weak self] in
            guard let sself = self else { return }
            if sself._menuView.superview == nil, let window = sself.view.window {
                sself._menuView.frame = window.bounds
                window.addSubview(sself._menuView)
            } else {
                sself._menuView.removeFromSuperview()
            }
        }).disposed(by: _disposeBag)

        _viewModel.conversationsRelay.asDriver(onErrorJustReturn: []).drive(_tableView.rx.items) { (tableView, _, item: ConversationInfo) in
            let cell = tableView.dequeueReusableCell(withIdentifier: ChatTableViewCell.className) as! ChatTableViewCell
            let placeholderName: String = item.conversationType == .c1v1 ? "contact_my_friend_icon" : "contact_group_setting_icon"
            let showName = item.showName?.isEmpty == true ? item.userID : item.showName
            cell.avatarImageView.setAvatar(url: item.faceURL, text: nil, placeHolder: placeholderName)
            cell.muteImageView.isHidden = item.recvMsgOpt == .receive
            
            cell.titleLabel.text = item.showName
            cell.contentView.backgroundColor = item.isPinned ? .cF0F0F0 : .tertiarySystemBackground
            if item.recvMsgOpt != .receive, item.unreadCount > 0 {
                let unread = "[\(item.unreadCount)条]"
            }
            cell.subtitleLabel.attributedText = MessageHelper.getAbstructOf(conversation: item)
            var unreadShouldHide: Bool = false
            if item.recvMsgOpt != .receive {
                unreadShouldHide = true
            }
            if item.unreadCount <= 0 {
                unreadShouldHide = true
            }
            cell.unreadLabel.isHidden = unreadShouldHide
            cell.unreadLabel.text = "\(item.unreadCount)"
            cell.muteImageView.isHidden = item.recvMsgOpt == .receive
            cell.timeLabel.text = MessageHelper.convertList(timestamp_ms: item.latestMsgSendTime)
            return cell
        }.disposed(by: _disposeBag)

        _tableView.rx.modelSelected(ConversationInfo.self).subscribe(onNext: { [weak self] (conversation: ConversationInfo) in
            
            let vc = ChatViewControllerBuilder().build(conversation)
            vc.hidesBottomBarWhenPushed = true
            self?.navigationController?.pushViewController(vc, animated: true)
        }).disposed(by: _disposeBag)

        _viewModel.loginUserPublish.subscribe(onNext: { [weak self] (userInfo: UserInfo?) in
            let nickname = userInfo?.nickname?.isEmpty == false ? userInfo?.nickname : userInfo?.userID
            self?._headerView.avatarImageView.setAvatar(url: userInfo?.faceURL, text: nil, placeHolder: "contact_my_friend_icon", onTap: nil)
            self?._headerView.nameLabel.text = userInfo?.nickname
            self?._headerView.statusLabel.titleLabel.text = "在线".innerLocalized()
            self?._headerView.statusLabel.statusView.backgroundColor = StandardUI.color_10CC64
        }).disposed(by: _disposeBag)
    }

    public func tableView(_: UITableView, trailingSwipeActionsConfigurationForRowAt indexPath: IndexPath) -> UISwipeActionsConfiguration? {
        let item = _viewModel.conversationsRelay.value[indexPath.row]
        
        var swipeActions = [UIContextualAction]()

        let pinActionTitle = item.isPinned ? "取消置顶".innerLocalized() : "置顶".innerLocalized()
        let setTopAction = UIContextualAction(style: .normal, title: pinActionTitle) { [weak self] _, _, completion in
            self?._viewModel.pinConversation(id: item.conversationID, isPinned: item.isPinned, onSuccess: { _ in
                completion(true)
            })
        }
        setTopAction.backgroundColor = UIColor.c0089FF
        swipeActions.append(setTopAction)
        
        let markReadTitle = "标记已读".innerLocalized()
        let markReadAction = UIContextualAction(style: .normal, title: markReadTitle) { [weak self] _, _, completion in
            self?._viewModel.markReaded(id: item.conversationID, onSuccess: { _ in
                completion(true)
            })
        }
        markReadAction.backgroundColor = UIColor.ffbd4c1
        
        let hideTitle = "隐藏".innerLocalized()
        let hideAction = UIContextualAction(style: .normal, title: hideTitle) { [weak self] _, _, completion in
            self?._viewModel.hideConversation(conversationID: item.conversationID, onSuccess: { _ in
                completion(true)
            })
        }
        markReadAction.backgroundColor = UIColor.ffbd4c1
        
        if item.unreadCount > 0 {
            swipeActions.append(markReadAction)
        } else {
            swipeActions.append(hideAction)
        }

        let deleteAction = UIContextualAction(style: .destructive, title: "移除".innerLocalized()) { [weak self] _, _, completion in
            self?._viewModel.deleteConversation(conversationID: item.conversationID, completion: { _ in
                completion(true)
            })
        }
        deleteAction.backgroundColor = UIColor.cFF381F
        swipeActions.append(deleteAction)
        
        let configure = UISwipeActionsConfiguration(actions: swipeActions)
        return configure
    }
}
