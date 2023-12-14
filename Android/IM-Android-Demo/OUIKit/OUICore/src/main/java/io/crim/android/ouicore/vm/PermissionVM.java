package io.crim.android.ouicore.vm;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.base.vm.injection.BaseVM;
import io.crim.android.ouicore.utils.Common;

public class PermissionVM extends BaseVM {

    public Boolean storage;

    public void requestStorage(Common.OnGrantedListener onGranted) {
        Common.permission(BaseApp.inst(), () -> {
            storage = true;
            if (null != onGranted)
                onGranted.onGranted();
        }, storage, Permission.Group.STORAGE);
    }

    public void hasStorage() {
        Common.UIHandler.postDelayed(() -> {
            storage = AndPermission.hasPermissions(BaseApp.inst(),
                Permission.Group.STORAGE);
        }, 300);
    }

}
