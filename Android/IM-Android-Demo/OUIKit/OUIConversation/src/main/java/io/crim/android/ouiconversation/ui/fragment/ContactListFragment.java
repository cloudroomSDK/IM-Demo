package io.crim.android.ouiconversation.ui.fragment;


import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.text.SpannableStringBuilder;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.Toast;

import com.alibaba.android.arouter.facade.annotation.Route;
import com.alibaba.android.arouter.launcher.ARouter;
import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;
import com.yanzhenjie.recyclerview.OnItemClickListener;
import com.yanzhenjie.recyclerview.SwipeMenuCreator;
import com.yanzhenjie.recyclerview.SwipeMenuItem;

import java.util.List;
import java.util.Observable;
import java.util.Observer;

import androidx.activity.result.ActivityResultLauncher;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import io.crim.android.ouiconversation.R;
import io.crim.android.ouiconversation.databinding.FragmentContactListBinding;
import io.crim.android.ouiconversation.databinding.LayoutAddActionBinding;
import io.crim.android.ouiconversation.ui.ChatActivity;
import io.crim.android.ouiconversation.ui.NotificationActivity;
import io.crim.android.ouiconversation.ui.SearchActivity;
import io.crim.android.ouiconversation.vm.ChatVM;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.entity.MsgConversation;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.EmojiUtil;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.OnDedrepClickListener;
import io.crim.android.ouicore.utils.PermissionUtil;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.utils.SinkHelper;
import io.crim.android.ouicore.utils.TimeUtil;
import io.crim.android.ouicore.vm.ContactListVM;
import io.crim.android.ouicore.vm.MultipleChoiceVM;
import io.crim.android.ouicore.vm.UserLogic;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.ConversationType;
import io.crim.android.sdk.enums.GrpAtType;

import static android.view.ViewGroup.LayoutParams.MATCH_PARENT;

@Route(path = Routes.Conversation.CONTACT_LIST)
public class ContactListFragment extends BaseFragment<ContactListVM> implements ContactListVM.ViewAction, Observer {

    private long mLastClickTime;
    private final long timeInterval = 700;

    private FragmentContactListBinding view;
    private CustomAdapter adapter;
    private boolean hasScanPermission = false;
    private ActivityResultLauncher<Intent> resultLauncher;
    private UserLogic user = Easy.find(UserLogic.class);

    public void setResultLauncher(ActivityResultLauncher<Intent> resultLauncher) {
        this.resultLauncher = resultLauncher;
    }

    public static ContactListFragment newInstance() {

        Bundle args = new Bundle();

        ContactListFragment fragment = new ContactListFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        bindVM(ContactListVM.class);
        BaseApp.inst().putVM(vm);
        Obs.inst().addObserver(this);
        Activity activity = getActivity();
        if (null != activity) {
            activity.runOnUiThread(() -> hasScanPermission = AndPermission.hasPermissions(this,
                Permission.CAMERA, Permission.READ_EXTERNAL_STORAGE));
        }
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = FragmentContactListBinding.inflate(getLayoutInflater());

        LinearLayout.LayoutParams lp = (LinearLayout.LayoutParams) view.header.getLayoutParams();
        lp.setMargins(0, SinkHelper.getStatusBarHeight(), 0, 0);
        view.header.setLayoutParams(lp);
        init();

        return view.getRoot();
    }

    private final OnItemClickListener onItemClickListener = (view, position) -> {
        long nowTime = System.currentTimeMillis();
        if (nowTime - mLastClickTime < timeInterval) return;
        mLastClickTime = nowTime;

        MsgConversation msgConversation = vm.conversations.getValue().get(position);
        if (msgConversation.conversationInfo.getConversationType() == ConversationType.NOTIFICATION) {
            //系统通知
            Intent intent =
                new Intent(getContext(), NotificationActivity.class).putExtra(Constant.K_NAME
                    , msgConversation.conversationInfo.getShowName()).putExtra(Constant.K_ID,
                    msgConversation.conversationInfo.getConversationID());
            startActivity(intent);
            return;
        }
        Intent intent = new Intent(getContext(), ChatActivity.class)
            .putExtra(Constant.K_NAME
                , msgConversation.conversationInfo.getShowName());
        if (msgConversation.conversationInfo.getConversationType() == ConversationType.SINGLE_CHAT)
            intent.putExtra(Constant.K_ID, msgConversation.conversationInfo.getUserID());

        if (msgConversation.conversationInfo.getConversationType() == ConversationType.GROUP_CHAT
            || msgConversation.conversationInfo.getConversationType() == ConversationType.SUPER_GROUP_CHAT)
            intent.putExtra(Constant.K_GROUP_ID, msgConversation.conversationInfo.getGroupID());

        if (msgConversation.conversationInfo.getGroupAtType() == ConversationType.NOTIFICATION)
            intent.putExtra(Constant.K_NOTICE, msgConversation.notificationMsg);
        startActivity(intent);

        //重置强提醒
        CRIMClient.getInstance().conversationManager.resetConversationGrpAtType(null,
            msgConversation.conversationInfo.getConversationID());
    };

