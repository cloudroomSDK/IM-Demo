
import Foundation
import UIKit
import RxSwift
import SnapKit
import CRUICore
import ProgressHUD
import CRUICalling

let kAppIDDefaultShow = "默认appID"
let kDefaultServerShow = "默认"

class ConfigCell: UITableViewCell, UITextFieldDelegate {
    let disposeBag = DisposeBag()
    
    let titleLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.systemFont(ofSize: 16)
        v.textColor = DemoUI.color_0C1C33
        v.textAlignment = .right
        return v
    }()
    
    var inputTextFiled: UITextField = {
        let v = UITextField()
        v.font = UIFont.systemFont(ofSize: 16)
        v.textColor = DemoUI.color_0C1C33
        v.clearButtonMode = .whileEditing
        
        return v
    }()
    
    private var titleRightToSuperViewConstraint: Constraint!
    
    var onTextChanged: ((String?) -> Void)?
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        backgroundColor = .white
        
        contentView.addSubview(titleLabel)
        titleLabel.snp.makeConstraints { make in
            make.left.equalToSuperview().offset(15)
            make.width.equalTo(60).priority(999)
            make.centerY.equalToSuperview()
            
            self.titleRightToSuperViewConstraint = make.right.equalToSuperview().offset(-15).priority(999).constraint
            self.titleRightToSuperViewConstraint.isActive = false
        }
        
        contentView.addSubview(inputTextFiled)
        inputTextFiled.snp.makeConstraints { make in
            make.left.equalTo(titleLabel.snp.right).offset(8)
            make.right.equalToSuperview().offset(-15)
            make.top.equalToSuperview().offset(3)
            make.bottom.equalToSuperview().offset(-3)
        }
        
        inputTextFiled.delegate = self
        inputTextFiled.rx.controlEvent(.editingChanged)
            .map { [weak inputTextFiled] _ -> String in
                // 实时去空格
                return inputTextFiled?.text?.replacingOccurrences(of: " ", with: "") ?? ""
            }
            .do(onNext: { [weak self] filterText in
                // 把去空格后的文本回调出去
                self?.onTextChanged?(filterText)
            })
            .bind(to: inputTextFiled.rx.text)
            .disposed(by: disposeBag)
    }
    
    func updateInputVisible(_ isVisible: Bool) {
        inputTextFiled.isHidden = !isVisible
        titleRightToSuperViewConstraint.isActive = !isVisible
        inputTextFiled.isUserInteractionEnabled = isVisible
        contentView.layoutIfNeeded()
    }
    
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
            return !string.contains(" ")
        }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

let bussinessSeverAddrKey = "com.crimuikit.bussiness.api.adr"
let adminSeverAddrKey = "com.criuikit.admin.adr"
let skipVerifyCertKey = "com.crimuikit.skip.verify.cert"

class ConfigViewController: UIViewController {

    let disposeBag = DisposeBag()
    
    let rowHeight = 54
        
    private var bussinessSeverAddr = UserDefaults.standard.string(forKey: bussinessSeverAddrKey) ?? AppConfig.defaultServerAddr
    private var skipVerifyCert = UserDefaults.standard.bool(forKey: skipVerifyCertKey)
    private var showSkipVerifyCert = false

    let list: UITableView = {
        let t = UITableView.init()
        t.backgroundColor = .viewBackgroundColor
        t.register(ConfigCell.self, forCellReuseIdentifier: ConfigCell.className)
        t.register(OptionTableViewCell.self, forCellReuseIdentifier: OptionTableViewCell.className)
        return t
    }()
    
    private lazy var bussinessServerCell: ConfigCell = {
        let v = getConfigCell()
        let value = ConfigCellType.bussinessSever
        v.titleLabel.text = value.title
        v.titleLabel.textAlignment = .natural
        v.titleLabel.textColor = DemoUI.color_666666
        v.inputTextFiled.text = makeServerAddressDisplay(bussinessSeverAddr)
        v.inputTextFiled.placeholder = "http://xxx.xxx"
        v.updateInputVisible(true)
        
        let line = UIView()
        line.backgroundColor = DemoUI.color_E0E0E0
        v.contentView.addSubview(line)
        line.snp.makeConstraints { make in
            make.left.equalToSuperview().offset(15)
            make.bottom.right.equalToSuperview()
            make.height.equalTo(1)
        }
        return v
    }()
    
    private lazy var skipVerifyCell: ConfigCell = {
        let v = getConfigCell()
        let value = ConfigCellType.skipVerifyCert
        v.titleLabel.text = value.title
        v.titleLabel.textAlignment = .natural
        v.titleLabel.textColor = DemoUI.color_666666
        v.inputTextFiled.isHidden = true
        v.updateInputVisible(!v.inputTextFiled.isHidden)
        
        return v
    }()
    
    private lazy var skipVerifySwitch: UISwitch = {
        let v = UISwitch()
        
        return v
    }()
    
    private let vStack: UIStackView = {
        let v = UIStackView()
        v.axis = .vertical
        return v
    }()
    
