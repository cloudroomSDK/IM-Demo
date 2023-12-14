package io.crim.android.demo;


import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;

import com.alibaba.android.arouter.facade.annotation.Route;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import io.crim.android.demo.ui.login.LoginActivity;
import io.crim.android.demo.ui.main.MainActivity;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.utils.Routes;

@Route(path = Routes.Main.SPLASH)
public class SplashActivity extends AppCompatActivity {
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        super.onCreate(savedInstanceState);

        LoginCertificate loginCertificate = LoginCertificate.getCache(this);
        if (null == loginCertificate)
            startActivity(new Intent(this, LoginActivity.class));
        else
            startActivity(new Intent(this, MainActivity.class));
        finish();
    }
}
