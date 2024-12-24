
import Foundation
import RxRelay
import RxSwift
import CRUICore
import GTSDK
import CRUICalling

class MineViewModel {
    var currentUserRelay: BehaviorRelay<QueryUserInfo?> = .init(value: nil)

    private let _disposeBag = DisposeBag()
    
    func queryUserInfo() {
        guard let userID = AccountViewModel.userID else { return }
        AccountViewModel.queryUserInfo(userIDList: [userID],
                                       valueHandler: { [weak self] (users: [QueryUserInfo]) in
            guard let user: QueryUserInfo = users.first else { return }
            self?.currentUserRelay.accept(user)
        }, completionHandler: {(errCode, errMsg) in
        })
    }

    func updateGender(_ gender: Gender, completion: (() -> Void)?) {
        AccountViewModel.updateUserInfo(userID: IMController.shared.userID, gender: gender) { (errCode, errMsg) in
            completion?()
        }
    }

    func updateNickname(_ name: String, completion: (() -> Void)?) {
        AccountViewModel.updateUserInfo(userID: IMController.shared.userID, nickname: name) { (errCode, errMsg) in
            completion?()
        }
    }

    func updateBirthday(timeStampSeconds: Int, completion: (() -> Void)?) {
        AccountViewModel.updateUserInfo(userID: IMController.shared.userID, birth: timeStampSeconds * 1000) { (errCode, errMsg) in
            completion?()
        }
    }

    func updateFaceURL(url: String, onComplete: @escaping () -> Void) {
        AccountViewModel.updateUserInfo(userID: IMController.shared.userID, faceURL: url) { (errCode, errMsg) in
            onComplete()
        }
    }

    func logout() {
        IMController.shared.logout(onSuccess: { _ in
            GeTuiSdk.unbindAlias(IMController.shared.userID, andSequenceNum: "crim", andIsSelf: true)
            
            IMController.shared.currentUserRelay.accept(nil)
            AccountViewModel.saveUser(uid: nil, imToken: nil, chatToken: nil)
            NotificationCenter.default.post(name: .init("logout"), object: nil)
            
            // 反初始化SDK
            IMController.shared.unInitSDK()
            CRUICalling.CallingManager.manager.unInitVideoSDK()
        })
    }

    func uploadFile(fullPath: String, onProgress: @escaping (CGFloat) -> Void, onComplete: @escaping () -> Void, onFailure: @escaping () -> Void) {
        IMController.shared.uploadFile(fullPath: fullPath, onProgress: onProgress) { [weak self] url in
            if let url = url {
                self?.updateFaceURL(url: url, onComplete: onComplete)
            }
        } onFailure: { errCode, errMsg in
            onFailure()
        }
    }
}
