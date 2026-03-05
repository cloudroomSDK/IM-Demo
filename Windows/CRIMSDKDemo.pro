QT += core gui widgets network svg multimedia
CONFIG += precompile_header qt
CONFIG += skip_target_version_ext
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#DEFINES += _GLIBCXX_USE_CXX11_ABI=0

TEMPLATE = app
TARGET = CRIMSDKDemo

# Winodws
msvc {
    contains(QT_ARCH, i386): ARCHITECTURE = x86
    else: ARCHITECTURE = $$QT_ARCH

    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
    LIBS += -L$$PWD/CRIMSDK/libs/$$ARCHITECTURE/ -lCRIMSDK -lUser32 \
            -L$$PWD/CRVideoSDK/lib/$$ARCHITECTURE/ -lCRBase -lCRVideoSDKCpp

    win32:RC_FILE = $$PWD/src/CRIMSDKDemo.rc

    QMAKE_CXXFLAGS *=  /wd"4100"
    contains (QMAKE_CXXFLAGS_WARN_ON, -w34100) : QMAKE_CXXFLAGS_WARN_ON -= -w34100

    SOURCES += $$PWD/src/ui_common/UICommon_process_win32.cpp
}

# MacOS
macx {
    # 启用通用二进制构建
    QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    QMAKE_OSX_ARCHITECTURES = x86_64 arm64

    # 设置 macOS 部署目标（建议 10.15 或更高）
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

    QMAKE_LFLAGS += -framework AppKit
    QMAKE_LFLAGS += -framework Foundation
    QMAKE_LFLAGS += -framework Security

    INCLUDEPATH += $$PWD/CRVideoSDK/lib/libCRVideoSDKCpp.xcframework/macos-arm64_x86_64/Headers
    LIBS += $$PWD/CRIMSDK/libs/imcore.dylib -L$$PWD/CRIMSDK/libs -lCRIMSDK \
            -L$$PWD/CRVideoSDK/lib/libCRVideoSDKCpp.xcframework/macos-arm64_x86_64 -lCRVideoSDKCpp

    QMAKE_INFO_PLIST = $$PWD/src/Info.plist
    DEFINES += MAC
    QMAKE_POST_LINK += $$PWD/src/installsDK_mac.sh

    SOURCES += $$PWD/src/ui_common/UICommon_process_mac.mm \
                $$PWD/src/ui_controls/QNoNCDialog_mac.mm
}

#Linux
linux {
    ARCHITECTURE=$$QMAKE_HOST.arch
    contains(QMAKE_HOST.arch, aarch64) || linux-aarch64-gnu-g++{
    ARCHITECTURE=aarch64
    }
    contains(QMAKE_HOST.arch, arm) || linux-arm-gnueabihf-g++{
    ARCHITECTURE=arm-linux-gnueabihf
    }
    QT += dbus
    DEFINES += LINUX
    LIBS += -L$$PWD/CRIMSDK/libs/$$ARCHITECTURE/ -lCRIMSDK \
            -L$$PWD/CRVideoSDK/lib/$$ARCHITECTURE/ -lCRBase -lCRVideoSDKCpp

    QMAKE_LFLAGS += -Wl,-rpath,$$PWD/:$$PWD/CRIMSDK/libs/$$ARCHITECTURE:$$PWD/CRVideoSDK/lib/$$ARCHITECTURE
    SOURCES += $$PWD/src/ui_common/UICommon_process_linux.cpp
}

DESTDIR = $$PWD/bin/$$ARCHITECTURE
PRECOMPILED_HEADER = $$PWD/src/main/stdafx.h

INCLUDEPATH += $$PWD/src \
    $$PWD/CRIMSDK/include \
    $$PWD/CRVideoSDK/include \
    $$PWD/src/IMSDKQtWrapper \
    $$PWD/src/ui_controls \
    $$PWD/src/ui_common \
    $$PWD/src/ui_controls/Chat \
    $$PWD/src/main \
    $$PWD/src/MessageMgr \
    $$PWD/src/MessageMgr/ConversationUI \
    $$PWD/src/SearchUI \
    $$PWD/src/GroupsMgr \
    $$PWD/src/FriendsMgr \
    $$PWD/src/Audio \
    $$PWD/src/CallMgr \
    $$PWD/src/MeetingMgr
	
