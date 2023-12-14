
import Foundation
import UIKit
import RxSwift
import SnapKit
import CRUICore
import ProgressHUD

let kAppIDDefaultShow = "默认appID"

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
    
    var onTextChanged: ((String?) -> Void)?
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        backgroundColor = .white
        
        contentView.addSubview(titleLabel)
        titleLabel.snp.makeConstraints { make in
            make.left.equalToSuperview().offset(15)
            make.width.equalTo(88)
            make.centerY.equalToSuperview()
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
                return inputTextFiled?.text?.replacingOccurrences(of: " ", with: "") ?? ""
            }
            .bind(to: inputTextFiled.rx.text)
            .disposed(by: disposeBag)
    }
    
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
            return !string.contains(" ")
        }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

let severAddressKey = "com.crimuikit.adr"
let adminSeverAddrKey = "com.criuikit.admin.adr"
let bussinessSeverAddrKey = "com.crimuikit.bussiness.api.adr"
let sdkAPIAddrKey = "com.crimuikit.sdk.api.adr"
let sdkAPPIDKey = "com.crimuikit.sdk.appId"
let sdkAPPSecretKey = "com.crimuikit.sdk.appSecret"
let sdkTokenKey = "com.crimuikit.sdk.token"
let sdkObjectStorageKey = "com.crimuikit.sdk.os"
let useTokenKey = "com.crimuikit.use.Token"

class ConfigViewController: UIViewController {

    let disposeBag = DisposeBag()
    
    let rowHeight = 54
    let authenTypeAppID = "帐号密码鉴权方式"
    let authenTypeToken = "Token鉴权方式"
    
    // 读取鉴权方式
    let enableToken = UserDefaults.standard.object(forKey: useTokenKey) == nil ? false : UserDefaults.standard.bool(forKey: useTokenKey)
    lazy var currentAuthen = enableToken ? authenTypeToken : authenTypeAppID

    let ports = [bussinessSeverAddrKey: bussinessPort]
    let scheme = [bussinessSeverAddrKey: "http", sdkAPIAddrKey: "http"]

    private var severAddress = UserDefaults.standard.string(forKey: severAddressKey) ?? defaultDomain
    private var bussinessSeverAddr = UserDefaults.standard.string(forKey: bussinessSeverAddrKey) ?? "http://\(defaultIP)\(bussinessPort)"
    private var sdkAPIAddr = UserDefaults.standard.string(forKey: sdkAPIAddrKey) ?? defaultSDKApi
    private var token = UserDefaults.standard.string(forKey: sdkTokenKey) ?? defaultToken
    private var appID = UserDefaults.standard.string(forKey: sdkAPPIDKey) ?? defaultAppID
    private var appSecret = UserDefaults.standard.string(forKey: sdkAPPSecretKey) ?? defaultAppSecret
    private var sdkObjectStorage = UserDefaults.standard.string(forKey: sdkObjectStorageKey) ?? "minio"

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
        v.inputTextFiled.text = severAddress
        
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
    
    private lazy var sdkServerCell: ConfigCell = {
        let v = getConfigCell()
        let value = ConfigCellType.sdkServer
        v.titleLabel.text = value.title
        v.inputTextFiled.text = sdkAPIAddr
        return v
    }()
    