    lazy var saveButton: UIButton = {
        let t = UIButton(type: .custom)
        t.setTitle("保存", for: .normal)
        t.setTitleColor(DemoUI.color_3981FC, for: .normal)
        t.titleLabel?.font = .systemFont(ofSize: 17)
        t.backgroundColor = .white
        
        t.rx.tap.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            sself.saveAddress()
            sself.navigationController?.popViewController(animated: true)
        }).disposed(by: disposeBag)
        return t
    }()
    
    lazy var resetButton: UIButton = {
        let t = UIButton(type: .custom)
        t.setTitle("重置为默认", for: .normal)
        t.setTitleColor(DemoUI.color_FF0000, for: .normal)
        t.titleLabel?.font = .systemFont(ofSize: 17)
        t.backgroundColor = .white
        
        t.rx.tap.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            sself.clearCacheConfig()
            sself.reloadRelatedRows()
            sself.saveAddress()
            ProgressHUD.succeed()
        }).disposed(by: disposeBag)
        return t
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .viewBackgroundColor
        navigationItem.title = "登录设置"
        replaceSystemBackBarButtonItem()
        
        view.addSubview(list)
        list.snp.makeConstraints { make in
            make.top.equalTo(view.safeAreaLayoutGuide.snp.top).offset(16)
            make.leading.bottom.trailing.equalToSuperview()
        }
        
        showSkipVerifyCert = bussinessSeverAddr.hasPrefix("https://")
        
        reloadRelatedRows()
        
        let footer = UIView()
        footer.frame = CGRect(x: 0, y: 0, width: Int(CGRectGetWidth(view.frame)), height: rowHeight*2 + 25 + 12)
        footer.addSubview(saveButton)
        saveButton.snp.makeConstraints { make in
            make.top.equalToSuperview().inset(25)
            make.left.trailing.equalToSuperview()
        }
        footer.addSubview(resetButton)
        resetButton.snp.makeConstraints { make in
            make.top.equalTo(saveButton.snp.bottom).offset(12)
            make.left.bottom.trailing.equalToSuperview()
            make.height.equalTo(saveButton)
        }
        list.tableFooterView = footer
        
        bussinessServerCell.onTextChanged = { [weak self] text in
            guard let inputText = text, !inputText.isEmpty else {
                self?.skipVerifyCert = false
                self?.skipVerifySwitch.isOn = false
                self?.showSkipVerifyCert = false
                UserDefaults.standard.setValue(false, forKey: skipVerifyCertKey)
                self?.reloadRelatedRows()
                return
            }
            
            let isHttps = inputText.hasPrefix("https://")
            
            self?.showSkipVerifyCert = isHttps
            self?.reloadRelatedRows()
        }
        
        skipVerifySwitch.rx.isOn
            .subscribe(onNext: { [weak self] isOn in
                self?.skipVerifyCert = isOn
                UserDefaults.standard.setValue(isOn, forKey: skipVerifyCertKey)
                CRIMSessionManagerWrapper.shared.updateCertificateValidation()
            })
            .disposed(by: disposeBag)
    }
    
    override open func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        navigationController?.setNavigationBarHidden(false, animated: false)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        navigationController?.setNavigationBarHidden(true, animated: false)
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        //saveAddress()
    }

    func reloadRelatedRows() {
        
        for value in vStack.arrangedSubviews {
            vStack.removeArrangedSubview(value)
            value.removeFromSuperview()
        }
        
        let spacer = UIView()
        spacer.backgroundColor = .viewBackgroundColor
        
        skipVerifySwitch.isOn = skipVerifyCert
        skipVerifyCell.accessoryView = skipVerifySwitch
        var arrangedViews = [bussinessServerCell]
        if showSkipVerifyCert {
            arrangedViews.append(skipVerifyCell)
        }
       
        for value in arrangedViews {
            vStack.addArrangedSubview(value)
        }
        
        bussinessServerCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        
        skipVerifyCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        
        vStack.bounds = CGRect(x: 0, y: 0, width: Int(kScreenWidth), height: rowHeight * arrangedViews.count + 16*(arrangedViews.count - 1))
        list.tableHeaderView = vStack
    }
    
    private func makeServerAddressDisplay(_ actualValue: String) -> String {
        // 如果是默认服务器地址，则显示"默认服务器"，否则显示实际值
        return actualValue == AppConfig.defaultServerAddr ? kDefaultServerShow : actualValue
    }
    
    @objc func clearCacheConfig() {
        self.view.endEditing(true)
        
        let ud = UserDefaults.standard
        ud.removeObject(forKey: bussinessSeverAddrKey)
                
        bussinessSeverAddr = UserDefaults.standard.string(forKey: bussinessSeverAddrKey) ?? AppConfig.defaultServerAddr
                
        bussinessServerCell.inputTextFiled.text = makeServerAddressDisplay(bussinessSeverAddr)

    }

    @objc func saveAddress() {

        self.view.endEditing(true)
        
        let inputText = bussinessServerCell.inputTextFiled.text
        bussinessSeverAddr = inputText == kDefaultServerShow ? AppConfig.defaultServerAddr : (inputText ?? AppConfig.defaultServerAddr)
        
        let ud = UserDefaults.standard
        ud.set(bussinessSeverAddr, forKey: bussinessSeverAddrKey)
        
        // 更新业务地址
        AccountViewModel.API_BASE_URL = bussinessSeverAddr
        
        ud.synchronize()
        AccountViewModel.saveUser(uid: AccountViewModel.userID, imToken: nil, chatToken: nil)
        
        CRIMSessionManagerWrapper.shared.updateCertificateValidation()
    }
    
    private func getConfigCell() -> ConfigCell {
        let cell = list.dequeueReusableCell(withIdentifier: ConfigCell.className) as! ConfigCell
        return cell
    }
    
    private func getOptionTableViewCell() -> OptionTableViewCell {
        let cell = list.dequeueReusableCell(withIdentifier: OptionTableViewCell.className) as! OptionTableViewCell
        return cell
    }
    
    deinit {
//#if DEBUG
        print("dealloc \(type(of: self))")
//#endif
    }

    enum ConfigCellType: CaseIterable {
        case bussinessSever
        case skipVerifyCert

        var title: String {
            switch self {
            case .bussinessSever:
                return "服务器:".innerLocalized()
            case .skipVerifyCert:
                return "忽略证书校验".innerLocalized()
            }
        }
    }
}
