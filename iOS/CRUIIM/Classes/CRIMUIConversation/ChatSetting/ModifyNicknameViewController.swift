
import CRUICore
import RxSwift

open class ModifyNicknameViewController: UIViewController {
    public let disposeBag = DisposeBag()

    public lazy var nameTextField: UITextField = {
        let v = UITextField()
        v.clearButtonMode = .always
        v.textColor = .c0C1C33
        v.font = .f17
        v.text = "昵称".innerLocalized()
        
        let maxTextLength = 30
        // 监听UITextView的文本变化
        v.rx.text.orEmpty
            .map { String($0.prefix(maxTextLength)) } // 限制输入文本的长度
            .bind(to: v.rx.text) // 将限制后的文本绑定回UITextView
            .disposed(by: disposeBag)
        
        v.rx.text.orEmpty.asDriver().map({ $0.count > 0}).drive(completeBtn.rx.isEnabled).disposed(by: disposeBag)
        
        return v
    }()
    
    public lazy var completeBtn: UIButton = {
        let v = UIButton(type: .system)
        v.setTitle("保存".innerLocalized(), for: .normal)
        
        return v
    }()
    
    open override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .viewBackgroundColor
        navigationItem.title = "昵称".innerLocalized()
        replaceSystemBackBarButtonItem()

        let rightButton = UIBarButtonItem(customView: completeBtn)
        navigationItem.rightBarButtonItem = rightButton
        
        let topLine: UIView = {
            let v = UIView()
            v.backgroundColor = .sepratorColor
            return v
        }()

        let container = UIView()
        container.backgroundColor = .cellBackgroundColor
        
        container.addSubview(topLine)
        topLine.snp.makeConstraints { make in
            make.left.top.right.equalToSuperview()
            make.height.equalTo(1)
        }

        let bottomLine: UIView = {
            let v = UIView()
            v.backgroundColor = .sepratorColor
            return v
        }()

        container.addSubview(bottomLine)
        bottomLine.snp.makeConstraints { make in
            make.left.bottom.right.equalToSuperview()
            make.height.equalTo(1)
        }

        container.addSubview(nameTextField)
        nameTextField.snp.makeConstraints { make in
            make.left.equalToSuperview().offset(15)
            make.centerY.equalToSuperview()
            make.height.equalTo(44)
            make.right.equalToSuperview().offset(-15)
        }

        view.addSubview(container)
        container.snp.makeConstraints { make in
            make.top.equalToSuperview().offset(15 + kStatusBarHeight + 44)
            make.left.right.equalToSuperview().inset(0)
            make.height.equalTo(46)
        }
    }
}
