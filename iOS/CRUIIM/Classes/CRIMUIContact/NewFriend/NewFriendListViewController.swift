
import CRUICore
import CRUICoreView
import RxSwift

class NewFriendListViewController: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()
        navigationItem.title = "新的好友".innerLocalized()
        view.backgroundColor = .white
        replaceSystemBackBarButtonItem()
        
        initView()
        bindData()
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        _viewModel.getNewFriendReqs()
    }

    private lazy var tableView: UITableView = {
        let v = UITableView()
        v.register(NewFriendTableViewCell.self, forCellReuseIdentifier: NewFriendTableViewCell.className)
        v.delegate = self
        v.rowHeight = 68
        v.tableFooterView = UIView()
        v.backgroundColor = .clear
        
        if #available(iOS 15.0, *) {
            v.sectionHeaderTopPadding = 0
        }
        return v
    }()

    private func initView() {
        view.addSubview(tableView)
        tableView.snp.makeConstraints { make in
            make.top.equalTo(view.safeAreaLayoutGuide.snp.top).offset(0)
            make.leading.bottom.trailing.equalToSuperview()
        }
    }

    private func bindData() {
        _viewModel.newFriendReqs.asDriver(onErrorJustReturn: []).drive(tableView.rx.items) { tableView, _, item in
            let cell = tableView.dequeueReusableCell(withIdentifier: NewFriendTableViewCell.className) as! NewFriendTableViewCell
            cell.titleLabel.text = item.fromNickname
            cell.subtitleLabel.text = item.reqMsg
            if let state = NewFriendTableViewCell.ApplyState(rawValue: item.handleResult.rawValue) {
                cell.setApplyState(state)
            }

            cell.avatarView.setAvatar(url: item.fromFaceURL, text: nil, placeHolder: "contact_my_friend_icon")

            cell.agreeBtn.rx.tap.subscribe { [weak self] _ in

                let vc = ApplicationViewController(friendApplication: item)
                self?.navigationController?.pushViewController(vc, animated: true)
            }.disposed(by: cell.disposeBag)

            return cell
        }.disposed(by: _disposeBag)

        tableView.rx.modelSelected(FriendApplication.self).subscribe(onNext: { [weak self] (application: FriendApplication) in
            if let state = NewFriendTableViewCell.ApplyState(rawValue: application.handleResult.rawValue), state == .agreed {
                let vc = UserDetailTableViewController(userId: application.fromUserID, groupId: nil)
                self?.navigationController?.pushViewController(vc, animated: true)
            }
        }).disposed(by: _disposeBag)
    }

    private let _viewModel = NewFriendListViewModel()
    private let _disposeBag = DisposeBag()
}

extension NewFriendListViewController: UITableViewDelegate {
    
    func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        return 33
    }
    
    func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        let name = "新的好友请求"
        let header = ViewUtil.createSectionHeaderWith(text: name)
        header.backgroundColor = .white
        return header
    }
}
