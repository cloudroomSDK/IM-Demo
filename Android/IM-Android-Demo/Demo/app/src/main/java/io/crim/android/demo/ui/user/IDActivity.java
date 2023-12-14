package io.crim.android.demo.ui.user;

import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;

import androidx.annotation.Nullable;
import io.crim.android.demo.databinding.ActivityIdBinding;
import io.crim.android.demo.vm.IDVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.utils.Common;

/**
 * Created by zjw on 2023/9/14.
 */
public class IDActivity extends BaseActivity<IDVM, ActivityIdBinding> {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        bindVM(IDVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityIdBinding.inflate(getLayoutInflater()));

        String id = getIntent().getStringExtra("ID");
        if (id != null) {
            view.tvID.setText(id);
        }
        view.tvCopy.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (id != null && !TextUtils.isEmpty(id)) {
                    Common.copy(id);
                    toast(getString(io.crim.android.ouicore.R.string.copy_succ));
                }
            }
        });
    }
}
