//
//  IdentifierCopyCell.swift
//  CRUIIM
//
//  Created by YunWu01 on 2023/12/6.
//

import RxSwift

class IdentifierCopyCell: UITableViewCell {

    var copyButtonTappedHandler: (() -> Void)?
    
    public var disposeBag = DisposeBag()

    public let titleLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f17
        v.textColor = UIColor.c0C1C33
        
        return v
    }()

    public let subtitleLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f17
        v.textColor = UIColor.c0C1C33
        v.textAlignment = .right
        
        return v
    }()
    
    public let copyBtn: UIButton = {
        let v = UIButton.init(type: .custom)
        v.setImage(UIImage(nameInBundle: "profile_copy_id_button_icon"), for: .normal)
        v.isHidden = true
        
        return v
    }()

    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        accessoryType = .disclosureIndicator
        selectionStyle = .none

        titleLabel.setContentHuggingPriority(.required, for: .horizontal)
        copyBtn.setContentHuggingPriority(.required, for: .horizontal)
        
        titleLabel.setContentCompressionResistancePriority(.required, for: .horizontal)
        copyBtn.setContentCompressionResistancePriority(.required, for: .horizontal)
        
        copyBtn.addTarget(self, action: #selector(copyButtonTapped), for: .touchUpInside)
        let row = UIStackView(arrangedSubviews: [titleLabel, subtitleLabel, copyBtn])
        row.spacing = 8
        row.distribution = .fillProportionally
        contentView.addSubview(row)

        row.snp.makeConstraints { make in
            make.edges.equalToSuperview().inset(16)
        }
    }

    @available(*, unavailable)
    required public init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    open override func prepareForReuse() {
        super.prepareForReuse()
        accessoryType = .disclosureIndicator
        subtitleLabel.text = nil
        titleLabel.text = nil
        titleLabel.isHidden = false
        titleLabel.textColor = UIColor.c0C1C33
        disposeBag = DisposeBag()
        copyBtn.isHidden = true
    }

    @objc func copyButtonTapped() {
        self.copyButtonTappedHandler?()
    }
}