SOURCES += $$PWD/src/IMSDKQtWrapper/ChatPublic.cpp \
    $$PWD/src/IMSDKQtWrapper/FaceHelper.cpp \
    $$PWD/src/IMSDKQtWrapper/CRIM_Callback.cpp \
    $$PWD/src/IMSDKQtWrapper/CRIM_DataConv.cpp \
    $$PWD/src/IMSDKQtWrapper/CRIM_Defines.cpp \
    $$PWD/src/IMSDKQtWrapper/CRIM_Defines_Ex.cpp \
    $$PWD/src/IMSDKQtWrapper/CRIM_Manager.cpp \
    $$PWD/src/ui_controls/CRDialogBtns.cpp \
    $$PWD/src/ui_controls/CRLineEdit.cpp \
    $$PWD/src/ui_controls/CRTxtEditor.cpp \
    $$PWD/src/ui_controls/QActionStyleBtn.cpp \
    $$PWD/src/ui_controls/QButtonWithGif.cpp \
    $$PWD/src/ui_controls/QClickedLabel.cpp \
    $$PWD/src/ui_controls/QElidedLabel.cpp \
    $$PWD/src/ui_controls/QFindEdit.cpp \
    $$PWD/src/ui_controls/QGifLabel.cpp \
    $$PWD/src/ui_controls/QNoNCDialog.cpp \
    $$PWD/src/ui_controls/CRMsgBox.cpp \
    $$PWD/src/ui_controls/QScrollAreaDragHandler.cpp \
    $$PWD/src/ui_controls/QSmoothScrollbar.cpp \
    $$PWD/src/ui_controls/QSuspendedScrollBar.cpp \
    $$PWD/src/ui_controls/QWidgetInstallKeyHandler.cpp \
    $$PWD/src/ui_controls/QWidgetResizeHandler.cpp \
    $$PWD/src/ui_controls/WidgetQssLayAble.cpp \
    $$PWD/src/ui_controls/QImageLabel.cpp \
    $$PWD/src/ui_controls/Chat/ChatInputEdit.cpp \
    $$PWD/src/ui_controls/Chat/ChatItemDelegate.cpp \
    $$PWD/src/ui_controls/Chat/ChatItemObject.cpp \
    $$PWD/src/ui_controls/Chat/ChatItemObject_File.cpp \
    $$PWD/src/ui_controls/Chat/ChatItemObject_Html.cpp \
    $$PWD/src/ui_controls/Chat/ChatModel.cpp \
    $$PWD/src/ui_controls/Chat/ChatStyle.cpp \
    $$PWD/src/ui_controls/Chat/ChatWidget.cpp \
    $$PWD/src/ui_controls/Chat/DocumentHighLighter.cpp \
    $$PWD/src/ui_controls/Chat/DocumentTextObject.cpp \
    $$PWD/src/ui_controls/Chat/mypicswidget.cpp \
    $$PWD/src/ui_common/Common.cpp \
    $$PWD/src/ui_common/UICommon.cpp \
    $$PWD/src/ui_common/UICommon_img.cpp \
    $$PWD/src/ui_common/QObjectGuard.cpp \
    $$PWD/src/ui_common/JsonHelper.cpp \
    $$PWD/src/MessageMgr/SessionItemWidget.cpp \
    $$PWD/src/MessageMgr/SessionListWidget.cpp \
    $$PWD/src/MessageMgr/DownloadHttpFile.cpp \
    $$PWD/src/MessageMgr/ImDownloadFile.cpp \
    $$PWD/src/MessageMgr/ImGroupMenuCreator.cpp \
    $$PWD/src/MessageMgr/MessageMgrPage.cpp \
    $$PWD/src/MessageMgr/ConversationUI/ConversationBaseWidget.cpp \
    $$PWD/src/MessageMgr/ConversationUI/Conversation_C2C.cpp \
    $$PWD/src/MessageMgr/ConversationUI/Conversation_Group.cpp \
    $$PWD/src/MessageMgr/ConversationUI/DlgAddGroupMember.cpp \
    $$PWD/src/MessageMgr/ConversationUI/DlgFastSelMember.cpp \
    $$PWD/src/MessageMgr/ConversationUI/GroupMemberList.cpp \
    $$PWD/src/MessageMgr/ConversationUI/IMFaceWidget.cpp \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget.cpp \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget_His.cpp \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget_Recv.cpp \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget_Search.cpp \
    $$PWD/src/MessageMgr/ConversationUI/MsgSendContainer.cpp \
    $$PWD/src/MessageMgr/ConversationUI/SelectMembersWidget.cpp \
    $$PWD/src/MessageMgr/AutoDelMessageMgr.cpp \
    $$PWD/src/SearchUI/GlobalIMSearchDlg.cpp \
    $$PWD/src/SearchUI/GlobalSearchRsltDlg.cpp \
    $$PWD/src/SearchUI/GlobalSearch_defs.cpp \
    $$PWD/src/FriendsMgr/AddFriendDlg.cpp \
    $$PWD/src/FriendsMgr/FriendDetailWidget.cpp \
    $$PWD/src/FriendsMgr/FriendItemWidget.cpp \
    $$PWD/src/FriendsMgr/FriendsMgrPage.cpp \
    $$PWD/src/FriendsMgr/NewFriendMsgPage.cpp \
    $$PWD/src/FriendsMgr/SendFriendRequestDlg.cpp \
    $$PWD/src/GroupsMgr/DlgNewGroup.cpp \
    $$PWD/src/GroupsMgr/GroupDetailWidget.cpp \
    $$PWD/src/GroupsMgr/GroupItemWidget.cpp \
    $$PWD/src/GroupsMgr/GroupsMgrPage.cpp \
    $$PWD/src/main/AboutDlg.cpp \
    $$PWD/src/main/App.cpp \
    $$PWD/src/main/AppSetting.cpp \
    $$PWD/src/main/CSystemTrayIcon.cpp \
    $$PWD/src/main/DemoSvrWebAPI.cpp \
    $$PWD/src/main/EditMyInfoDlg.cpp \
    $$PWD/src/main/ImIconMgr.cpp \
    $$PWD/src/main/LoginDlg.cpp \
    $$PWD/src/main/LoginPage.cpp \
    $$PWD/src/main/LoginSettingPage.cpp \
    $$PWD/src/main/main.cpp \
    $$PWD/src/main/MainDialog.cpp \
    $$PWD/src/main/stdafx.cpp \
    $$PWD/src/main/UiActionMgr.cpp \
    $$PWD/src/main/UserInfoPage.cpp \
    $$PWD/src/main/UserStatusMgr.cpp \
    $$PWD/src/CallMgr/ImCallDef.cpp \
    $$PWD/src/CallMgr/ImCallinDlg.cpp \
    $$PWD/src/CallMgr/ImCallMgr.cpp \
    $$PWD/src/CallMgr/ImCalloutDlg.cpp \
    $$PWD/src/Audio/AudioMgr.cpp \
    $$PWD/src/MeetingMgr/CustomRenderWidget.cpp \
    $$PWD/src/MeetingMgr/KVideoUI.cpp \
    $$PWD/src/MeetingMgr/MeetingMainDialog.cpp \
    $$PWD/src/MeetingMgr/KeepAspectRatioDrawer.cpp \

