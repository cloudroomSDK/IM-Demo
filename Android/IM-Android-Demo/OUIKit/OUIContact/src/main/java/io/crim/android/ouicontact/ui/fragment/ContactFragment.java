package io.crim.android.ouicontact.ui.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.alibaba.android.arouter.facade.annotation.Route;
import com.alibaba.android.arouter.launcher.ARouter;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Observable;
import java.util.Observer;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import io.crim.android.ouicontact.databinding.FragmentContactMainBinding;
import io.crim.android.ouicontact.databinding.ViewContactHeaderBinding;
import io.crim.android.ouicontact.ui.AllFriendActivity;
import io.crim.android.ouicontact.ui.GroupNoticeListActivity;
import io.crim.android.ouicontact.ui.MyGroupActivity;
import io.crim.android.ouicontact.ui.NewFriendActivity;
import io.crim.android.ouicontact.vm.ContactVM;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.entity.MsgConversation;
import io.crim.android.ouicore.services.MomentsBridge;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.utils.SharedPreferencesUtil;
import io.crim.android.ouicore.utils.SinkHelper;
import io.crim.android.ouicore.vm.ContactListVM;
import io.crim.android.sdk.enums.ConversationType;

@Route(path = Routes.Contact.HOME)
public class ContactFragment extends BaseFragment<ContactVM> implements Observer {
    private FragmentContactMainBinding view;
    private ViewContactHeaderBinding header;
    private RecyclerViewAdapter<MsgConversation, ViewHol.ItemRectViewHo> adapter;
    private ContactListVM contactListVM;

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
        view.header.moments.setVisibility(null == momentsBridge ? View.GONE : View.VISIBLE);
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
        /*Easy.find(NotificationVM.class).momentsUnread.observe(getActivity(), v -> {
            header.newMomentsMsg.badge.setVisibility(v == 0 ? View.GONE : View.VISIBLE);
        });*/
        view.rvContact.setLayoutManager(new LinearLayoutManager(getContext()));
        view.rvContact.setAdapter(adapter = new RecyclerViewAdapter<MsgConversation,
            ViewHol.ItemRectViewHo>(ViewHol.ItemRectViewHo.class) {

            @Override
            public void onBindView(@NonNull ViewHol.ItemRectViewHo holder, MsgConversation data, int position) {
                boolean isGroup =
                    data.conversationInfo.getConversationType() != ConversationType.SINGLE_CHAT;
                String id = isGroup ? data.conversationInfo.getGroupID() :
                    data.conversationInfo.getUserID();
                String faceURL = data.conversationInfo.getFaceURL();
                String name = data.conversationInfo.getShowName();
                holder.view.avatar.load(faceURL, isGroup, isGroup ? null : name);
                holder.view.nickName.setText(name);

                /*holder.view.select.setVisibility(View.VISIBLE);
                if (multipleChoiceVM.contains(new MultipleChoice(id))) {
                    holder.view.select.setChecked(true);
                    for (MultipleChoice choice : multipleChoiceVM.metaData.val()) {
                        if (choice.key.equals(id)) {
                            holder.view.select.setEnabled(choice.isEnabled);
                        }
                    }
                } else {
                    holder.view.select.setChecked(false);
                }*/

                holder.view.getRoot().setOnClickListener(v -> {
                    if (contactListVM!=null && contactListVM.conversations!=null){
                        MsgConversation msgConversation = Objects.requireNonNull(contactListVM.conversations.getValue()).get(position);
                        if (msgConversation!=null){
                            /*Intent intent = new Intent(getContext(), ChatActivity.class)
                                .putExtra(Constant.K_NAME
                                    , msgConversation.conversationInfo.getShowName());*/
                            String showName = msgConversation.conversationInfo.getShowName();
                            String keyId = "";
                            String valueId = "";
                            if (msgConversation.conversationInfo.getConversationType() == ConversationType.SINGLE_CHAT){
                                keyId=Constant.K_ID;
                                valueId=msgConversation.conversationInfo.getUserID();
                            }
//                                intent.putExtra(Constant.K_ID, msgConversation.conversationInfo.getUserID());

                            if (msgConversation.conversationInfo.getConversationType() == ConversationType.GROUP_CHAT
                                || msgConversation.conversationInfo.getConversationType() == ConversationType.SUPER_GROUP_CHAT){
                                keyId=Constant.K_GROUP_ID;
                                valueId=msgConversation.conversationInfo.getGroupID();
                            }
//                                intent.putExtra(Constant.K_GROUP_ID, msgConversation.conversationInfo.getGroupID());

                            /*if (msgConversation.conversationInfo.getGroupAtType() == ConversationType.NOTIFICATION)
                                intent.putExtra(Constant.K_NOTICE, msgConversation.notificationMsg);
                            startActivity(intent);*/

                            ARouter.getInstance().build(Routes.Conversation.CHAT).withString(keyId, valueId).withString(io.crim.android.ouicore.utils.Constant.K_NAME, showName).navigation();

                        }
                    }
                });
            }

        });
        /*ContactListVM vmByCache = BaseApp.inst().getVMByCache(ContactListVM.class);
        if (multipleChoiceVM.invite || multipleChoiceVM.isCreateGroup) {
            //只保留单聊
            for (MsgConversation msgConversation : vmByCache.conversations.getValue()) {
                if (msgConversation.conversationInfo.getConversationType() == ConversationType.SINGLE_CHAT) {
                    conversations.add(msgConversation);
                }
            }
        } else {
            conversations.addAll(vmByCache.conversations.getValue());
        }*/
        /*if (vmByCache!=null){
            List<MsgConversation> conversations = new ArrayList<>(Objects.requireNonNull(vmByCache.conversations.getValue()));
            adapter.setItems(conversations);
        }*/
    }

    @Override
    public void onHiddenChanged(boolean hidden) {
        super.onHiddenChanged(hidden);
        if (!hidden){
            contactListVM = BaseApp.inst().getVMByCache(ContactListVM.class);
            if (contactListVM!=null){
                List<MsgConversation> conversations = new ArrayList<>(Objects.requireNonNull(contactListVM.conversations.getValue()));
                adapter.setItems(conversations);
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Obs.inst().deleteObserver(this);
    }

    @Override
    public void update(Observable o, Object arg) {
        Obs.Message message = (Obs.Message) arg;

    }
}
