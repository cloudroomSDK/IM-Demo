
import CRUICore
import RxSwift
import ProgressHUD

class SettingTableViewController: UITableViewController {
    let _disposeBag = DisposeBag()
    
    private let _viewModel = SettingViewModel()
    private let _mineViewModel = MineViewModel()
    private let rowItems: [[RowType]] = [
        [.blocked, .clearHistory, .destroy]
    ]
    
    init() {
        super.init(style: .plain)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        navigationItem.title = "账号设置".innerLocalized()
        replaceSystemBackBarButtonItem()
        configureTableView()
        bindData()
        initView()
        _viewModel.getSettingInfo()
    }

    private func configureTableView() {
        
        tableView.register(OptionTableViewCell.self, forCellReuseIdentifier: OptionTableViewCell.className)
        tableView.register(SwitchTableViewCell.self, forCellReuseIdentifier: SwitchTableViewCell.className)
        tableView.register(UITableViewCell.self, forCellReuseIdentifier: UITableViewCell.className)
        tableView.tableFooterView = UIView()
        if #available(iOS 15.0, *) {
            tableView.sectionHeaderTopPadding = 0
        }
    }

    private func initView() {
        view.backgroundColor = .viewBackgroundColor
    }
    
    private func bindData() {
    }
    
    private func destroyAccount() {
        let account = "admin1"
        let password = "e00cf25ad42683b3df678c61f42c6bda"
        AccountViewModel.loginAdmin(account: account, psw: password) { errCode, errMsg in
            if errCode == 0 {
                AccountViewModel.blockAdd(userID: IMController.shared.userID, adminToken: AccountViewModel.adminToken) { [weak self] errCode, errMsg in
                    if errCode == 0 {
                        self?._mineViewModel.logout()
                    }
                }
            }
        }
    }
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return rowItems.count
    }

    override func tableView(_: UITableView, numberOfRowsInSection section: Int) -> Int {
        return rowItems[section].count
    }

    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let rowType: RowType = rowItems[indexPath.section][indexPath.row]
        let cell = tableView.dequeueReusableCell(withIdentifier: OptionTableViewCell.className) as! OptionTableViewCell
        
        switch rowType {
        case .blocked:
            cell.titleLabel.text = rowType.title
        case .clearHistory:
            cell.titleLabel.text = rowType.title
            cell.titleLabel.textColor = .cFF381F
        case .destroy:
            cell.titleLabel.text = rowType.title
            cell.titleLabel.textColor = .cFF381F
        }
        
        return cell
    }
    
    override func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        60
    }
    
    override func tableView(_ tableView: UITableView, heightForFooterInSection section: Int) -> CGFloat {
        .leastNormalMagnitude
    }
    
    override func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        16
    }
    
    override func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        UIView()
    }
    
    override func tableView(_ tableView: UITableView, viewForFooterInSection section: Int) -> UIView? {
        UIView()
    }

    override func tableView(_: UITableView, didSelectRowAt indexPath: IndexPath) {
        let rowType: RowType = rowItems[indexPath.section][indexPath.row]
        switch rowType {
        case .blocked:
            let vc = BlockedListViewController()
            navigationController?.pushViewController(vc, animated: true)
        case .clearHistory:
            presentAlert(title: "您确定要清空聊天记录吗？".innerLocalized()) {
                ProgressHUD.animate("请等待...".innerLocalized())
                self._viewModel.clearHistory { res in
                    ProgressHUD.success("清空完成".innerLocalized())
                }
            }
        case .destroy:
            let alertController = SPAlertController.alertController(withTitle: "注销账号", message: "您确定要注销账号吗？注销账号之后你将永久不可登录和使用账号，聊天消息和好友关系将被清空".innerLocalized(), preferredStyle: .alert)
            alertController.messageColor = DemoUI.color_353535
            let action1 = SPAlertAction.action(withTitle: "取消".innerLocalized(), style: .default) { (action) in
            }
            let action2 = SPAlertAction.action(withTitle: "确定".innerLocalized(), style: .default) { [weak self] (action) in
                self?.destroyAccount()
            }
            action1.titleColor = DemoUI.color_353535
            action2.titleColor = DemoUI.color_0584FE
            alertController.addAction(action: action1)
            alertController.addAction(action: action2)
            present(alertController, animated: true, completion: nil)
        }
    }

    enum RowType: CaseIterable {
        case blocked
        case clearHistory
        case destroy
        
        var title: String {
            switch self {
            case .blocked:
                return "通讯录黑名单".innerLocalized()
            case .clearHistory:
                return "清空聊天记录".innerLocalized()
            case .destroy:
                return "注销账号".innerLocalized()
            }
        
        }
    }
}