    private lazy var authenTypeCell: OptionTableViewCell = {
        let v = getOptionTableViewCell()
        let value = ConfigCellType.authenType
        v.titleLabel.text = value.title
        v.subtitleLabel.text = currentAuthen
        
        let line = UIView()
        line.backgroundColor = DemoUI.color_E0E0E0
        v.addSubview(line)
        line.snp.makeConstraints { make in
            make.left.equalToSuperview().offset(15)
            make.bottom.right.equalToSuperview()
            make.height.equalTo(1)
        }
        
        let tap = UITapGestureRecognizer()
        tap.rx.event.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            
            ConfigAuthenTypeView.show(onWindowOf: sself.view, alertTitle: "鉴权方式", currentItem: sself.currentAuthen, options: [sself.authenTypeAppID, sself.authenTypeToken], confirmTitle: "确定") { rslt  in
                sself.currentAuthen = rslt ?? ""
                sself.authenTypeCell.subtitleLabel.text = sself.currentAuthen
                sself.reloadRelatedRows()
            }
        }).disposed(by: disposeBag)
        v.addGestureRecognizer(tap)
        return v
    }()
    
    private lazy var appIDCell: ConfigCell = {
        let v = getConfigCell()
        let value = ConfigCellType.appID
        v.titleLabel.text = value.title
        
        let line = UIView()
        line.backgroundColor = DemoUI.color_E0E0E0
        v.contentView.addSubview(line)
        line.snp.makeConstraints { make in
            make.left.equalToSuperview().offset(15)
            make.bottom.right.equalToSuperview()
            make.height.equalTo(1)
        }
        
        let appIDCache = UserDefaults.standard.string(forKey: sdkAPPIDKey) ?? ""
        if appIDCache.isEmpty == true, KDefaultAppID.isEmpty == false, appID == KDefaultAppID {
            v.inputTextFiled.text = kAppIDDefaultShow
        } else {
            v.inputTextFiled.text = appID
        }
        return v
    }()
    
    private lazy var appSecretCell: ConfigCell = {
        let v = getConfigCell()
        v.inputTextFiled.isSecureTextEntry = true
        let value = ConfigCellType.appSecret
        v.titleLabel.text = value.title
        v.inputTextFiled.text = appSecret
        return v
    }()
    
    private lazy var tokenCell: ConfigCell = {
        let v = getConfigCell()
        let value = ConfigCellType.token
        v.titleLabel.text = value.title
        v.inputTextFiled.text = token
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
            ProgressHUD.showSucceed()
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
        
        var arrangedViews = [UIView]()
        if currentAuthen == authenTypeToken {
            arrangedViews = [bussinessServerCell, sdkServerCell, spacer, authenTypeCell, tokenCell]
        } else {
            arrangedViews = [bussinessServerCell, sdkServerCell, spacer, authenTypeCell, appIDCell, appSecretCell]
        }
        for value in arrangedViews {
            vStack.addArrangedSubview(value)
        }
        
        bussinessServerCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        sdkServerCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        spacer.snp.remakeConstraints { make in
            make.height.equalTo(16)
        }
        authenTypeCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        appIDCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        appSecretCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        
        vStack.bounds = CGRect(x: 0, y: 0, width: Int(kScreenWidth), height: rowHeight * (arrangedViews.count - 1) + 16)
        list.tableHeaderView = vStack
    }
    
    @objc func clearCacheConfig() {
        self.view.endEditing(true)
        
        let ud = UserDefaults.standard
        ud.removeObject(forKey: severAddressKey)
        ud.removeObject(forKey: bussinessSeverAddrKey)
        ud.removeObject(forKey: sdkAPIAddrKey)
        ud.removeObject(forKey: sdkObjectStorageKey)
        ud.removeObject(forKey: useTokenKey)
        ud.removeObject(forKey: sdkTokenKey)
        ud.removeObject(forKey: sdkAPPIDKey)
        ud.removeObject(forKey: sdkAPPSecretKey)
        
        currentAuthen = authenTypeAppID
        
        severAddress = UserDefaults.standard.string(forKey: severAddressKey) ?? defaultDomain
        bussinessSeverAddr = UserDefaults.standard.string(forKey: bussinessSeverAddrKey) ?? "http://\(defaultIP)\(bussinessPort)"
        sdkAPIAddr = UserDefaults.standard.string(forKey: sdkAPIAddrKey) ?? defaultSDKApi
        token = UserDefaults.standard.string(forKey: sdkTokenKey) ?? defaultToken
        appID = UserDefaults.standard.string(forKey: sdkAPPIDKey) ?? defaultAppID
        appSecret = UserDefaults.standard.string(forKey: sdkAPPSecretKey) ?? defaultAppSecret
        sdkObjectStorage = UserDefaults.standard.string(forKey: sdkObjectStorageKey) ?? "minio"
                
        bussinessServerCell.inputTextFiled.text = severAddress
        sdkServerCell.inputTextFiled.text = sdkAPIAddr
        authenTypeCell.subtitleLabel.text = currentAuthen
        let appIDCache = UserDefaults.standard.string(forKey: sdkAPPIDKey) ?? ""
        if appIDCache.isEmpty == true, KDefaultAppID.isEmpty == false, appID == KDefaultAppID {
            appIDCell.inputTextFiled.text = kAppIDDefaultShow
        } else {
            appIDCell.inputTextFiled.text = appID
        }
        appSecretCell.inputTextFiled.text = appSecret
        tokenCell.inputTextFiled.text = token

    }

    @objc func saveAddress() {

        self.view.endEditing(true)
        
        severAddress = bussinessServerCell.inputTextFiled.text ?? defaultHost
        bussinessSeverAddr = scheme[bussinessSeverAddrKey]! + "://" + severAddress + ports[bussinessSeverAddrKey]!
        sdkAPIAddr = sdkServerCell.inputTextFiled.text ?? defaultSDKApi
        let newSdkAPIAddr = scheme[sdkAPIAddrKey]! + "://" + sdkAPIAddr
        
        token = tokenCell.inputTextFiled.text ?? defaultToken
        appID = appIDCell.inputTextFiled.text ?? defaultAppID
        appSecret = appSecretCell.inputTextFiled.text ?? defaultAppSecret

        let ud = UserDefaults.standard
        ud.set(severAddress, forKey: severAddressKey)
        ud.set(bussinessSeverAddr, forKey: bussinessSeverAddrKey)
        ud.set(sdkAPIAddr, forKey: sdkAPIAddrKey)
        ud.set(sdkObjectStorage, forKey: sdkObjectStorageKey)
        ud.set(currentAuthen == authenTypeToken, forKey: useTokenKey)
        ud.set(token, forKey: sdkTokenKey)
        ud.set(appSecret, forKey: sdkAPPSecretKey)
        
        // 更新业务地址
        AccountViewModel.API_BASE_URL = bussinessSeverAddr
        
        // 不存储默认appID信息
        if appID.isEmpty == false, appID != defaultAppID, appID != kAppIDDefaultShow {
            ud.set(appID, forKey: sdkAPPIDKey)
        } else {
            ud.set(nil, forKey: sdkAPPIDKey)
        }
        
        ud.synchronize()
        AccountViewModel.saveUser(uid: AccountViewModel.userID , imToken: nil, chatToken: nil)
        
        // 反初始化SDK
        IMController.shared.unInitSDK()
        // 初始化SDK
        IMController.shared.setup(sdkAPIAdrr: newSdkAPIAddr,
                                  sdkOS: sdkObjectStorage) {
            IMController.shared.currentUserRelay.accept(nil)
            AccountViewModel.saveUser(uid: nil, imToken: nil, chatToken: nil)
            NotificationCenter.default.post(name: .init("logout"), object: nil)
        }
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
        case sdkServer
        case authenType
        case appID
        case appSecret
        case token

        var title: String {
            switch self {
            case .bussinessSever:
                return "业务服务器:".innerLocalized()
            case .sdkServer:
                return "SDK服务器:".innerLocalized()
            case .authenType:
                return "   鉴权方式:".innerLocalized()
            case .appID:
                return "AppID:".innerLocalized()
            case .appSecret:
                return "AppSecret:".innerLocalized()
            case .token:
                return "Token:".innerLocalized()
            }
        }
    }
}
