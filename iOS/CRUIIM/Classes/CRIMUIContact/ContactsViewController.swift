
import RxSwift
import CRUICore

public class ContactsViewController: UITableViewController {
    public lazy var viewModel = ContactsViewModel()
    private let _disposeBag = DisposeBag()

    private lazy var newFriendCell: ContactsEntranceTableViewCell = {
        let v = getEntranceCell()
        let value = EntranceCellType.newFriend
        v.avatarImageView.image = value.iconImage
        v.titleLabel.text = value.title
        let tap = UITapGestureRecognizer()
        tap.rx.event.subscribe(onNext: { [weak self] _ in
            let vc = NewFriendListViewController()
            vc.hidesBottomBarWhenPushed = true
            self?.navigationController?.pushViewController(vc, animated: true)
        }).disposed(by: _disposeBag)
        v.addGestureRecognizer(tap)
        return v
    }()
    
    private lazy var _menuView: ChatMenuView = {
        let v = ChatMenuView()
        v.setOffsetY(-20)
        v.setItems(makeMenuItems())
        
        return v
    }()

    override public func viewDidLoad() {
        super.viewDidLoad()
        replaceSystemBackBarButtonItem()
        configureTableView()
        initView()
        bindData()
    }

    override public func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
    }

    private func initView() {
        tableView.backgroundColor = .viewBackgroundColor
        navigationItem.leftBarButtonItem = nil
        navigationItem.title = "通讯录".innerLocalized()

        let addItem: UIBarButtonItem = {
            let v = UIBarButtonItem()
            v.image = UIImage(nameInBundle: "contact_add_icon")?.withRenderingMode(.alwaysOriginal)
            v.imageInsets = UIEdgeInsets(top: 0, left: 0, bottom: 0, right: 15)
            v.rx.tap.subscribe(onNext: { [weak self] in
                guard let sself = self else { return }
                if sself._menuView.superview == nil, let window = sself.view.window {
                    sself._menuView.frame = window.bounds
                    window.addSubview(sself._menuView)
                } else {
                    sself._menuView.removeFromSuperview()
                }
            }).disposed(by: _disposeBag)
            return v
        }()
        
        navigationItem.rightBarButtonItems = [addItem]

        let vStack: UIStackView = {
            let rowHeight = 60
            let myFriendCell: ContactsEntranceTableViewCell = {
                let v = getEntranceCell()
                let value = EntranceCellType.myFriend
                v.avatarImageView.image = value.iconImage
                v.titleLabel.text = value.title
                v.badgeLabel.isHidden = true
                let tap = UITapGestureRecognizer()
                tap.rx.event.subscribe(onNext: { [weak self] _ in
                    let vc = FriendListViewController()
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                }).disposed(by: _disposeBag)
                v.addGestureRecognizer(tap)
                return v
            }()
            let myGroupCell: ContactsEntranceTableViewCell = {
                let v = getEntranceCell()
                let value = EntranceCellType.myGroup
                v.avatarImageView.image = value.iconImage
                v.titleLabel.text = value.title
                v.badgeLabel.isHidden = true
                let tap = UITapGestureRecognizer()
                tap.rx.event.subscribe(onNext: { [weak self] _ in
                    let vc = GroupListViewController()
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                }).disposed(by: _disposeBag)
                v.addGestureRecognizer(tap)
                return v
            }()
            
//            let spacer = UIView()
//            spacer.backgroundColor = .clear
            
            let arrangedViews: [UIView] = [newFriendCell, myFriendCell, myGroupCell]
            let v = UIStackView(arrangedSubviews: arrangedViews)
            v.axis = .vertical
            
            newFriendCell.snp.makeConstraints { make in
                make.height.equalTo(rowHeight)
            }
            myFriendCell.snp.makeConstraints { make in
                make.height.equalTo(rowHeight)
            }
            myGroupCell.snp.makeConstraints { make in
                make.height.equalTo(rowHeight)
            }
            
            v.bounds = CGRect(x: 0, y: 0, width: Int(kScreenWidth), height: rowHeight * 3)
            
            return v
        }()

        tableView.tableHeaderView = vStack
    }

    private func bindData() {
        viewModel.newFriendCountRelay.map { $0 == 0 }.bind(to: newFriendCell.badgeLabel.rx.isHidden).disposed(by: _disposeBag)
        viewModel.newFriendCountRelay.map { "\($0)" }.bind(to: newFriendCell.badgeLabel.rx.text).disposed(by: _disposeBag)
        viewModel.frequentContacts.asDriver().drive { [weak self] _ in
            self?.tableView.reloadData()
        }.disposed(by: _disposeBag)
        viewModel.companyDepartments.asDriver().drive { [weak self] _ in
            self?.tableView.reloadData()
        }.disposed(by: _disposeBag)
        viewModel.getFriendApplications()
        viewModel.getGroupApplications()
    }

    private func configureTableView() {
        tableView.backgroundColor = .systemGroupedBackground
        if #available(iOS 15.0, *) {
            tableView.sectionHeaderTopPadding = 0
        }
        tableView.rowHeight = 60
        tableView.separatorStyle = .none
        tableView.register(ContactsEntranceTableViewCell.self, forCellReuseIdentifier: ContactsEntranceTableViewCell.className)
        tableView.register(FrequentUserTableViewCell.self, forCellReuseIdentifier: FrequentUserTableViewCell.className)
    }

    private func getEntranceCell() -> ContactsEntranceTableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: ContactsEntranceTableViewCell.className) as! ContactsEntranceTableViewCell
        return cell
    }

    enum EntranceCellType: CaseIterable {
        case newFriend
        case myFriend
        case myGroup

        var iconImage: UIImage? {
            switch self {
            case .newFriend:
                return UIImage(nameInBundle: "contact_new_friend_icon")
            case .myFriend:
                return UIImage(nameInBundle: "contact_my_friend_icon")
            case .myGroup:
                return UIImage(nameInBundle: "contact_my_group_icon")
            }
        }

        var title: String {
            switch self {
            case .newFriend:
                return "新的好友".innerLocalized()
            case .myFriend:
                return "我的好友".innerLocalized()
            case .myGroup:
                return "我的群组".innerLocalized()
            }
        }
    }
}
