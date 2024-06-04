
import UIKit
import RxSwift
import RxCocoa
import RxGesture
import MMBAlertsPickers
import SnapKit
import ProgressHUD
import CRUICore

class LoginViewController: UIViewController {
    
    private let _disposeBag = DisposeBag()
    
    private var _areaCode = "+86"
    
    private let logoImageView: UIImageView = {
        let v = UIImageView(image: UIImage(named: "logo_image"))
        
        return v
    }()
    
    private let versionLabel: UILabel = {
        let v = UILabel()
        v.textColor = DemoUI.color_666666
        v.numberOfLines = 2
        v.textAlignment = .center
        
        let infoDictionary = Bundle.main.infoDictionary
        let majorVersion = infoDictionary!["CFBundleShortVersionString"] as! String //主程序版本号
        v.text = "V " + majorVersion
        
        return v
    } ()
    
    private let settingButton: UIButton = {
        let v = UIButton()
        v.setImage(UIImage(named: "ic_setting"), for: .normal)
        
        return v
    }()
    
    private let phoneLabel: UILabel = {
        let v = UILabel()
        v.textColor = DemoUI.color_3D3D3D
        v.text = "验证码登录".localized()
        v.font = UIFont.preferredFont(forTextStyle: .title3)
        
        return v
    }()
    
    private lazy var phoneSegment: UIButton = {
        let v = UIButton()
        var placeholder = "未注册的手机号通过验证后自动注册".localized()
        v.setTitle(placeholder, for: .normal)
        v.setTitleColor(DemoUI.color_666666, for: .normal)
        v.titleLabel?.font = DemoUI.smallFont
        
        return v
    }()
    
