CUR_PATH=$(cd `dirname $0`; pwd)
QT_PATH=/Applications/Qt/5.15.14
EXAMPLES_ROOT_PATH=${CUR_PATH}/..
APP_PATH=$EXAMPLES_ROOT_PATH/bin/CRIMSDKDemo.app

mkdir -p $APP_PATH/Contents/Frameworks

# imcore.dylib
cp -f $EXAMPLES_ROOT_PATH/CRIMSDK/libs/imcore.dylib $APP_PATH/Contents/Frameworks/
install_name_tool -change imcore.dylib @executable_path/../Frameworks/imcore.dylib "$APP_PATH/Contents/MacOS/CRIMSDKDemo"
# libCRIMSDK.dylib

cp -f $EXAMPLES_ROOT_PATH/CRIMSDK/libs/libCRIMSDK.dylib $APP_PATH/Contents/Frameworks/
install_name_tool -change libCRIMSDK.dylib @executable_path/../Frameworks/libCRIMSDK.dylib "$APP_PATH/Contents/MacOS/CRIMSDKDemo"
#libCRVideoSDKCpp.dylib
cp -f $EXAMPLES_ROOT_PATH/CRVideoSDK/lib/libCRVideoSDKCpp.xcframework/macos-arm64_x86_64/libCRVideoSDKCpp.dylib $APP_PATH/Contents/Frameworks/
install_name_tool -change /usr/local/lib/libCRVideoSDKCpp.dylib @executable_path/../Frameworks/libCRVideoSDKCpp.dylib "$APP_PATH/Contents/MacOS/CRIMSDKDemo"

$QT_PATH/bin/macdeployqt $APP_PATH
