package io.crim.android.ouicore.utils;

import com.yanzhenjie.permission.runtime.Permission;

/**
 * Created by zjw on 2023/12/26.
 */
public class PermissionUtil {

    public static final String READ_MEDIA_IMAGES = "android.permission.READ_MEDIA_IMAGES";
    public static final String POST_NOTIFICATIONS = "android.permission.POST_NOTIFICATIONS";

    public static boolean isSdkInt33() {
        return getSdkInt() >= 33;
    }

    public static int getSdkInt() {
        return android.os.Build.VERSION.SDK_INT;
    }

    public static String getReadImgPermission() {
        String picPermission = Permission.READ_EXTERNAL_STORAGE;
        if (PermissionUtil.isSdkInt33()) {
            picPermission = PermissionUtil.READ_MEDIA_IMAGES;
        }
        return picPermission;
    }

}
