//
//  LocationQuoteView.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/2/23.
//

import ChatLayout
import Foundation
import UIKit
import CRUICore
import MapKit

final class LocationQuoteView: UIView, ContainerCollectionViewCellDelegate {

    private lazy var titleLabel = UILabel(frame: bounds)
    
    private lazy var stackView = UIStackView(frame: bounds)

    private lazy var mapView = MKMapView(frame: bounds)
    
    private lazy var nameLabel = UILabel(frame: bounds)
    
    private var controller: LocationController!

    private var stackViewWidthConstraint: NSLayoutConstraint?
    
    private var imageWidthConstraint: NSLayoutConstraint?

    private var imageHeightConstraint: NSLayoutConstraint?

    private var viewPortWidth: CGFloat = 300
    
    private let quoteImageViewWidth: CGFloat = 44
    private let loadingIndicatorWidth: CGFloat = 20
    private let quoteItemSpace: CGFloat = 5
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupSubviews()
    }

    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupSubviews()
    }

    func prepareForReuse() {
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
            titleLabel.text = controller.senderNickname
            nameLabel.text = "[位置]".innerLocalized() + (controller.name ?? "")
            
            let coordinate = CLLocationCoordinate2D(latitude: controller.latitude, longitude: controller.longitude)
            mapView.setCenter(coordinate, animated: false)
            let span = MKCoordinateSpan(latitudeDelta: 0.02, longitudeDelta: 0.02) // 设置缩放级别
            let coordinateRegion = MKCoordinateRegion(center: coordinate, span: span)
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
        
        titleLabel.font = .preferredFont(forTextStyle: .footnote)
        titleLabel.textColor = .black
        
        nameLabel.font = .preferredFont(forTextStyle: .footnote)
        nameLabel.textColor = .black
        
        mapView.isZoomEnabled = false
        mapView.isScrollEnabled = false
        mapView.isUserInteractionEnabled = false
        
        stackView.alignment = .top
        stackView.spacing = quoteItemSpace

        addSubview(stackView)
        stackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: layoutMarginsGuide.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: layoutMarginsGuide.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: layoutMarginsGuide.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: layoutMarginsGuide.trailingAnchor)
        ])
        
        stackView.addArrangedSubview(titleLabel)
        stackView.addArrangedSubview(nameLabel)
        stackView.addArrangedSubview(mapView)
        
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        stackViewWidthConstraint = stackView.widthAnchor.constraint(lessThanOrEqualToConstant: viewPortWidth)
        stackViewWidthConstraint?.priority = UILayoutPriority(999)
        stackViewWidthConstraint?.isActive = true
        
        nameLabel.translatesAutoresizingMaskIntoConstraints = false
        nameLabel.setContentHuggingPriority(.defaultLow, for: .horizontal)

        mapView.translatesAutoresizingMaskIntoConstraints = false
        
        let tap = UITapGestureRecognizer(target: self, action: #selector(tap))
        isUserInteractionEnabled = true
        addGestureRecognizer(tap)
        
        let longPress = UILongPressGestureRecognizer(target: self, action: #selector(longPress))
        addGestureRecognizer(longPress)

        imageWidthConstraint = mapView.widthAnchor.constraint(equalToConstant: quoteImageViewWidth)
        imageWidthConstraint?.priority = UILayoutPriority(999)
        imageWidthConstraint?.isActive = true

        imageHeightConstraint = mapView.heightAnchor.constraint(equalTo: mapView.widthAnchor, multiplier: 1)
        imageHeightConstraint?.priority = UILayoutPriority(999)
        imageHeightConstraint?.isActive = true
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
            stackViewWidthConstraint?.constant = viewPortWidth * StandardUI.maxWidth
            setNeedsLayout()
        }
    }

}
