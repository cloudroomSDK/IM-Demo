//
//  LocationView.swift
//  CRUICoreView
//
//  Created by YunWu01 on 2024/2/2.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore
import MapKit

final class LocationView: UIView, ContainerCollectionViewCellDelegate {

    private lazy var nameLabel = UILabel(frame: bounds)
    private lazy var addrLabel = UILabel(frame: bounds)
    
    private lazy var stackView = UIStackView(frame: bounds)

    private lazy var mapView = MKMapView(frame: bounds)
    
    private var controller: LocationController!

    private var stackViewWidthConstraint: NSLayoutConstraint?
    
    private var viewPortWidth: CGFloat = 300
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupSubviews()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupSubviews()
    }

    func prepareForReuse() {
        nameLabel.text = nil
        addrLabel.text = nil
        
        let annotations = mapView.annotations
        mapView.removeAnnotations(annotations)
    }

    func apply(_ layoutAttributes: ChatLayoutAttributes) {
        viewPortWidth = layoutAttributes.layoutFrame.width
        setupSize()
    }

    func setup(with controller: LocationController) {
        self.controller = controller
    }

    func reloadData() {
        UIView.performWithoutAnimation {
            nameLabel.text = controller.name
            addrLabel.text = controller.addr
            
            let coordinate = CLLocationCoordinate2D(latitude: controller.latitude, longitude: controller.longitude)
            mapView.setCenter(coordinate, animated: false)
            let regionRadius: CLLocationDistance = 100 // meters
            let coordinateRegion = MKCoordinateRegion(center: coordinate, latitudinalMeters: regionRadius, longitudinalMeters: regionRadius)
            mapView.setRegion(coordinateRegion, animated: false)
            
            // 创建大头针注释
            let annotation = MKPointAnnotation()
            annotation.coordinate = coordinate
            // 添加大头针注释到地图视图
            mapView.addAnnotation(annotation)
        }
    }

    private func setupSubviews() {
        layoutMargins = .zero
        translatesAutoresizingMaskIntoConstraints = false
        insetsLayoutMarginsFromSafeArea = false
        
        nameLabel.font = .preferredFont(forTextStyle: .callout)
        nameLabel.textColor = .black
        addrLabel.font = .preferredFont(forTextStyle: .footnote)
        addrLabel.textColor = .c999999
        
        mapView.isZoomEnabled = false
        mapView.isScrollEnabled = false
        mapView.isUserInteractionEnabled = false
        
        stackView.spacing = 5
        stackView.axis = .vertical

        addSubview(stackView)
        stackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        stackView.addArrangedSubview(nameLabel)
        stackView.addArrangedSubview(addrLabel)
        stackView.addArrangedSubview(mapView)
        
        mapView.translatesAutoresizingMaskIntoConstraints = false
        nameLabel.translatesAutoresizingMaskIntoConstraints = false
        
        stackViewWidthConstraint = stackView.widthAnchor.constraint(equalToConstant: viewPortWidth - 30)
        stackViewWidthConstraint?.priority = UILayoutPriority(999)
        stackViewWidthConstraint?.isActive = true
        
        let mapViewHeightConstraint = mapView.heightAnchor.constraint(equalToConstant: 88)
        mapViewHeightConstraint.priority = UILayoutPriority(999)
        mapViewHeightConstraint.isActive = true
        
        let tap = UITapGestureRecognizer(target: self, action: #selector(tap))
        isUserInteractionEnabled = true
        addGestureRecognizer(tap)
        
        let longPress = UILongPressGestureRecognizer(target: self, action: #selector(longPress))
        addGestureRecognizer(longPress)
    }
    
    @objc
    private func tap() {
        controller?.action()
    }
    
    @objc
    private func longPress() {
        controller?.longPressAction()
    }

    private func setupSize() {
        UIView.performWithoutAnimation {
            stackViewWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth - 30
            setNeedsLayout()
        }
    }
}
