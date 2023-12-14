
import RxSwift
import CRUICore

class QuitTableViewCell: UITableViewCell {
    var disposeBag = DisposeBag()

    let titleLabel: UILabel = {
        let v = UILabel()
        v.font = .f17
        v.backgroundColor = .c0584FE
        v.textColor = .white
        v.textAlignment = .center
        
        return v
    }()

    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        selectionStyle = .none
        contentView.addSubview(titleLabel)
        titleLabel.snp.makeConstraints { make in
            make.edges.equalToSuperview()
            make.height.equalTo(44)
        }
    }

    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func prepareForReuse() {
        super.prepareForReuse()
        disposeBag = DisposeBag()
    }
}
