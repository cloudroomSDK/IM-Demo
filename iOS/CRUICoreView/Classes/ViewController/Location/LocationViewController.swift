//
//  SelectLocationViewController.swift
//  CRUICoreView
//
//  Created by YunWu01 on 2024/2/2.
//

import UIKit
import RxSwift
import RxCocoa
import CRUICore
import CoreLocation
import MapKit

public enum LocationFor {
    case locationForSelect
    case locationForShow
}

public typealias SelectedLocationHandler = ((_ latitude: Double, _ longitude: Double, _ desc: String) -> Void)

public class LocationViewController: UIViewController {

    private var selectedHandler: SelectedLocationHandler?
    
    private let _disposeBag = DisposeBag()
    private let locationManager = CLLocationManager()
    private let topButtonInset: CGFloat = 5
    private let leftButtonInset: CGFloat = 8
    private let geocoder = CLGeocoder()
    private var selectLocation: CLLocation?
    
    private var previousAnnotation: MKPointAnnotation?
    
    lazy var mapView: MKMapView = {
        let v = MKMapView()
        v.delegate = self
        v.mapType = .standard
        
        return v
    }()
    
    lazy var cancelButton: UIButton = {
        let v = UIButton()
        v.titleLabel?.font = UIFont.preferredFont(forTextStyle: .body)
        v.setTitle("取消".innerLocalized(), for: .normal)
        v.contentEdgeInsets = UIEdgeInsets(top: topButtonInset, left: leftButtonInset, bottom: topButtonInset, right: leftButtonInset)
        
        v.rx.tap.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            sself.dismiss(animated: true)
        }).disposed(by: _disposeBag)
        
        return v
    }()
    
    lazy var sendButton: UIButton = {
        let v = UIButton()
        v.titleLabel?.font = UIFont.preferredFont(forTextStyle: .body)
        v.setTitle("发送".innerLocalized(), for: .normal)
        v.backgroundColor = .c0089FF
        v.layer.cornerRadius = 5
        v.contentEdgeInsets = UIEdgeInsets(top: topButtonInset, left: leftButtonInset, bottom: topButtonInset, right: leftButtonInset)
        
        v.rx.tap.subscribe(onNext: { [weak self] _ in
            guard let sself = self else { return }
            
            sself.reverseGeocodeLocation()
            sself.dismiss(animated: true)
        }).disposed(by: _disposeBag)
        
        return v
    }()
    
    private var locationFor: LocationFor!
    private var latitude: Double!
    private var longitude: Double!
    private var name: String!
    private var addr: String!
    
    public init(locationFor: LocationFor = .locationForSelect, latitude: Double = 0, longitude: Double = 0, name: String? = nil, addr: String? = nil) {
        super.init(nibName: nil, bundle: nil)
        self.locationFor = locationFor
        self.latitude = latitude
        self.longitude = longitude
        self.name = name
        self.addr = addr
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    public override func viewDidLoad() {
        super.viewDidLoad()
        
        view.backgroundColor = .white
        
        initView()
        requestLocationAuthorization()
        startLocationUpdates()
    }
    
    // 设置其它选项
    public func selectedLocation(handler: SelectedLocationHandler?) {
        self.selectedHandler = handler
    }

    private func initView() {
        view.addSubview(mapView)
        mapView.snp.makeConstraints { make in
            make.edges.equalTo(view)
        }
        
        if locationFor == .locationForShow {
            replaceSystemBackBarButtonItem()
            navigationItem.title = name
        }
        
        if locationFor == .locationForSelect {
            // 添加点击手势识别器
            let tapGesture = UITapGestureRecognizer(target: self, action: #selector(handleTap(_:)))
            mapView.addGestureRecognizer(tapGesture)
            
            view.addSubview(cancelButton)
            cancelButton.snp.makeConstraints { make in
                make.top.equalTo(view.safeAreaLayoutGuide.snp.top).offset(16)
                make.leading.equalToSuperview().offset(15)
            }
            
            view.addSubview(sendButton)
            sendButton.snp.makeConstraints { make in
                make.centerY.equalTo(cancelButton)
                make.trailing.equalToSuperview().offset(-15)
            }
        }
    }
    
    @objc
    private func handleTap(_ gestureRecognizer: UITapGestureRecognizer) {
        if gestureRecognizer.state == .ended {
            let location = gestureRecognizer.location(in: mapView)
            let coordinate = mapView.convert(location, toCoordinateFrom: mapView)

            // 创建大头针注释
            let annotation = MKPointAnnotation()
            annotation.coordinate = coordinate

            removePreviousAnnotation()
            // 添加大头针注释到地图视图
            mapView.addAnnotation(annotation)
            previousAnnotation = annotation
            selectLocation = CLLocation(latitude: coordinate.latitude, longitude: coordinate.longitude)
        }
    }
    
    private func showLocationAnnotation() {
        let validLocation = CLLocation(latitude: latitude, longitude: longitude)
        self.mapView.setCenter(validLocation.coordinate, animated: false)
        let regionRadius: CLLocationDistance = 50 // meters
        let coordinateRegion = MKCoordinateRegion(center: validLocation.coordinate, latitudinalMeters: regionRadius, longitudinalMeters: regionRadius)
        self.mapView.setRegion(coordinateRegion, animated: false)
        selectLocation = validLocation
        
        // 创建大头针注释
        let coordinate = validLocation.coordinate
        let annotation = MKPointAnnotation()
        annotation.coordinate = coordinate

        removePreviousAnnotation()
        // 添加大头针注释到地图视图
        mapView.addAnnotation(annotation)
        previousAnnotation = annotation
        selectLocation = CLLocation(latitude: coordinate.latitude, longitude: coordinate.longitude)
    }
    
    private func removePreviousAnnotation() {
        guard let previousAnnotation = previousAnnotation else { return }

        // 从地图中移除上一个标注
        mapView.removeAnnotation(previousAnnotation)

        // 清空上一个标注的引用
        self.previousAnnotation = nil
    }
    
    private func requestLocationAuthorization() {
        // 检查定位服务是否可用
        if CLLocationManager.locationServicesEnabled() {
            switch CLLocationManager.authorizationStatus() {
            case .notDetermined:
                // 请求授权
                locationManager.requestWhenInUseAuthorization()
            case .restricted, .denied:
                // 提示用户打开定位权限
                showLocationPermissionAlert()
            case .authorizedAlways, .authorizedWhenInUse:
                // 已授权，可以开始使用定位功能
                startLocationUpdates()
            default:
                break
            }
        } else {
            // 定位服务不可用
            showLocationServiceAlert()
        }
    }
    
    private func startLocationUpdates() {
        // 开始定位更新
        mapView.showsUserLocation = true
        mapView.userTrackingMode = .follow
        
        if locationFor == .locationForShow {
            showLocationAnnotation()
        }
    }
    
    func searchNearbyPlaces(coordinate: CLLocationCoordinate2D) {
        let searchRequest = MKLocalSearch.Request()
        searchRequest.naturalLanguageQuery = "restaurant" // 搜索关键词，这里以餐厅为例
        searchRequest.region = mapView.region // 使用地图的当前显示区域作为搜索区域

        let localSearch = MKLocalSearch(request: searchRequest)
        localSearch.start { (response, error) in
            guard let mapItems = response?.mapItems else {
                // 处理搜索错误
                print("Error searching nearby places: \(error?.localizedDescription ?? "")")
                return
            }

            // 处理搜索结果
            for mapItem in mapItems {
                print("Name: \(mapItem.name ?? ""), Location: \(mapItem.placemark.coordinate)")
            }
        }
    }
    
    private func showLocationServiceAlert() {
        // 显示定位服务不可用的提示
        let alert = UIAlertController(title: "Location Services Disabled", message: "Please enable Location Services in Settings", preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
    }

    private func showLocationPermissionAlert() {
        // 显示定位权限被拒绝的提示
        let alert = UIAlertController(title: "Location Permission Denied", message: "Please grant location permission in Settings", preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
        present(alert, animated: true, completion: nil)
    }
    
    private func reverseGeocodeLocation() {
        // 检查位置数据是否有效
        guard let validLocation = selectLocation else {
            return
        }
        
        // 进行地理反编码
        geocoder.reverseGeocodeLocation(validLocation) { [weak self] (placemarks, error) in
            if let error = error {
                print("地理反编码错误: \(error.localizedDescription)")
                return
            }
            
            guard let placemark = placemarks?.first else {
                print("找不到该位置的地理信息")
                return
            }
            
            // 从 placemark 中获取地理信息数据
            let address = placemark.name ?? ""
            let street = placemark.thoroughfare ?? ""
            let city = placemark.locality ?? ""
            let state = placemark.administrativeArea ?? ""
            let country = placemark.country ?? ""
            
            // 打印地理信息
            print("地址: \(address)")
            print("街道: \(street)")
            print("城市: \(city)")
            print("州/省份: \(state)")
            print("国家: \(country)")
            
            let descStr = String.init(data: try! JSONSerialization.data(withJSONObject: ["name": address, "addr": country + state + city + street]),
                                      encoding: .utf8)!
            let coordinate = validLocation.coordinate
            self?.selectedHandler?(coordinate.latitude, coordinate.longitude, descStr)
        }
    }

}

// 实现MKMapViewDelegate中的方法来处理用户选择的位置
extension LocationViewController: MKMapViewDelegate {
    
    public func mapView(_ mapView: MKMapView, didUpdate userLocation: MKUserLocation) {
        let location = userLocation.location
        
        guard selectLocation == nil else {
            return
        }
        
        // 检查位置数据是否有效
        guard let validLocation = location else {
            return
        }
        
        guard locationFor == .locationForSelect else { return }
        
        self.mapView.setCenter(validLocation.coordinate, animated: false)
        let regionRadius: CLLocationDistance = 1000 // meters
        let coordinateRegion = MKCoordinateRegion(center: validLocation.coordinate, latitudinalMeters: regionRadius, longitudinalMeters: regionRadius)
        self.mapView.setRegion(coordinateRegion, animated: false)
        selectLocation = validLocation
    }
    
    /*
    public func mapView(_ mapView: MKMapView, didSelect view: MKAnnotationView) {
        // 用户选择了一个地点的大头针
        guard let annotation = view.annotation else { return }
        
        let title = annotation.title
        // 获取选定位置的经纬度信息
        let latitude = annotation.coordinate.latitude
        let longitude = annotation.coordinate.longitude
        
        
    }
    */
    
    // 实现MKMapViewDelegate中的方法，以便自定义用户位置的标注视图
    public func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
        if annotation is MKUserLocation {
            let annotationView = mapView.view(for: annotation) ?? MKAnnotationView(annotation: annotation, reuseIdentifier: nil)
            annotationView.image = UIImage(systemName: "location.circle.fill")
            return annotationView
        }
        return nil
    }
}
