Pod::Spec.new do |s|
  s.name             = 'CRUICalling'
  s.version          = '0.0.1'
  s.summary          = '配合CRIMSDK的iOS原生音视频界面'
  s.description      = <<-DESC
  CRIM：iOS版本IM SDK 可以轻松替代第三方IM云服务，打造具备聊天、社交功能的app。
                       DESC

  s.homepage         = 'https://sdk.cloudroom.com/'
  s.license          = { :type => 'MIT', :file => 'LICENSE' }
  s.author           = { 'CRUICore' => 'https://sdk.cloudroom.com/' }
  s.source           = { :git => 'git@github.com:CRIMUI/CRUICalling.git', :tag => s.version.to_s }

  s.ios.deployment_target = '13.0'
  s.swift_version = '5.0'

  s.source_files = 'CRUICalling/*.{swift,h,m}'
  s.resource_bundles = {
    'CRIMUICalling' => ['CRUICalling/CRIMUICalling.bundle/**/*.{storyboard,xib,xcassets,json,imageset,png,strings,mp3}'],
  }

  s.static_framework = true

  s.dependency 'CRIMSDK'
  s.dependency 'SnapKit'
  s.dependency 'RxCocoa'
  s.dependency 'RxSwift'
  s.dependency 'lottie-ios'
#  s.dependency 'LiveKitClient'
  s.dependency 'cloudroomvideo_ios_library'
  s.dependency 'Kingfisher'
  s.dependency 'ProgressHUD'
  s.dependency 'CRUICore'
end
