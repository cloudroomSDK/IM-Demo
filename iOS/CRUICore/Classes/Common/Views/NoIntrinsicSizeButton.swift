
import UIKit

public class NoIntrinsicSizeButton: UIButton {
    public weak var delegate: UIResponder?
    public override var intrinsicContentSize: CGSize {
        return CGSize(width: UIView.noIntrinsicMetric, height: UIView.noIntrinsicMetric)
    }

    public override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        delegate?.touchesMoved(touches, with: event)
    }
}
