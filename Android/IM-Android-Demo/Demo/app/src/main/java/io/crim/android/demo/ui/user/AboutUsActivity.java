package io.crim.android.demo.ui.user;

import android.os.Bundle;

import io.crim.android.demo.databinding.ActivityAboutUsBinding;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.utils.Common;

public class AboutUsActivity extends BaseActivity<BaseViewModel, ActivityAboutUsBinding> {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityAboutUsBinding.inflate(getLayoutInflater()));
        sink();

        view.version.setText(Common.getAppVersionName(this));
        view.update.setOnClickListener(v -> {

        });
    }
}
