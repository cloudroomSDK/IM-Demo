package io.crim.android.ouicore.im;

import android.widget.Toast;

import io.crim.android.sdk.listener.OnBase;
import io.crim.android.ouicore.base.BaseApp;

public class IMBack<T> implements OnBase<T> {
    @Override
    public void onError(int code, String error) {
        Toast.makeText(BaseApp.inst(), error+"("+code+")",
            Toast.LENGTH_LONG).show();
    }

    @Override
    public void onSuccess(T data) {

    }
}