HEADERS += $$PWD/src/IMSDKQtWrapper/ChatPublic.h \
    $$PWD/src/IMSDKQtWrapper/FaceHelper.h \
    $$PWD/src/IMSDKQtWrapper/CRIM_DataConv.h \
    $$PWD/src/IMSDKQtWrapper/CRIM_Defines.h \
    $$PWD/src/IMSDKQtWrapper/CRIM_Defines_Ex.h \
    $$PWD/src/IMSDKQtWrapper/CRIM_Callback.h \
    $$PWD/src/IMSDKQtWrapper/CRIM_Manager.h \
    $$PWD/CRIMSDK/include/CRIMCallbacker.h \
    $$PWD/CRIMSDK/include/CRIMDefine.h \
    $$PWD/CRIMSDK/include/CRIMSDK.h \
    $$PWD/src/ui_controls/QSmoothScrollbar.h \
    $$PWD/src/ui_controls/QSuspendedScrollBar.h \
    $$PWD/src/ui_controls/WidgetQssLayAble.h \
    $$PWD/src/ui_controls/QImageLabel.h \
    $$PWD/src/ui_controls/QWidgetInstallKeyHandler.h \
    $$PWD/src/ui_controls/QWidgetResizeHandler.h \
    $$PWD/src/ui_controls/QFindEdit.h \
    $$PWD/src/ui_controls/CRLineEdit.h \
    $$PWD/src/ui_controls/QElidedLabel.h \
    $$PWD/src/ui_controls/QClickedLabel.h \
    $$PWD/src/ui_controls/CRDialogBtns.h \
    $$PWD/src/ui_controls/CRTxtEditor.h \
    $$PWD/src/ui_controls/QScrollAreaDragHandler.h \
    $$PWD/src/ui_controls/QActionStyleBtn.h \
    $$PWD/src/ui_controls/QGifLabel.h \
    $$PWD/src/ui_controls/QButtonWithGif.h \
    $$PWD/src/ui_controls/QNoNCDialog.h \
    $$PWD/src/ui_controls/CRMsgBox.h \
    $$PWD/src/ui_controls/Chat/ChatInputEdit.h \
    $$PWD/src/ui_controls/Chat/ChatItemDelegate.h \
    $$PWD/src/ui_controls/Chat/ChatItemObject.h \
    $$PWD/src/ui_controls/Chat/ChatItemObject_File.h \
    $$PWD/src/ui_controls/Chat/ChatItemObject_Html.h \
    $$PWD/src/ui_controls/Chat/ChatModel.h \
    $$PWD/src/ui_controls/Chat/ChatStyle.h \
    $$PWD/src/ui_controls/Chat/ChatWidget.h \
    $$PWD/src/ui_controls/Chat/DocumentHighLighter.h \
    $$PWD/src/ui_controls/Chat/DocumentTextObject.h \
    $$PWD/src/ui_controls/Chat/mypicswidget.h \
    $$PWD/src/ui_common/UICommon_img.h \
    $$PWD/src/ui_common/UICommon_process.h \
    $$PWD/src/ui_common/Common.h \
    $$PWD/src/ui_common/UICommon.h \
    $$PWD/src/ui_common/QObjectGuard.h \
    $$PWD/src/ui_common/JsonHelper.h \
    $$PWD/src/MessageMgr/ImGroupMenuCreator.h \
    $$PWD/src/MessageMgr/DownloadHttpFile.h \
    $$PWD/src/MessageMgr/ImDownloadFile.h \
    $$PWD/src/MessageMgr/SessionItemWidget.h \
    $$PWD/src/MessageMgr/SessionListWidget.h \
    $$PWD/src/MessageMgr/MessageMgrPage.h \
    $$PWD/src/MessageMgr/ConversationUI/SelectMembersWidget.h \
    $$PWD/src/MessageMgr/ConversationUI/Conversation_C2C.h \
    $$PWD/src/MessageMgr/ConversationUI/Conversation_Group.h \
    $$PWD/src/MessageMgr/ConversationUI/ConversationBaseWidget.h \
    $$PWD/src/MessageMgr/ConversationUI/DlgAddGroupMember.h \
    $$PWD/src/MessageMgr/ConversationUI/DlgFastSelMember.h \
    $$PWD/src/MessageMgr/ConversationUI/GroupMemberList.h \
    $$PWD/src/MessageMgr/ConversationUI/IMFaceWidget.h \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget.h \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget_His.h \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget_Recv.h \
    $$PWD/src/MessageMgr/ConversationUI/IMMsgWidget_Search.h \
    $$PWD/src/MessageMgr/ConversationUI/MsgSendContainer.h \
    $$PWD/src/MessageMgr/AutoDelMessageMgr.h \
    $$PWD/src/SearchUI/GlobalSearch_defs.h \
    $$PWD/src/SearchUI/GlobalIMSearchDlg.h \
    $$PWD/src/SearchUI/GlobalSearchRsltDlg.h \
    $$PWD/src/FriendsMgr/FriendsMgrPage.h \
    $$PWD/src/FriendsMgr/AddFriendDlg.h \
    $$PWD/src/FriendsMgr/NewFriendMsgPage.h \
    $$PWD/src/FriendsMgr/FriendItemWidget.h \
    $$PWD/src/FriendsMgr/FriendDetailWidget.h \
    $$PWD/src/FriendsMgr/SendFriendRequestDlg.h \
    $$PWD/src/GroupsMgr/GroupsMgrPage.h \
    $$PWD/src/GroupsMgr/DlgNewGroup.h \
    $$PWD/src/GroupsMgr/GroupDetailWidget.h \
    $$PWD/src/GroupsMgr/GroupItemWidget.h \
    $$PWD/src/main/stdafx.h \
    $$PWD/src/main/App.h \
    $$PWD/src/main/AccountInfo.h \
    $$PWD/src/main/Version.h \
    $$PWD/src/main/AboutDlg.h \
    $$PWD/src/main/ImIconMgr.h \
    $$PWD/src/main/LoginDlg.h \
    $$PWD/src/main/LoginPage.h \
    $$PWD/src/main/LoginSettingPage.h \
    $$PWD/src/main/MainDialog.h \
    $$PWD/src/main/CSystemTrayIcon.h \
    $$PWD/src/main/AppSetting.h \
    $$PWD/src/main/DemoSvrWebAPI.h \
    $$PWD/src/main/EditMyInfoDlg.h \
    $$PWD/src/main/UiActionMgr.h \
    $$PWD/src/main/UserInfoPage.h \
    $$PWD/src/main/UserStatusMgr.h \
    $$PWD/src/CallMgr/ImCallDef.h \
    $$PWD/src/CallMgr/ImCallinDlg.h \
    $$PWD/src/CallMgr/ImCallMgr.h \
    $$PWD/src/CallMgr/ImCalloutDlg.h \
    $$PWD/src/Audio/AudioMgr.h \
    $$PWD/src/MeetingMgr/CustomRenderWidget.h \
    $$PWD/src/MeetingMgr/KVideoUI.h \
    $$PWD/src/MeetingMgr/MeetingMainDialog.h \
    $$PWD/src/MeetingMgr/KeepAspectRatioDrawer.h \

