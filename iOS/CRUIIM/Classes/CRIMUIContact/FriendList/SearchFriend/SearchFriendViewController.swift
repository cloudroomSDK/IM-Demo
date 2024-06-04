
import RxSwift
import CRUICore
import CRUICoreView
import FDFullscreenPopGesture

extension SearchFriendViewController: UISearchControllerDelegate {
    func willPresentSearchController(_ searchController: UISearchController) {
        updateAuxiliaryViewConstraints(kStatusBarHeight + kNavigationBarHeight)
    }
    
    func willDismissSearchController(_ searchController: UISearchController) {
        updateAuxiliaryViewConstraints(kStatusBarHeight + kNavigationBarHeight + 52)
    }
}

class SearchFriendViewController: UIViewController, UITableViewDelegate, UITableViewDataSource  {
    
    private var _viewModel: UserProfileViewModel!
    var didSelectedItem: ((_ ID: String) -> Void)?
    
    private lazy var tableView: UITableView = {
        let v = UITableView()
        v.delegate = self
        v.dataSource = self
        v.isScrollEnabled = false
        return v
    }()
    
    private lazy var resultVC: SearchResultViewController = {
        let v = SearchResultViewController(searchType: .user)
        v.didSelectedItem = didSelectedItem
        return v
    }()
    
    private let auxiliaryView = UIView()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        replaceSystemBackBarButtonItem()
        fd_interactivePopDisabled = true
        view.backgroundColor = .white
        
        let searchVC: UISearchController = {
            let v = UISearchController(searchResultsController: resultVC)
            v.searchResultsUpdater = resultVC
            v.searchBar.placeholder = "搜索".innerLocalized()
            v.obscuresBackgroundDuringPresentation = false
            v.definesPresentationContext = true
            v.delegate = self
            return v
        }()
        navigationItem.searchController = searchVC
        
        initView()
        configureTableView()
        
        _viewModel = UserProfileViewModel(userId: IMController.shared.userID, groupId: nil)
    }
    
    open override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        getUserOrMemberInfo()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
    }
    
    public func getUserOrMemberInfo() {
        _viewModel.getUserOrMemberInfo()
    }
    
    private func initView() {
        view.addSubview(auxiliaryView)
        auxiliaryView.snp.makeConstraints { make in
            make.top.equalToSuperview().inset(kStatusBarHeight + kNavigationBarHeight + 52)
            make.left.right.equalToSuperview()
            make.height.equalTo(1)
        }
        
        view.addSubview(tableView)
        tableView.snp.makeConstraints { make in
            make.top.equalTo(auxiliaryView.snp.bottom)
            make.left.bottom.right.equalToSuperview()
        }
    }
    
    private func updateAuxiliaryViewConstraints(_ topConstraints: CGFloat) {
        self.auxiliaryView.snp.updateConstraints { make in
            make.top.equalToSuperview().inset(topConstraints)
        }
        UIView.animate(withDuration: 0.25) {
            self.view.layoutIfNeeded()
        }
    }
    
    private func configureTableView() {
        tableView.rowHeight = 74
        if #available(iOS 15.0, *) {
            tableView.sectionHeaderTopPadding = 0
        }
        tableView.separatorColor = .viewBackgroundColor
        tableView.separatorInset = UIEdgeInsets(top: 0, left: 71, bottom: 0, right: 22)
        tableView.separatorStyle = .none
        tableView.register(AddEntranceTableViewCell.self, forCellReuseIdentifier: AddEntranceTableViewCell.className)
    }
    
    private let rowTypes: [EntranceType] = [
        .mineQrcode,
        .scanQrcode,
    ]

     open func tableView(_: UITableView, numberOfRowsInSection section: Int) -> Int {
        rowTypes.count
    }

     open func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: AddEntranceTableViewCell.className) as! AddEntranceTableViewCell
        let item = rowTypes[indexPath.row]
        cell.avatarImageView.image = item.iconImage
        cell.titleLabel.text = item.title
        cell.subtitleLabel.text = item.subtitle
        return cell
    }

     open func tableView(_: UITableView, didSelectRowAt indexPath: IndexPath) {
        let item = rowTypes[indexPath.row]
        switch item {
        case .mineQrcode:
            guard let user = _viewModel.userInfoRelay.value else { return }
            let vc = QRCodeViewController(idString: IMController.addFriendPrefix.append(string: user.userID))
            vc.nameLabel.text = user.nickname
            vc.avatarView.setAvatar(url: user.faceURL, text: nil, placeHolder: "contact_my_friend_icon")
            vc.tipLabel.text = "扫一扫上面的二维码，添加为好友"
            navigationController?.pushViewController(vc, animated: true)
        case .scanQrcode:
            let vc = ScanViewController()
            vc.scanDidComplete = { [weak self] (result: String) in
                if result.contains(IMController.addFriendPrefix) {
                    let uid = result.replacingOccurrences(of: IMController.addFriendPrefix, with: "")
                    let vc = UserDetailTableViewController(userId: uid, groupId: nil)
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                    self?.dismiss(animated: true)
                } else if result.contains(IMController.joinGrpPrefix) {
                    let groupID = result.replacingOccurrences(of: IMController.joinGrpPrefix, with: "")
                    let vc = GroupDetailViewController(groupId: groupID, joinSource: .QRCode)
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                    self?.dismiss(animated: true)
                } else {
                    //ProgressHUD.showError(result)
                }
            }
            vc.modalPresentationStyle = .fullScreen
            present(vc, animated: true)
        }
    }

    struct SectionModel {
        let name: String
        let items: [EntranceType]
    }

    enum EntranceType {
        case mineQrcode
        case scanQrcode

        var iconImage: UIImage? {
            switch self {
            case .mineQrcode:
                return UIImage(nameInBundle: "add_mine_qrcode")
            case .scanQrcode:
                return UIImage(nameInBundle: "add_scan_icon")
            }
        }

        var title: String {
            switch self {
            case .mineQrcode:
                return "我的二维码".innerLocalized()
            case .scanQrcode:
                return "扫一扫".innerLocalized()
            }
        }

        var subtitle: String {
            switch self {
            case .mineQrcode:
                return "邀请对方扫码添加好友".innerLocalized()
            case .scanQrcode:
                return "扫描二维码名片".innerLocalized()
            }
        }
    }
}
