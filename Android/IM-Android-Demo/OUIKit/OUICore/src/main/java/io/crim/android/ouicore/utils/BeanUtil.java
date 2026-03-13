package io.crim.android.ouicore.utils;

import android.text.TextUtils;

import io.crim.android.sdk.enums.MsgType;
import io.crim.android.sdk.models.CustomElem;
import io.crim.android.sdk.models.CustomElemData;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.utils.JsonUtil;

/**
 * Created by zjw on 2024/9/14.
 */
public class BeanUtil {

    public static CustomElemData getCustomElemData(Message msg) {
        if (msg != null && msg.getContentType() == MsgType.CUSTOM) {
            CustomElem elem = msg.getCustomElem();
            if (elem != null) {
                String data = elem.getData();
                if (data != null && !TextUtils.isEmpty(data)) {
                    return JsonUtil.toObj(data, CustomElemData.class);
                }
            }
        }
        return null;
    }

}