FORMS += $$PWD/src/ui_controls/standardTitleBar.ui \
    $$PWD/src/ui_controls/CRDialogBtns.ui \
    $$PWD/src/MessageMgr/MessageMgrPage.ui \
    $$PWD/src/MessageMgr/SessionItemWidget.ui \
    $$PWD/src/MessageMgr/ConversationUI/Conversation_C2C.ui \
    $$PWD/src/MessageMgr/ConversationUI/Conversation_Group.ui \
    $$PWD/src/MessageMgr/ConversationUI/DlgAddGroupMember.ui \
    $$PWD/src/MessageMgr/ConversationUI/GroupHostItemWidget.ui \
    $$PWD/src/MessageMgr/ConversationUI/MsgSendContainer.ui \
    $$PWD/src/MessageMgr/ConversationUI/UnreadTipsWidget.ui \
    $$PWD/src/MessageMgr/ConversationUI/SelectMemberRsltWidget.ui \
    $$PWD/src/MessageMgr/ConversationUI/SelectMembersWidget.ui \
    $$PWD/src/SearchUI/GlobalIMSearchDlg.ui \
    $$PWD/src/SearchUI/GlobalSearchRsltDlg.ui \
    $$PWD/src/FriendsMgr/FriendsMgrPage.ui \
    $$PWD/src/FriendsMgr/AddFriendDlg.ui \
    $$PWD/src/FriendsMgr/NewFriendMsgItem.ui \
    $$PWD/src/FriendsMgr/NewFriendMsgPage.ui \
    $$PWD/src/FriendsMgr/FriendItemWidget.ui \
    $$PWD/src/FriendsMgr/FriendDetailWidget.ui \
    $$PWD/src/FriendsMgr/SendFriendRequestDlg.ui \
    $$PWD/src/GroupsMgr/GroupsMgrPage.ui \
    $$PWD/src/GroupsMgr/DlgNewGroup.ui \
    $$PWD/src/GroupsMgr/GroupDetailWidget.ui \
    $$PWD/src/GroupsMgr/GroupItemWidget.ui \
    $$PWD/src/main/LoginDlg.ui \
    $$PWD/src/main/LoginPage.ui \
    $$PWD/src/main/LoginSettingPage.ui \
    $$PWD/src/main/MainDialog.ui \
    $$PWD/src/main/AboutDlg.ui \
    $$PWD/src/main/AppSetting.ui \
    $$PWD/src/main/EditMyInfoDlg.ui \
    $$PWD/src/main/UserInfoPage.ui \
    $$PWD/src/CallMgr/ImCallinDlg.ui \
    $$PWD/src/CallMgr/ImCalloutDlg.ui \
    $$PWD/src/MeetingMgr/KVideoUI.ui \
    $$PWD/src/MeetingMgr/MeetingMainDialog.ui \

RESOURCES += \
    $$PWD/src/CRIMSDKDemo.qrc

