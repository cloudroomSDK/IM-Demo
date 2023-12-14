package io.crim.android.ouicore.im;


import android.app.Application;
import android.content.Context;
import android.util.Log;

import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.L;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.Platform;

public class IM {
    public static void initSdk(Application app) {
        L.e("App", "---IM--initSdk==="+ Constant.getSdkServerUrl());
        ///IM 初始化
        CRIMClient.getInstance().initSDK(app,
            Platform.ANDROID,
            Constant.getSdkServerUrl(),
            Constant.getImWsUrl(), getStorageDir(), 6,
            true,
            Constant.File_DIR,
            false,IMEvent.getInstance().connListener);
        IMEvent.getInstance().init();
    }

    //存储路径
    public static String getStorageDir() {
        return BaseApp.inst().getExternalFilesDir(getAppName(BaseApp.inst())).getAbsolutePath();
//        return BaseApp.inst().getFilesDir().getAbsolutePath();
    }

    public static String getAppName(Context context) {
        if (context == null) {
            Log.w("App", "getAppName context is null");
            return "";
        }
        String packageName = context.getPackageName();
        String[] strs = packageName.split("\\.");
        return strs.length == 0 ? packageName : strs[strs.length - 1];
    }
}
