package io.crim.android.ouicore.widget;

import android.os.Bundle;

import androidx.fragment.app.FragmentActivity;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.im.IM;
import io.crim.android.ouicore.utils.L;

/**
 * 模块调试时使用
 */
public class DebugActivity extends FragmentActivity implements OnBase<String> {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        IM.initSdk(getApplication());

        LoginCertificate loginCertificate = new LoginCertificate();
        loginCertificate.userID = "a@qq.com";
        loginCertificate.nickName = "a@qq.com";
        loginCertificate.imToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJVSUQiOiJhQHFxLmNvbSIsIlBsYXRmb3JtIjoiQW5kcm9pZCIsImV4cCI6MTk2ODYzOTQxOSwibmJmIjoxNjUzMjc5NDE5LCJpYXQiOjE2NTMyNzk0MTl9.XAfKwQ-KDhLBn96FYgH52-OWEZjN3buCgiLxn6wlAhg";
        loginCertificate.cache(this);
        CRIMClient.getInstance().login(this, loginCertificate.userID, loginCertificate.imToken,"","");
    }


    @Override
    public void onError(int code, String error) {
        L.e("登录失败---" + error);

    }

    @Override
    public void onSuccess(String data) {

    }
}