    var phone: String? {
        return phoneTextField.text?.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)
    }
    
    var areaCode: String {
        return _areaCode
    }
    
    var verificationCode: String? {
        return passwordTextField.text?.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)
    }
    
    let countDownButton: CountDownTimerButton = {
        let t = CountDownTimerButton()
        
        return t
    }()
    
    lazy var areaCodeButton: UIButton = {
        let t = UIButton(type: .custom)
        t.setTitle("\(_areaCode)", for: .normal)
        t.setTitleColor(DemoUI.color_0C1C33, for: .normal)
        t.titleLabel?.font = .systemFont(ofSize: 17)
        t.isUserInteractionEnabled = false
        
        t.rx.tap.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            let alert = UIAlertController(style: .actionSheet, title: "Phone Codes")
            alert.addLocalePicker(type: .phoneCode) {[weak self] info in
                // action with selected object
                guard let phoneCode = info?.phoneCode else {return}
                self?._areaCode = phoneCode
                t.setTitle("\(phoneCode)", for: .normal)
            }
            
            alert.addAction(title: "取消".localized(), style: .cancel)
            sself.present(alert, animated: true)
        }).disposed(by: _disposeBag)
        return t
    }()
    
    private lazy var phoneTextField: UITextField = {
        let v = UITextField()
        v.keyboardType = .numberPad
        var placeholder = "请输入手机号码".localized()
        v.placeholder = placeholder
        v.clearButtonMode = .whileEditing
        v.text = AccountViewModel.perLoginAccount
        v.textColor = DemoUI.color_0C1C33
        
        return v
    }()
    
    private lazy var passwordTextField: UITextField = {
        let v = UITextField()
        v.keyboardType = .numberPad
        v.placeholder = "请输入验证码".localized()
        v.isSecureTextEntry = false
        v.rightView = countDownButton
        v.rightViewMode = .always
        v.borderStyle = .none
        v.textColor = DemoUI.color_0C1C33
        v.clearButtonMode = .whileEditing
        v.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 8, height: 1))
        v.leftViewMode = .always
        
        return v
    }()
    
    private let captchaPromptLabel: UILabel = {
        let v = UILabel()
        v.textColor = .lightGray
        v.text = "私有化验证码用8888".localized()
        v.font = UIFont.preferredFont(forTextStyle: .body)
        v.isHidden = true
        
        return v
    }()
    
    lazy var loginBtn: UIButton = {
        let v = UIButton(type: .system)
        v.setTitle("登录/注册".localized(), for: .normal)
        v.setTitleColor(.white, for: .normal)
        v.backgroundColor = DemoUI.color_3981FC
        v.titleLabel?.font = UIFont.preferredFont(forTextStyle: .title3)
        v.layer.cornerRadius = DemoUI.cornerRadius
        
        return v
    }()
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        navigationController?.setNavigationBarHidden(true, animated: false)
        updateCountDownTimerButton()
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .white
        
        let bgImageView = UIImageView()
        bgImageView.frame = view.bounds
        view.addSubview(bgImageView)
        
        countDownButton.clickedBlock = { [weak self] sender in
            guard let sself = self,
                  let phone = sself.phoneTextField.text?.trimmingCharacters(in: .whitespacesAndNewlines) else { return }
            
            guard sself.validatePhoneNumber(phone) else {
                ProgressHUD.showError("填写正确的手机号码".localized())
                return
            }
            
            AccountViewModel.requestCode(phone: phone, areaCode: sself.areaCode, useFor: .login) { (errCode, errMsg) in
                if errMsg != nil {
                    ProgressHUD.showError(errMsg)
                } else {
                    ProgressHUD.showSuccess("验证码发送成功".localized())
                }
            }
        }
        
        view.addSubview(logoImageView)
        logoImageView.snp.makeConstraints { make in
            make.centerX.equalToSuperview()
            make.top.equalToSuperview().offset(kStatusBarHeight + 44)
            make.size.equalTo(150)
        }
        
        view.addSubview(versionLabel)
        versionLabel.snp.makeConstraints { make in
            make.centerX.equalToSuperview()
            make.top.equalTo(logoImageView.snp.bottom).offset(5)
        }
        
        settingButton.rx.tap.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            sself.showConfigViewController()
        }).disposed(by: _disposeBag)
        
        view.addSubview(settingButton)
        settingButton.snp.makeConstraints { make in
            make.top.equalToSuperview().offset(kStatusBarHeight)
            make.right.equalToSuperview().offset(-15)
            make.size.equalTo(44)
        }
        
        // 登录
        let container: UIView = {
            let v = UIView()
            return v
        }()
        
        view.addSubview(container)
        container.snp.makeConstraints { make in
            make.top.equalTo(versionLabel.snp.bottom).offset(60)
            make.leading.trailing.equalToSuperview().inset(32)
        }
        
        container.addSubview(phoneLabel)
        phoneLabel.snp.makeConstraints { make in
            make.leading.equalToSuperview()
            make.top.equalToSuperview()
        }
        
        container.addSubview(phoneSegment)
        phoneSegment.snp.makeConstraints { make in
            make.leading.equalToSuperview()
            make.top.equalTo(phoneLabel.snp.bottom).offset(3)
        }
        
        let line = UIView()
        line.backgroundColor = DemoUI.color_E8EAEF
        
        let accountStack = UIStackView(arrangedSubviews: [areaCodeButton, line, phoneTextField])
        accountStack.spacing = 8
        accountStack.alignment = .center
        
        phoneTextField.setContentHuggingPriority(UILayoutPriority(248), for: .horizontal)
        
        areaCodeButton.snp.makeConstraints { make in
            make.width.equalTo(60)
        }
        
        container.addSubview(accountStack)
        accountStack.snp.makeConstraints { make in
            make.leading.trailing.equalToSuperview()
            make.top.equalTo(phoneSegment.snp.bottom).offset(4)
        }
        
        line.snp.makeConstraints { make in
            make.width.equalTo(1)
            make.top.bottom.equalToSuperview().inset(13)
        }
        
        phoneTextField.snp.makeConstraints { make in
            make.height.equalTo(42)
        }
        
        let phoneBorder = UIView()
        phoneBorder.backgroundColor = DemoUI.color_E8EAEF
        container.addSubview(phoneBorder)
        phoneBorder.snp.makeConstraints { make in
            make.height.equalTo(1)
            make.leading.trailing.equalTo(accountStack)
            make.top.equalTo(accountStack.snp.bottom)
        }
  
        container.addSubview(passwordTextField)
        passwordTextField.snp.makeConstraints { make in
            make.top.equalTo(accountStack.snp.bottom).offset(16)
            make.leading.trailing.bottom.equalToSuperview()
            make.height.equalTo(42)
        }
        
        let passwordBorder = UIView()
        passwordBorder.backgroundColor = DemoUI.color_E8EAEF
        container.addSubview(passwordBorder)
        passwordBorder.snp.makeConstraints { make in
            make.height.equalTo(1)
            make.leading.trailing.equalTo(passwordTextField)
            make.top.equalTo(passwordTextField.snp.bottom)
        }
        
        container.addSubview(captchaPromptLabel)
        captchaPromptLabel.snp.makeConstraints { make in
            make.leading.equalTo(passwordBorder).offset(8)
            make.top.equalTo(passwordBorder.snp.bottom).offset(3)
        }

        view.addSubview(loginBtn)
        loginBtn.snp.makeConstraints { make in
            make.height.equalTo(40)
            make.top.equalTo(container.snp.bottom).offset(68)
            make.leading.trailing.equalTo(container)
        }
        
    }
    
    deinit {
#if DEBUG
        print("dealloc \(type(of: self))")
#endif
    }
    
    func updateCountDownTimerButton() {
        guard !countDownButton.isCounting else {
            return
        }
        
        let sdkAPIAddr = UserDefaults.standard.string(forKey: sdkAPIAddrKey) ?? defaultSDKApi
        if sdkAPIAddr.isEmpty == false, sdkAPIAddr != "sdk.cloudroom.com" {
            captchaPromptLabel.isHidden = false
        } else {
            captchaPromptLabel.isHidden = true
        }
    }
    
    func toRegister() {
        let vc = InputAccountViewController()
        navigationController?.pushViewController(vc, animated: true)
    }
    
    func showConfigViewController() {
        let vc = ConfigViewController()
        navigationController?.pushViewController(vc, animated: true)
    }
    
    func validatePhoneNumber(_ phoneNumber: String) -> Bool {
        let phoneRegex = #"^1[3456789]\d{9}$"#
        let phonePredicate = NSPredicate(format: "SELF MATCHES %@", phoneRegex)
        return phonePredicate.evaluate(with: phoneNumber)
    }
}

extension UIButton {
    
    func setBackgroundColor(color: UIColor, forState: UIControl.State) {
        
        UIGraphicsBeginImageContext(CGSize(width: 1, height: 1))
        UIGraphicsGetCurrentContext()!.setFillColor(color.cgColor)
        UIGraphicsGetCurrentContext()!.fill(CGRect(x: 0, y: 0, width: 1, height: 1))
        let colorImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        self.setBackgroundImage(colorImage, for: forState)
    }
}