    @SuppressLint("NewApi")
    private void init() {
        view.setLifecycleOwner(this);
        view.setUser(user);
        view.setCLv(vm);
        initHeader();
        view.recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        SwipeMenuCreator mSwipeMenuCreator = (leftMenu, rightMenu, position) -> {
            SwipeMenuItem delete = new SwipeMenuItem(getContext());
            delete.setText(io.crim.android.ouicore.R.string.remove);
            delete.setHeight(MATCH_PARENT);
            delete.setWidth(Common.dp2px(73));
            delete.setTextSize(16);
            delete.setTextColor(getContext().getColor(android.R.color.white));
            delete.setBackgroundColor(getResources().getColor(io.crim.android.ouicore.R.color.txt_warning));

            MsgConversation conversationInfo = vm.conversations.getValue().get(position);
            SwipeMenuItem top = new SwipeMenuItem(getContext());
            top.setText(conversationInfo.conversationInfo.isPinned() ?
                io.crim.android.ouicore.R.string.cancel_top : R.string.top);
            top.setHeight(MATCH_PARENT);
            top.setWidth(Common.dp2px(73));
            top.setTextSize(16);
            top.setTextColor(getContext().getColor(android.R.color.white));
            top.setBackgroundColor(getResources().getColor(io.crim.android.ouicore.R.color.theme));

            SwipeMenuItem martRead = new SwipeMenuItem(getContext());
            martRead.setText(io.crim.android.ouicore.R.string.mark_read);
            martRead.setHeight(MATCH_PARENT);
            martRead.setWidth(Common.dp2px(73));
            martRead.setTextSize(16);
            martRead.setTextColor(getContext().getColor(android.R.color.white));
            martRead.setBackgroundColor(getResources().getColor(io.crim.android.ouicore.R.color.txt_shallow4));

            //右侧添加菜单
            rightMenu.addMenuItem(top);
            if (conversationInfo.conversationInfo.getUnreadCount() > 0)
                rightMenu.addMenuItem(martRead);
            rightMenu.addMenuItem(delete);
        };
        view.recyclerView.setSwipeMenuCreator(mSwipeMenuCreator);
        ChatVM chatVM = new ChatVM();
        view.recyclerView.setOnItemMenuClickListener((menuBridge, adapterPosition) -> {
            int menuPosition = menuBridge.getPosition();
            MsgConversation conversationInfo = vm.conversations.getValue().get(adapterPosition);
            if (menuPosition == 0) {
                vm.pinConversation(conversationInfo.conversationInfo,
                    !conversationInfo.conversationInfo.isPinned());
            } else if (menuPosition == 1 && conversationInfo.conversationInfo.getUnreadCount() > 0) {
                chatVM.markReadedByConID(conversationInfo.conversationInfo.getConversationID(),
                    null);
            } else {
                vm.conversations.getValue().remove(conversationInfo);
                adapter.notifyItemRemoved(adapterPosition);
                vm.deleteConversationAndDeleteAllMsg(conversationInfo.conversationInfo.getConversationID());
            }
            menuBridge.closeMenu();
        });

        adapter = new CustomAdapter(onItemClickListener);
        view.recyclerView.setAdapter(adapter);
        view.recyclerView.addHeaderView(createHeaderView());
        view.recyclerView.setItemAnimator(null);

        vm.conversations.observe(getActivity(), v -> {
            if (null == v || v.size() == 0) return;
            adapter.setConversationInfos(v);
            adapter.notifyDataSetChanged();
        });
        vm.subscribe(getActivity(), subject -> {
            if (subject.equals(ContactListVM.NOTIFY_ITEM_CHANGED)) {
                adapter.notifyItemChanged((Integer) subject.value);
            }
        });

        Animation animation = AnimationUtils.loadAnimation(getActivity(),
            R.anim.animation_repeat_spinning);
        Easy.find(UserLogic.class).connectStatus
            .observe(getActivity(), connectStatus -> {
                if (connectStatus == UserLogic.ConnectStatus.CONNECTING
                    || connectStatus == UserLogic.ConnectStatus.SYNCING) {
                    view.status.startAnimation(animation);
                } else {
                    view.status.clearAnimation();
                }
            });
        vm.getSelfUserInfo();
    }

