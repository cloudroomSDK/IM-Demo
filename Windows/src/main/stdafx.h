#ifndef STDAFX_H
#define STDAFX_H

#include <QtWidgets>
#include <QtNetwork>
#include <QtSvg>

#include <assert.h>
#include <map>
#include <unordered_map>
#include <list>
#include <set>
#include <unordered_set>
#include <string>
#include <memory>
#include <mutex>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif

#define ScreenScale(a) a


#include "CRIMSDK.h"
using namespace CRIMSDK;
#include "CRIM_Manager.h"
#include "CRIM_Defines.h"
#include "CRIM_Defines_Ex.h"

#include "JsonHelper.h"
#include "UICommon.h"
#include "UICommon_img.h"
#include "UICommon_process.h"
#include "QNoNCDialog.h"
#include "CRMsgBox.h"
#include "QWidgetInstallKeyHandler.h"
#include "QWidgetResizeHandler.h"
#include "QClickedLabel.h"

#include "UiActionMgr.h"
#include "ImIconMgr.h"
#include "DemoSvrWebAPI.h"

#include "App.h"
#include "ChatPublic.h"
#include "ImCallDef.h"

#define  CFG_FILE_NAME "Cfg.ini"

using namespace ChatPublic;

enum E_LOG_LEVEL_DEF {
    LOGLEVEL_TRACE = 0,
    LOGLEVEL_DEBUG,
    LOGLEVEL_WARN,
    LOGLEVEL_ERR,
};
void OutPutLog(E_LOG_LEVEL_DEF level, const char *modlue, const char* strFormat, ...);

#define MainLogDebug(fmt, ...) OutPutLog(LOGLEVEL_DEBUG, "Main", fmt, ##__VA_ARGS__)
#define MainLogWarn(fmt, ...) OutPutLog(LOGLEVEL_WARN, "Main", fmt, ##__VA_ARGS__)
#define MainLogErr(fmt, ...) OutPutLog(LOGLEVEL_ERR, "Main", fmt, ##__VA_ARGS__)

#if defined WIN32 || defined LINUX
#define SHADOW_SIZE 8
#else
#define SHADOW_SIZE 0
#endif


//CRVideoSDK
#include <CRVideoSDK_Def.h>
#include <CRVideoSDK_Objs.h>
#include <CRVideoSDKMain.h>
#include <CRVideoSDKMeeting.h>
using namespace CRBase;
using namespace CRVSDK;
#include "Common.h"
extern CRVideoSDKMain*	g_sdkMain;

#endif //STDAFX_H
