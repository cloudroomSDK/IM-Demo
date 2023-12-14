Pod::Spec.new do |s|
  s.name             = 'CRUICore'
  s.version          = '0.0.1'
  s.summary          = '配合CRIMSDK的iOS原生界面'
  s.description      = <<-DESC
  CRIM：iOS版本IM SDK 可以轻松替代第三方IM云服务，打造具备聊天、社交功能的app。
                       DESC

  s.homepage         = 'https://sdk.cloudroom.com/'
  s.license          = { :type => 'MIT', :file => 'LICENSE' }
  s.author           = { 'CRUICore' => 'https://sdk.cloudroom.com/' }
  s.source           = { :git => 'git@github.com:CRIMUI/CRUICore.git', :tag => s.version.to_s }

  s.ios.deployment_target = '13.0'
  s.swift_versions = '5.0'
  
  s.source_files = 'CRUICore/Classes/**/*'
  s.resource_bundles = {
    'CRIMUIResource' => ['CRUICore/Assets/CRIMUIResource.bundle/*.*'],
    'CRIMUIEmoji' => ['CRUICore/Assets/CRIMUIEmoji.bundle/*']
  }

  s.static_framework = true

  s.dependency 'CRIMSDK'
  s.dependency 'SnapKit', '~> 4.0'
  s.dependency 'RxSwift', '~> 6.5'
  s.dependency 'RxCocoa', '~> 6.0'
  s.dependency 'RxKeyboard', '~> 2.0'
  s.dependency 'RxDataSources'
  s.dependency 'Kingfisher', '7.7.0'
  s.dependency 'lottie-ios', '~> 3.0'
  s.dependency 'ZLPhotoBrowser', '4.3.0'
  s.dependency 'ProgressHUD'
  s.dependency 'IQKeyboardManagerSwift'
  s.dependency 'Localize-Swift', '~> 3.2'
  s.dependency 'Alamofire', '4.9.1'
  
end
