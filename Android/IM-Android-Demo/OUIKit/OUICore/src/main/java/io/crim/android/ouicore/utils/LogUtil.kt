package io.crim.android.ouicore.utils

import android.util.Log
import io.crim.android.ouicore.BuildConfig
import io.crim.android.sdk.CRIMClient

/**
 * Created by zjw on 2024/12/27.
 */
object LogUtil {

    fun logcat(content: String) {
        if (BuildConfig.DEBUG) {
            Log.d("eeeeeee", content)
        }
    }

    fun writeLog(content: String){
        CRIMClient.getInstance().writeLog(Constant.LOG_LEVEL, content)
    }
}
