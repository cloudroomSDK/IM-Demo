
import Foundation

protocol ReloadDelegate: AnyObject {

    func reloadMessage(with id: String)
    func didTapAvatar(with id: String)
    func didTapContent(with id: String,  data: Message.Data)
    func didLongPressContent(with id: String, bubbleView: UIView,  data: Message.Data)
    func removeMessage(messageID: String)
    func reeditMessage(with id: String)
}

// view 的点击代理，经过controler 传递到 view controller
extension ReloadDelegate {
    func didTapAvatar(with _: String) {}
    func didTapContent(with _: String, _: Message.Data) {}
    func didLongPressContent(with _: String, _: UIView,  _: Message.Data) {}
    func removeMessage(_: String) {}
    func reeditMessage(with _: String) {}
}
