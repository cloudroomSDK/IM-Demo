package io.crim.android.ouigroup.ui;

import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;

import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.im.IMBack;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.vm.GroupVM;
import io.crim.android.ouigroup.databinding.ActivitySetMuteBinding;

public class SetMuteActivity extends BaseActivity<GroupVM,ActivitySetMuteBinding> {


    private String uid;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(GroupVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivitySetMuteBinding.inflate(getLayoutInflater()));
        uid=getIntent().getStringExtra(Constant.K_ID);
        view.setGroupVM(vm);
        listener();
    }

    private void listener() {
        view.customizeDay.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                String day=view.customizeDay.getText().toString();
                if (!TextUtils.isEmpty(day))
                    vm.muteStatus.setValue(0);
            }
        });
        view.submit.setOnClickListener(v -> {
            String day=view.customizeDay.getText().toString();
            long seconds=0;
            if(!TextUtils.isEmpty(day)){
               seconds= Integer.valueOf(day)*24*60*60;
            }
            vm.setMemberMute(new IMBack<String>(){
                @Override
                public void onSuccess(String data) {
                    toast(getString(io.crim.android.ouicore.R.string.set_succ));
                    setResult(RESULT_OK);
                    finish();
                }
            },uid,seconds);
        });
    }
}