    private void initHeader() {
        view.avatar.load(BaseApp.inst().loginCertificate.faceURL);
//        view.name.setText(BaseApp.inst().loginCertificate.nickName);
        view.addFriend.setOnClickListener(this::showPopupWindow);
        view.callRecord.setOnClickListener(view -> {
            ARouter.getInstance().build(Routes.Main.CALL_HISTORY).navigation();
        });
    }


    private void showPopupWindow(View v) {
        //初始化一个PopupWindow，width和height都是WRAP_CONTENT
        PopupWindow popupWindow = new PopupWindow(ViewGroup.LayoutParams.WRAP_CONTENT,
            ViewGroup.LayoutParams.WRAP_CONTENT);
        LayoutAddActionBinding view = LayoutAddActionBinding.inflate(getLayoutInflater());
        view.scan.setOnClickListener(c -> {
            popupWindow.dismiss();
            Common.permission(getActivity(), () -> {
                hasScanPermission = true;
                Common.jumpScan(getActivity(), resultLauncher);
            }, hasScanPermission, Permission.CAMERA, PermissionUtil.getReadImgPermission());
        });
        view.addFriend.setOnClickListener(c -> {
            popupWindow.dismiss();
            ARouter.getInstance().build(Routes.Main.ADD_CONVERS).navigation();
        });
        view.addGroup.setOnClickListener(c -> {
            popupWindow.dismiss();
            ARouter.getInstance().build(Routes.Main.ADD_CONVERS).withBoolean(Constant.K_RESULT,
                false).navigation();
        });
        view.createGroup.setOnClickListener(c -> {
            popupWindow.dismiss();

            Easy.installVM(MultipleChoiceVM.class).isCreateGroup = true;
            ARouter.getInstance().build(Routes.Group.SELECT_TARGET).navigation();
        });
        view.videoMeeting.setOnClickListener(c -> {
            popupWindow.dismiss();
            ARouter.getInstance().build(Routes.Meeting.HOME).navigation();
        });
        //设置PopupWindow的视图内容
        popupWindow.setContentView(view.getRoot());
        //点击空白区域PopupWindow消失，这里必须先设置setBackgroundDrawable，否则点击无反应
        popupWindow.setBackgroundDrawable(new ColorDrawable(0x00000000));
        popupWindow.setOutsideTouchable(true);

        //设置PopupWindow消失监听
        popupWindow.setOnDismissListener(new PopupWindow.OnDismissListener() {
            @Override
            public void onDismiss() {
            }
        });
        //PopupWindow在targetView下方弹出
        popupWindow.showAsDropDown(v);
    }

    private View createHeaderView() {
        View header = getLayoutInflater().inflate(R.layout.view_search, view.recyclerView, false);
        header.setOnClickListener(v -> startActivity(new Intent(getActivity(),
            SearchActivity.class)));
        return header;
    }


    @Override
    public void onErr(String msg) {
        try {
            Toast.makeText(getContext(), msg, Toast.LENGTH_SHORT).show();
        } catch (Exception ignored) {
        }

    }

    @Override
    public void onSuccess(Object body) {
        super.onSuccess(body);
    }

    static class CustomAdapter extends RecyclerView.Adapter<ViewHol.ContactItemHolder> {

        private List<MsgConversation> conversationInfos;
        private OnItemClickListener itemClickListener;

