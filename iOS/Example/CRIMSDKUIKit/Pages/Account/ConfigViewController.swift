
import Foundation
import UIKit
import RxSwift
import SnapKit
import CRUICore
import ProgressHUD
import CRUICalling

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
let bussinessSeverAddrKey = "com.crimuikit.bussiness.api.adr"
let adminSeverAddrKey = "com.criuikit.admin.adr"
let protocolKey = "com.crimuikit.use.protocol"

class ConfigViewController: UIViewController {

    let disposeBag = DisposeBag()
    
    let rowHeight = 54
    
    var currentProtocol = UserDefaults.standard.integer(forKey: protocolKey)
    let protocolTypes = ["http", "https(CA证书)", "https(自签名SSL证书)"]
    
    private var severAddress = UserDefaults.standard.string(forKey: severAddressKey) ?? defaultDomain
    private var bussinessSeverAddr = UserDefaults.standard.string(forKey: bussinessSeverAddrKey) ?? "http://\(defaultIP)\(bussinessPort)"

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
        v.titleLabel.textColor = DemoUI.color_666666
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
    
    private lazy var protocolTypeCell: OptionTableViewCell = {
        let v = getOptionTableViewCell()
        let value = ConfigCellType.protocolType
        v.titleLabel.text = value.title
        v.titleLabel.textColor = DemoUI.color_666666
        v.subtitleLabel.text = protocolTypes[currentProtocol]
        
        let tap = UITapGestureRecognizer()
        tap.rx.event.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            
            ConfigPickerView.show(onWindowOf: sself.view, alertTitle: "通讯协议", currentItem: sself.protocolTypes[sself.currentProtocol], options: sself.protocolTypes, confirmTitle: "确定") { index, title in
                sself.currentProtocol = index
                sself.protocolTypeCell.subtitleLabel.text = title
                sself.reloadRelatedRows()
            }
        }).disposed(by: disposeBag)
        v.addGestureRecognizer(tap)
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
        
        let arrangedViews = [bussinessServerCell, spacer, protocolTypeCell]
       
        for value in arrangedViews {
            vStack.addArrangedSubview(value)
        }
        
        bussinessServerCell.snp.remakeConstraints { make in
            make.height.equalTo(rowHeight)
        }
        spacer.snp.remakeConstraints { make in
            make.height.equalTo(16)
        }
        protocolTypeCell.snp.remakeConstraints { make in
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
                
        severAddress = UserDefaults.standard.string(forKey: severAddressKey) ?? defaultDomain
        bussinessSeverAddr = UserDefaults.standard.string(forKey: bussinessSeverAddrKey) ?? "http://\(defaultIP)\(bussinessPort)"
                
        bussinessServerCell.inputTextFiled.text = severAddress

    }

    @objc func saveAddress() {

        self.view.endEditing(true)
        
        severAddress = bussinessServerCell.inputTextFiled.text ?? defaultHost
        bussinessSeverAddr = (currentProtocol == 0 ? "http" : "https") + "://" + severAddress + (currentProtocol == 0 ? bussinessPort : httpsBussinessPort)

        let ud = UserDefaults.standard
        ud.set(severAddress, forKey: severAddressKey)
        ud.set(bussinessSeverAddr, forKey: bussinessSeverAddrKey)
        ud.setValue(currentProtocol, forKey: protocolKey)
        
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
        case sdkServer
        case authenType
        case appID
        case appSecret
        case token
        case protocolType

        var title: String {
            switch self {
            case .bussinessSever:
                return "服务器:".innerLocalized()
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
            case .protocolType:
                return "通讯协议:".innerLocalized()
            }
        }
    }
}
