package io.crim.android.demo.ui.user;

import android.os.Bundle;

import io.crim.android.demo.databinding.ActivityMoreDataBinding;
import io.crim.android.demo.vm.PersonalVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.utils.TimeUtil;

public class MoreDataActivity extends BaseActivity<PersonalVM, ActivityMoreDataBinding> {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(PersonalVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityMoreDataBinding.inflate(getLayoutInflater()));
        init();
    }

    void init() {
        if (vm.userInfo.val() != null) {
            view.avatar.load(vm.userInfo.val().getFaceURL());
            view.nickName.setText(vm.userInfo.val().getNickname());
            view.gender.setText(vm.userInfo.val().getGender() == 1 ? io.crim.android.ouicore.R.string.male
                : io.crim.android.ouicore.R.string.girl);
            long birth = vm.userInfo.val().getBirth();
            if (birth != 0) {
                view.birthday.setText(TimeUtil.getTime(birth ,
                    TimeUtil.yearMonthDayFormat));
            }
            view.phoneTv.setText(vm.userInfo.val().getPhoneNumber());
            view.mailTv.setText(vm.userInfo.val().getEmail());
        }
    }
}