        public CustomAdapter(OnItemClickListener itemClickListener) {
            this.itemClickListener = itemClickListener;
        }

        public void setConversationInfos(List<MsgConversation> conversationInfos) {
            this.conversationInfos = conversationInfos;
        }

        @Override
        public ViewHol.ContactItemHolder onCreateViewHolder(ViewGroup viewGroup, int viewType) {
            return new ViewHol.ContactItemHolder(viewGroup);
        }

        @Override
        public void onBindViewHolder(ViewHol.ContactItemHolder viewHolder, int position) {
            final int index = position;
            viewHolder.viewBinding.getRoot().setOnClickListener(new OnDedrepClickListener() {
                @Override
                public void click(View v) {
                    if (null != itemClickListener)
                        itemClickListener.onItemClick(v, index);
                }
            });

            MsgConversation msgConversation = conversationInfos.get(position);
            boolean isGroup =
                msgConversation.conversationInfo.getConversationType() != ConversationType.SINGLE_CHAT;
            viewHolder.viewBinding.avatar.load(msgConversation.conversationInfo.getFaceURL(),
                isGroup, isGroup ? null : msgConversation.conversationInfo.getShowName());
            viewHolder.viewBinding.nickName.setText(msgConversation.conversationInfo.getShowName());

            if (msgConversation.conversationInfo.getRecvMsgOpt() != 0) {
                viewHolder.viewBinding.noDisturbTips
                    .setVisibility(msgConversation.conversationInfo.getUnreadCount() > 0 ? View.VISIBLE : View.GONE);
                viewHolder.viewBinding.noDisturbIc.setVisibility(View.VISIBLE);
                viewHolder.viewBinding.badge.badge.setVisibility(View.GONE);
            } else {
                viewHolder.viewBinding.badge.badge.setVisibility(View.VISIBLE);
                viewHolder.viewBinding.noDisturbTips.setVisibility(View.GONE);
                viewHolder.viewBinding.noDisturbIc.setVisibility(View.GONE);
                viewHolder.viewBinding.badge.badge.setVisibility(msgConversation.conversationInfo.getUnreadCount() != 0 ? View.VISIBLE : View.GONE);
                viewHolder.viewBinding.badge.badge.setText(msgConversation.conversationInfo.getUnreadCount() + "");
            }
            viewHolder.viewBinding.time.setText(TimeUtil.getTimeString(msgConversation.conversationInfo.getLatestMsgSendTime()));

//            viewHolder.viewBinding.getRoot().setBackgroundColor(Color.parseColor
//            (msgConversation.conversationInfo.isPinned() ? "#FFF3F3F3" : "#FFFFFF"));
            viewHolder.viewBinding.setTop.setVisibility(msgConversation.conversationInfo.isPinned() ? View.VISIBLE : View.GONE);

            CharSequence lastMsg = msgConversation.lastMsg;
            if (msgConversation.conversationInfo.isPrivateChat()) {
                viewHolder.viewBinding.lastMsg.setText("[阅后即焚消息]");
            } else if (msgConversation.conversationInfo.getGroupAtType() == GrpAtType.AT_ME) {  //强提醒
                String target =
                    "@" + BaseApp.inst().getString(io.crim.android.ouicore.R.string.you);
                if (!lastMsg.toString().contains(target))
                    lastMsg = target + "\t" + lastMsg;

                IMUtil.buildClickAndColorSpannable(new SpannableStringBuilder(lastMsg), target, android.R.color.holo_red_dark, null);
                viewHolder.viewBinding.lastMsg.setText(lastMsg);
            } else {
                EmojiUtil.showSpanTextview(viewHolder.viewBinding.lastMsg, lastMsg.toString());
            }
        }

        @Override
        public int getItemCount() {
            return null == conversationInfos ? 0 : conversationInfos.size();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        BaseApp.viewModels.remove(vm.getClass().getCanonicalName());
        Obs.inst().deleteObserver(this);

        Animation animation = view.status.getAnimation();
        if (null != animation) animation.cancel();
    }

    @Override
    public void update(Observable observable, Object o) {
        Obs.Message message = (Obs.Message) o;
        if (message.tag == Constant.Event.USER_INFO_UPDATE) {
            initHeader();
        }
    }
}
