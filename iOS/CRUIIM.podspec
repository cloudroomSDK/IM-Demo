Pod::Spec.new do |s|
  s.name             = 'CRUIIM'
  s.version          = '0.0.1'
  s.summary          = '配合CRIMSDK的iOS原生界面'
  s.description      = <<-DESC
  CRIM：iOS版本IM SDK 可以轻松替代第三方IM云服务，打造具备聊天、社交功能的app。
                       DESC

  s.homepage         = 'https://sdk.cloudroom.com/'
  s.license          = { :type => 'MIT', :file => 'LICENSE' }
  s.author           = { 'CRUIIM' => 'https://sdk.cloudroom.com/' }
  s.source           = { :git => 'git@github.com:CRIMUI/CRUIIM.git', :tag => s.version.to_s }

  s.ios.deployment_target = '13.0'

  s.source_files = 'CRUIIM/Classes/**/*'

  s.static_framework = true
  s.dependency 'CRUICore'
  s.dependency 'CRUICoreView'
  s.dependency 'JXSegmentedView'
  s.dependency 'MJRefresh'
  s.dependency 'RxSwift'
  s.dependency 'RxRelay'
  s.dependency 'SnapKit'
  s.dependency 'DifferenceKit'
  s.dependency 'ChatLayout'
  s.dependency 'InputBarAccessoryView'
  s.dependency 'FDFullscreenPopGesture'

end
