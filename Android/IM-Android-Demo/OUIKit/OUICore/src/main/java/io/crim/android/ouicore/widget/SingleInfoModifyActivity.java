package io.crim.android.ouicore.widget;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;

import java.io.Serializable;

import io.crim.android.ouicore.R;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.databinding.ActivitySingleModifyBinding;

public class SingleInfoModifyActivity extends BaseActivity<BaseViewModel, ActivitySingleModifyBinding> {
    public static final String SINGLE_INFO_MODIFY_DATA = "single_info_modify_data";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivitySingleModifyBinding.inflate(getLayoutInflater()));
        sink();
        SingleInfoModifyData singleInfoModifyData = (SingleInfoModifyData) getIntent().getSerializableExtra(SINGLE_INFO_MODIFY_DATA);
        view.title.setText(singleInfoModifyData.title);
        view.description.setText(singleInfoModifyData.description);
        view.avatar.load(singleInfoModifyData.avatarUrl);
        view.editText.setText(singleInfoModifyData.editT);
        view.editText.post(() -> view.editText.requestFocus());
    }

    public void complete(View v) {
        String name = view.editText.getText().toString();
        if (TextUtils.isEmpty(name)){
            toast(getString(R.string.no_empty));
        }else {
            setResult(RESULT_OK, new Intent().putExtra(SINGLE_INFO_MODIFY_DATA, name));
            finish();
        }
    }

    public static class SingleInfoModifyData implements Serializable {
        public String title;
        public String description;
        public String avatarUrl;
        public String editT;
    }
}
