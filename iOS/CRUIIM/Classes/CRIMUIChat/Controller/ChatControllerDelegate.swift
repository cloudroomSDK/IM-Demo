import Foundation
import CRUICore

protocol ChatControllerDelegate: AnyObject {
    func isInGroup(with isIn: Bool)
    func update(with sections: [Section], requiresIsolatedProcess: Bool)
    func updateUnreadCount(count: Int)
    func didTapAvatar(with id: String)
    func didTapContent(with id: String, data: Message.Data)
    func didLongPressContent(with id: String, bubbleView: UIView, data: Message.Data)
}

extension ChatControllerDelegate {
    func isInGroup(with _: Bool) {}
    func updateUnreadCount(_: Int) {}
    func didTapAvatar(with _: String) {}
    func didTapContent(with _: String, _:  Message.Data) {}
    func didLongPressContent(with _: String, _: UIView, _: Message.Data) {}
}
