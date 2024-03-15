
import RxSwift
import CRUICore

class GroupChatNameTableViewCell: UITableViewCell {
    var disposeBag = DisposeBag()
    var avatarViewTapHandler: (() -> Void)?
    
    let avatarImageView: UIImageView = {
        let v = UIImageView()
        v.layer.cornerRadius = 24
        v.clipsToBounds = true
        return v
    }()

    let titleLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f17
        v.textColor = UIColor.c0C1C33
        
        return v
    }()
    
    lazy var nameTextFiled: UITextField = {
        let v = UITextField()
        v.placeholder = "取个群名称".innerLocalized()
        
        let maxTextLength = 60
        // 监听UITextView的文本变化
        v.rx.text.orEmpty
            .map { String($0.prefix(maxTextLength)) } // 限制输入文本的长度
            .bind(to: v.rx.text) // 将限制后的文本绑定回UITextView
            .disposed(by: disposeBag)
        
        return v
    }()
    
    @objc
    private func avatarImageViewAction() {
        self.avatarViewTapHandler?()
    }

    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        selectionStyle = .none
        contentView.addSubview(avatarImageView)
        avatarImageView.snp.makeConstraints { make in
            make.size.equalTo(48)
            make.left.equalToSuperview().offset(StandardUI.margin_22)
            make.top.equalToSuperview().inset(20)
            make.bottom.equalToSuperview().inset(20).priority(.low)
        }

        contentView.addSubview(titleLabel)
        titleLabel.snp.makeConstraints { make in
            make.left.equalTo(avatarImageView.snp.right).offset(20)
            make.centerY.equalToSuperview()
            make.right.equalToSuperview().offset(-20)
        }
        
        contentView.insertSubview(nameTextFiled, belowSubview: titleLabel)
        nameTextFiled.snp.makeConstraints { make in
            make.edges.equalTo(titleLabel)
        }
        
        let tap = UITapGestureRecognizer(target: self, action: #selector(avatarImageViewAction))
        avatarImageView.isUserInteractionEnabled = true
        avatarImageView.addGestureRecognizer(tap)
    }

    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func prepareForReuse() {
        super.prepareForReuse()
        disposeBag = DisposeBag()
        nameTextFiled.isHidden = true
    }
    
    var enableInput: Bool = false {
        didSet {
            titleLabel.isHidden = enableInput
            nameTextFiled.isHidden = !enableInput
        }
    }
}
