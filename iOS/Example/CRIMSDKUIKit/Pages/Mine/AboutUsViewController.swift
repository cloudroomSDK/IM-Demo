//
//  AboutUsViewController.swift
//  CRIMSDKUIKit_Example
//
//  Created by YunWu01 on 2023/9/8.
//  Copyright © 2023 rentsoft. All rights reserved.
//

import UIKit
import CRUICore

class AboutUsViewController: UIViewController {
    
    private let logoImageView: UIImageView = {
        let v = UIImageView(image: UIImage(named: "logo_image"))
        v.layer.cornerRadius = 5.0
        v.clipsToBounds = true
        
        return v
    }()
    
    private let versionLabel: UILabel = {
        let v = UILabel()
        v.font = UIFont.f14
        v.textColor = UIColor.c8E9AB0
        
        let infoDictionary = Bundle.main.infoDictionary
        let majorVersion = infoDictionary!["CFBundleShortVersionString"] as! String //主程序版本号
        //let minorVersion = infoDictionary!["CFBundleVersion"] as! String //版本号（内部标示）
        v.text = "版本".innerLocalized() + majorVersion
        
        return v
    }()

    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .white
        navigationItem.title = "关于我们".innerLocalized()
        replaceSystemBackBarButtonItem()
        initView()
    }
    

    private func initView() {
        view.addSubview(logoImageView)
        logoImageView.snp.makeConstraints { make in
            make.top.equalTo(self.view.safeAreaLayoutGuide.snp.top).offset(60)
            make.centerX.equalToSuperview()
            make.size.equalTo(180)
        }
        
        view.addSubview(versionLabel)
        versionLabel.snp.makeConstraints { make in
            make.top.equalTo(logoImageView.snp.bottom).offset(20)
            make.centerX.equalToSuperview()
        }
    }
    

}
