package io.crim.android.ouicontact.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.alibaba.android.arouter.facade.annotation.Route;
import com.alibaba.android.arouter.launcher.ARouter;

import java.util.Observable;
import java.util.Observer;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.crim.android.ouicontact.databinding.FragmentContactMainBinding;
import io.crim.android.ouicontact.databinding.ViewContactHeaderBinding;
import io.crim.android.ouicontact.ui.AllFriendActivity;
import io.crim.android.ouicontact.ui.GroupNoticeListActivity;
import io.crim.android.ouicontact.ui.MyGroupActivity;
import io.crim.android.ouicontact.ui.NewFriendActivity;
import io.crim.android.ouicontact.vm.ContactVM;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.services.MomentsBridge;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.utils.SharedPreferencesUtil;
import io.crim.android.ouicore.utils.SinkHelper;
import io.crim.android.ouicore.vm.NotificationVM;

@Route(path = Routes.Contact.HOME)
public class ContactFragment extends BaseFragment<ContactVM> implements Observer {
    private FragmentContactMainBinding view;
    private ViewContactHeaderBinding header;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        bindVM(ContactVM.class);
        Obs.inst().addObserver(this);
        super.onCreate(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = FragmentContactMainBinding.inflate(getLayoutInflater());
        header = view.header;
        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams) view.title.getLayoutParams();
        lp.setMargins(0, SinkHelper.getStatusBarHeight(), 0, 0);
        view.title.setLayoutParams(lp);

        initView();
        click();

        return view.getRoot();
    }


    public ContactVM getVM() {
        return vm;
    }

    private void click() {
        MomentsBridge momentsBridge =
            (MomentsBridge) ARouter.getInstance().build(Routes.Service.MOMENTS).navigation();
        view.header.moments.setVisibility(null==momentsBridge?View.GONE:View.VISIBLE);
        view.header.moments.setOnClickListener(v -> ARouter.getInstance().build(Routes.Moments.HOME).navigation());

        view.addFriend.setOnClickListener(view1 -> {
            ARouter.getInstance().build(Routes.Main.ADD_CONVERS)
                .navigation();
        });
        view.search.setOnClickListener(view1 -> {
            ARouter.getInstance().build(Routes.Conversation.SEARCH).navigation();
        });
        header.groupNotice.setOnClickListener(v -> {
            vm.groupDotNum.setValue(0);
            SharedPreferencesUtil.remove(getContext(), Constant.K_GROUP_NUM);
            startActivity(new Intent(getActivity(), GroupNoticeListActivity.class));
        });

        header.newFriendNotice.setOnClickListener(v -> {
            vm.friendDotNum.setValue(0);
            SharedPreferencesUtil.remove(getContext(), Constant.K_FRIEND_NUM);
            startActivity(new Intent(getActivity(), NewFriendActivity.class));
        });

        header.myGoodFriend.setOnClickListener(v -> {
            startActivity(new Intent(getActivity(), AllFriendActivity.class));
        });

        header.myGroup.setOnClickListener(v -> {
            startActivity(new Intent(getActivity(), MyGroupActivity.class));
        });
//        header.labelLy.setOnClickListener(v -> {
//            startActivity(new Intent(getActivity(), LabelActivity.class));
//        });
    }


    private void initView() {
        vm.groupDotNum.observe(getActivity(), v -> {
            header.badge.badge.setVisibility(v == 0 ? View.GONE : View.VISIBLE);
            header.badge.badge.setText(v + "");
        });
        vm.friendDotNum.observe(getActivity(), v -> {
            header.newFriendNoticeBadge.badge.setVisibility(v == 0 ? View.GONE : View.VISIBLE);
            header.newFriendNoticeBadge.badge.setText(v + "");
        });

        Easy.find(NotificationVM.class).momentsUnread.observe(getActivity(),v->{
            header.newMomentsMsg.badge.setVisibility(v == 0 ? View.GONE : View.VISIBLE);
        });
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Obs.inst().deleteObserver(this);
    }

    @Override
    public void update(Observable o, Object arg) {
        Obs.Msg message = (Obs.Msg) arg;

    }
}
