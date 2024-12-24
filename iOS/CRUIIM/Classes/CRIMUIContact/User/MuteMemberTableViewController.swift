
import Photos
import RxSwift
import ProgressHUD
import CRUICore

class MuteMemberTableViewController: UITableViewController {
    
    public init(viewModel: UserDetailViewModel) {
        self._viewModel = viewModel
        super.init(style: .plain)
    }
    
    public func getUserOrMemberInfo() {
        
    }
    
    private let _viewModel: UserDetailViewModel
    
    required public init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    private var currentRowType: RowType = .undefine
    private lazy var rowItems: [[RowType]] = [[.tenMinu, .anHour, .halfDay, .oneDay, .cancelMute], [.custom]]
    
    private var muteDayStr: String = ""

    private let _disposeBag = DisposeBag()

    open override func viewDidLoad() {
        super.viewDidLoad()
        navigationItem.title = "设置禁言".innerLocalized()
        replaceSystemBackBarButtonItem()
        
        configureTableView()
        bindData()
    }
    
    open override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        getUserOrMemberInfo()
    }

    private func configureTableView() {
        tableView.register(OptionTableViewCell.self, forCellReuseIdentifier: OptionTableViewCell.className)
        tableView.tableFooterView = UIView()
        tableView.separatorStyle = .singleLine
        tableView.backgroundColor = .viewBackgroundColor
        if #available(iOS 15.0, *) {
            tableView.sectionHeaderTopPadding = 0
        }
    }

    private func bindData() {
        
    }
    
    private func setMutedSeconds(_ seconds: Int) {
        _viewModel.setMutedSeconds(seconds: seconds) { _ in
            
        }
    }
    
    @objc func textFieldDidChanged(textField: UITextField) {
        muteDayStr = textField.text ?? ""
    }

    open override func numberOfSections(in tableView: UITableView) -> Int {
        rowItems.count
    }
    
    open override func tableView(_: UITableView, numberOfRowsInSection section: Int) -> Int {
        rowItems[section].count
    }

    public override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let rowType = rowItems[indexPath.section][indexPath.row]
        
        let cell = tableView.dequeueReusableCell(withIdentifier: OptionTableViewCell.className, for: indexPath) as! OptionTableViewCell
        cell.titleLabel.text = rowType.title
        cell.titleLabel.textColor = cell.subtitleLabel.textColor
        cell.accessoryType = .none
        cell.accessoryView = rowType == currentRowType && rowType != .custom ? UIImageView(image: UIImage(nameInBundle: "common_check_selected")) : nil
        
        if currentRowType == .custom && rowType == currentRowType {
            let muteDay = Int(muteDayStr) ?? 0
            cell.subtitleLabel.text = muteDay > 0 ? muteDayStr + "天" : nil
        }
        
        return cell
    }

    public override func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
       60
    }
    
    public override func tableView(_ tableView: UITableView, heightForFooterInSection section: Int) -> CGFloat {
        .leastNormalMagnitude
    }
    
    public override func tableView(_ tableView: UITableView, heightForHeaderInSection section: Int) -> CGFloat {
        16
    }
    
    public override func tableView(_ tableView: UITableView, viewForHeaderInSection section: Int) -> UIView? {
        UIView()
    }
    
    public override func tableView(_ tableView: UITableView, viewForFooterInSection section: Int) -> UIView? {
        UIView()
    }

    open override func tableView(_: UITableView, didSelectRowAt indexPath: IndexPath) {
        let rowType = rowItems[indexPath.section][indexPath.row]
        currentRowType = rowType
        
        switch rowType {
        case .custom:
            let alertController = SPAlertController.alertController(withTitle: nil, message: nil, preferredStyle: .alert)
            alertController.messageColor = .c353535
            let action1 = SPAlertAction.action(withTitle: "取消".innerLocalized(), style: .default) { [weak self] (action) in
                guard let self else { return }
                self.tableView.reloadData()
            }
            let action2 = SPAlertAction.action(withTitle: "确定".innerLocalized(), style: .default) { [weak self] (action) in
                guard let self else { return }
                self.tableView.reloadData()
                let seconds = Int(self.muteDayStr) ?? 0
                self.setMutedSeconds(seconds*60*60*24)
            }
            action1.titleColor = .c353535
            action2.titleColor = .c0584FE
            alertController.addTextFieldWithConfigurationHandler { [weak self] textField in
                guard let `self` else {
                    return
                }
                
                textField.placeholder = "请输入禁言天数".innerLocalized()
                textField.keyboardType = .numberPad
                textField.addTarget(self, action: #selector(self.textFieldDidChanged(textField:)), for: .editingChanged)
                
                let maxTextLength = 4
                // 监听UITextView的文本变化
                textField.rx.text.orEmpty
                    .map { String($0.prefix(maxTextLength)) } // 限制输入文本的长度
                    .bind(to: textField.rx.text) // 将限制后的文本绑定回UITextView
                    .disposed(by: _disposeBag)
            }
            alertController.addAction(action: action1)
            alertController.addAction(action: action2)
            present(alertController, animated: true, completion: nil)
        default:
            setMutedSeconds(rowType.seconds)
        }
        tableView.reloadData()
    }

    deinit {
        #if DEBUG
            print("dealloc \(type(of: self))")
        #endif
    }

    enum RowType: CaseIterable {
        case undefine
        case tenMinu
        case anHour
        case halfDay
        case oneDay
        case cancelMute
        case custom
        case spacer
        

        var title: String {
            switch self {
            case .tenMinu:
                return "10分钟".innerLocalized()
            case .anHour:
                return "1小时".innerLocalized()
            case .halfDay:
                return "12小时".innerLocalized()
            case .oneDay:
                return "1天".innerLocalized()
            case .cancelMute:
                return "取消禁言".innerLocalized()
            case .custom:
                return "自定义".innerLocalized()
            case .spacer, .undefine:
                return ""
            }
        }
        
        var seconds: Int {
            switch self {
            case .tenMinu:
                return 60*10
            case .anHour:
                return 60*60
            case .halfDay:
                return 60*60*12
            case .oneDay:
                return 60*60*24
            case .cancelMute:
                return 0
            case .custom:
                return 0
            case .spacer, .undefine:
                return 0
            }
        }
    }
}

