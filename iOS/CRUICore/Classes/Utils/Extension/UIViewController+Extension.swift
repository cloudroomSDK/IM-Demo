
extension UIViewController {
    public func presentAlert(title: String? = nil, confirmHandler: (() -> Void)? = nil) {
        let alertController = SPAlertController.alertController(withTitle: title, message: nil, preferredStyle: .actionSheet)
        alertController.messageColor = .c353535
        
        let action1 = SPAlertAction.action(withTitle: "确定".innerLocalized(), style: .default) { (action) in
            confirmHandler?()
        }
        action1.titleColor = .c0089FF
        alertController.addAction(action: action1)
        
        let action2 = SPAlertAction.action(withTitle: "取消".innerLocalized(), style: .default) { (action) in
        }
        action2.titleColor = .c0089FF
        alertController.addAction(action: action2)
        
        present(alertController, animated: true)
    }
    
    public func presentActionSheet(title: String? = nil,
                            action1Title: String,
                            action1Handler: (() -> Void)?,
                            action2Title: String? = nil,
                            action2Handler: (() -> Void)? = nil) {
        
        let sheet = SPAlertController.alertController(withTitle: nil, message: title, preferredStyle: .actionSheet)
        sheet.messageColor = .c353535
        let action1 = SPAlertAction.action(withTitle: action1Title, style: .default) { [weak self] (action) in
            action1Handler?()
        }
        action1.titleColor = .c353535
        sheet.addAction(action: action1)
        
        if let action2Title {
            let action2 = SPAlertAction.action(withTitle: action2Title, style: .default) { [weak self] (action) in
                action2Handler?()
            }
            action2.titleColor = .c353535
            sheet.addAction(action: action2)
        }
        let cancelAction = SPAlertAction.action(withTitle: "取消".innerLocalized(), style: .default) { (action) in
        }
        cancelAction.titleColor = .c353535
        sheet.addAction(action: cancelAction)
        present(sheet, animated: true, completion: nil)
    }
    
    public func presentForwardConfirmAlert(contacts: [ContactInfo], abstruct: String, confirmHandler: (() -> Void)? = nil) {
        let alertHeaderView = SendConfirmationAlertView()
        alertHeaderView.reloadCollectionView(contacts: contacts, msgAbstract: abstruct)
        
        let sheet = SPAlertController.alertController(withCustomHeaderView: alertHeaderView, preferredStyle: .alert, animationType: .default)
        let cancelAction = SPAlertAction.action(withTitle: "取消".innerLocalized(), style: .default) { [weak self] (action) in
            
        }
        let sendAction = SPAlertAction.action(withTitle: "发送".innerLocalized(), style: .default) { [weak self] (action) in
            confirmHandler?()
        }
        
        cancelAction.titleColor = .c353535
        sendAction.titleColor = .c0584FE
        sheet.addAction(action: cancelAction)
        sheet.addAction(action: sendAction)
        self.present(sheet, animated: true, completion: nil)
    }
    
}

fileprivate var popActionKey: Void?
extension UIViewController {
    
    var action: (() -> Void)? {
        get {
            return objc_getAssociatedObject(self, &popActionKey) as? () -> Void
        }
        set {
            objc_setAssociatedObject(self, &popActionKey, newValue, .OBJC_ASSOCIATION_COPY_NONATOMIC)
        }
    }
    
    public func replaceSystemBackBarButtonItem(_ title: String? = nil, _ action: (() -> Void)? = nil) {
        let button = UIButton(type: .system)
        button.setTitle(title ?? "      ", for: .normal)
        button.setImage(UIImage(systemName: "chevron.left"), for: .normal)
        button.sizeToFit()
        button.bounds = CGRectMake(0, 0, button.bounds.width, 44)
        
        let backBarButtonItem = UIBarButtonItem(customView: button)
        navigationItem.leftBarButtonItem = backBarButtonItem
        
        button.addTarget(self, action: #selector(buttonAction), for: .touchUpInside)
    }
    
    @objc
    private func buttonAction() {
        self.action?()
        navigationController?.popViewController(animated: true)
    }
}
