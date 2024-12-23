
import RxSwift
import CRUICore

class GroupBasicInfoCell: UITableViewCell {
    
    public var disposeBag = DisposeBag()
    
    let editImageView = UIImageView(image: UIImage(nameInBundle: "contact_group_setting_edit_icon"))
    let avatarView = AvatarView()
    
    var enableInput: Bool = false {
        didSet {
            textFiled.rightViewMode = enableInput ? .always : .never
        }
    }
    
    var inputHandler: (() -> Void)!
    var QRCodeTapHandler: (() -> Void)?
    var avatarViewTapHandler: (() -> Void)?
    
    lazy var textFiled: UITextField = {
        let v = UITextField()
        v.font = UIFont.f17
        v.textColor = UIColor.c0C1C33
        v.rightViewMode = .always
        v.rightView = UIImageView(image: UIImage(systemName: "square.and.pencil"))
        v.delegate = self
        
        return v
    }()
    
    let subLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f14
        v.textColor = UIColor.c8E9AB0
        
        return v
    }()

    private var _disposeBag = DisposeBag()
    
    lazy var QRCodeButton: UIButton = {
        let v = UIButton(type: .system)
        v.setImage(UIImage(systemName: "qrcode"), for: .normal)
        v.tintColor = .secondaryLabel
        v.isUserInteractionEnabled = true
        let tap = UITapGestureRecognizer(target: self, action: #selector(QRCodeAction))
        v.addGestureRecognizer(tap)
        
        return v
    }()
    
    @objc
    private func QRCodeAction() {
        self.QRCodeTapHandler?()
    }
    
    @objc
    private func avatarViewAction() {
        self.avatarViewTapHandler?()
    }
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        selectionStyle = .none
       
        let infoStack = UIStackView(arrangedSubviews: [textFiled, subLabel])
        infoStack.axis = .vertical
        infoStack.spacing = 4
        
        let hStack = UIStackView(arrangedSubviews: [avatarView, infoStack, UIView(), QRCodeButton])
        hStack.spacing = 8
        hStack.alignment = .center
        hStack.translatesAutoresizingMaskIntoConstraints = false
        
        contentView.addSubview(hStack)
        NSLayoutConstraint.activate([
            hStack.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: .margin16),
            hStack.topAnchor.constraint(equalTo: contentView.topAnchor, constant: .margin16),
            hStack.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -.margin16),
            hStack.bottomAnchor.constraint(equalTo: contentView.bottomAnchor, constant: -.margin16),
        ])
        
        contentView.addSubview(editImageView)
        editImageView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            editImageView.trailingAnchor.constraint(equalTo: avatarView.trailingAnchor),
            editImageView.bottomAnchor.constraint(equalTo: avatarView.bottomAnchor),
        ])
        
        let tap = UITapGestureRecognizer(target: self, action: #selector(avatarViewAction))
        avatarView.isUserInteractionEnabled = true
        avatarView.addGestureRecognizer(tap)
    }

    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func prepareForReuse() {
        super.prepareForReuse()
        _disposeBag = DisposeBag()
    }
}

extension GroupBasicInfoCell: UITextFieldDelegate {
    func textFieldShouldBeginEditing(_ textField: UITextField) -> Bool {
        if let inputHandler = inputHandler {
            inputHandler()
        }
        
        return false
    }
}

