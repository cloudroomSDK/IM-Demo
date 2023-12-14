
import RxSwift
import CRUICore
import CRUICoreView
import FDFullscreenPopGesture

class SearchGroupViewController: UIViewController {
    
    var didSelectedItem: ((_ ID: String) -> Void)?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        replaceSystemBackBarButtonItem()
        fd_interactivePopDisabled = true
        view.backgroundColor = .white
        let resultVC = SearchResultViewController(searchType: .group)
        let searchVC: UISearchController = {
            let v = UISearchController(searchResultsController: resultVC)
            v.searchResultsUpdater = resultVC
            v.searchBar.placeholder = "通过群ID号搜索添加"
            v.obscuresBackgroundDuringPresentation = false
            return v
        }()
        navigationItem.searchController = searchVC
        resultVC.didSelectedItem = didSelectedItem
        
        
    }
    
    
}
