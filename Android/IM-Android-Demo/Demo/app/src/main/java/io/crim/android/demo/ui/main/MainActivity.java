package io.crim.android.demo.ui.main;


import android.app.NotificationManager;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.widget.RadioButton;

import com.alibaba.android.arouter.facade.annotation.Route;
import com.alibaba.android.arouter.launcher.ARouter;
import com.igexin.sdk.PushManager;
import com.yanzhenjie.permission.AndPermission;

import androidx.activity.result.ActivityResultLauncher;
import androidx.fragment.app.FragmentTransaction;
import io.crim.android.demo.R;
import io.crim.android.demo.databinding.ActivityMainBinding;
import io.crim.android.demo.ui.login.LoginActivity;
import io.crim.android.demo.ui.user.PersonalFragment;
import io.crim.android.demo.vm.LoginVM;
import io.crim.android.demo.vm.MainVM;
import io.crim.android.ouicontact.ui.fragment.ContactFragment;
import io.crim.android.ouicontact.vm.ContactVM;
import io.crim.android.ouiconversation.ui.fragment.ContactListFragment;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.PermissionUtil;
import io.crim.android.ouicore.utils.Routes;

@Route(path = Routes.Main.HOME)
public class MainActivity extends BaseActivity<MainVM, ActivityMainBinding> implements LoginVM.ViewAction {

    private int mCurrentTabIndex;
    private BaseFragment lastFragment, conversationListFragment, contactFragment,
        personalFragment, appletFragment;
    private ActivityResultLauncher<Intent> resultLauncher = Common.getCaptureActivityLauncher(this);
    private boolean hasShoot = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        init();

        PushManager.getInstance().initialize(this);
        bindVM(MainVM.class);
        vm.fromLogin = getIntent().getBooleanExtra(LoginActivity.FORM_LOGIN, false);
        bindViewDataBinding(ActivityMainBinding.inflate(getLayoutInflater()));
        super.onCreate(savedInstanceState);
        setLightStatus();

        view.setMainVM(vm);

        click();
        listener();
        view.men1.setChecked(true);
    }

    private void init() {
        runOnUiThread(() -> {
//            hasShoot = AndPermission.hasPermissions(MainActivity.this, Permission.CAMERA
//                , Permission.RECORD_AUDIO, PermissionUtil.POST_NOTIFICATIONS);
            hasShoot = AndPermission.hasPermissions(MainActivity.this, PermissionUtil.POST_NOTIFICATIONS);
            Common.permission(MainActivity.this, () -> {
                hasShoot = true;
                //shang ceng ying yong quan xian
//                AndPermission.with(this).overlay().start();
            }, hasShoot,/* Permission.CAMERA, Permission.RECORD_AUDIO, */PermissionUtil.POST_NOTIFICATIONS);

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                NotificationManager notificationManager = getSystemService(NotificationManager.class);
                if (!notificationManager.areNotificationsEnabled()) {
                    Intent intent = new Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS);
                    intent.putExtra(Settings.EXTRA_APP_PACKAGE, getPackageName());
                    startActivity(intent);
                }
            }
        });
    }

    private void listener() {
        vm.totalUnreadMsgCount.observe(this, v -> Common.buildBadgeView(this, view.men1, v));
    }

    private void bindDot() {
        ContactVM contactVM = ((ContactFragment) contactFragment).getVM();
        if (null == contactVM) return;
        contactVM.friendDotNum.observe(this, integer -> {
            int val = integer /*+ contactVM.groupDotNum.val()*/;
            Common.buildBadgeView(this, view.men2, val);
        });
        contactVM.groupDotNum.observe(this, integer -> {
            int val = integer + contactVM.friendDotNum.val();
//            Common.buildBadgeView(this, view.men2, val);
        });
    }

    View.OnClickListener clickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            RadioButton[] menus = new RadioButton[]{view.men1, view.men2, view.men3, view.men4};
            if (v == view.men1) switchFragment(conversationListFragment);
            if (v == view.men2) switchFragment(contactFragment);
            if (v == view.men3) switchFragment(appletFragment);
            if (v == view.men4) switchFragment(personalFragment);
            for (RadioButton menu : menus) {
                menu.setChecked(menu == v);
            }
        }
    };


    private void click() {
        view.men1.setOnClickListener(clickListener);
        view.men2.setOnClickListener(clickListener);
        view.men3.setOnClickListener(clickListener);
        view.men4.setOnClickListener(clickListener);
    }


    @Override
    public void jump() {
        //token过期
        IMUtil.logout(this, LoginActivity.class);
    }

    @Override
    public void err(String msg) {

    }

    @Override
    public void succ(Object o) {

    }

    @Override
    public void initDate() {
        contactFragment =
            (ContactFragment) ARouter.getInstance().build(Routes.Contact.HOME).navigation();
        conversationListFragment =
            (ContactListFragment) ARouter.getInstance().build(Routes.Conversation.CONTACT_LIST).navigation();
        personalFragment = PersonalFragment.newInstance();

        appletFragment =
            (BaseFragment) ARouter.getInstance().build(Routes.Applet.HOME).navigation();

        personalFragment.setPage(4);
        switchFragment(personalFragment);

        if (null != appletFragment) {
            appletFragment.setPage(3);
            switchFragment(appletFragment);
        } else {
            view.men3.setVisibility(View.GONE);
        }
        if (null != contactFragment) {
            contactFragment.setPage(2);
            switchFragment(contactFragment);
        }
        if (null != conversationListFragment) {
            ((ContactListFragment) conversationListFragment).setResultLauncher(resultLauncher);
            conversationListFragment.setPage(1);
            switchFragment(conversationListFragment);
        }
        Common.UIHandler.postDelayed(this::bindDot, 500);
    }


    /**
     * 切换Fragment
     */
    private void switchFragment(BaseFragment fragment) {
        try {
            if (fragment != null && !fragment.isVisible() && mCurrentTabIndex != fragment.getPage()) {
                FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
                if (!fragment.isAdded()) {
                    transaction.add(R.id.fragment_container, fragment);
                }
                if (lastFragment != null) {
                    transaction.hide(lastFragment);
                }
                transaction.show(fragment).commit();
                lastFragment = fragment;
                mCurrentTabIndex = lastFragment.getPage();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


}
