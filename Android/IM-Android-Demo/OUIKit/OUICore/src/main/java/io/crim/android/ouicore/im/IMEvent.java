package io.crim.android.ouicore.im;


import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

import crim_sdk_callback.OnSignalingListener;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnAdvanceMsgListener;
import crim_sdk_callback.OnConnListener;
import io.crim.android.sdk.listener.OnConversationListener;
import io.crim.android.sdk.listener.OnFriendshipListener;
import io.crim.android.sdk.listener.OnGrpListener;
import io.crim.android.sdk.listener.OnUserListener;
import io.crim.android.sdk.models.BlacklistInfo;
import io.crim.android.sdk.models.ConversationInfo;
import io.crim.android.sdk.models.FriendInfo;
import io.crim.android.sdk.models.FriendReqInfo;
import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.sdk.models.GrpReqInfo;
import io.crim.android.sdk.models.KeyValue;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.models.ReadReceiptInfo;
import io.crim.android.sdk.models.RevokedInfo;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.L;
import io.crim.android.ouicore.vm.UserLogic;

///im事件 统一处理
public class IMEvent {
    private static IMEvent listener = null;
    private List<OnConnListener> connListeners;
    private List<OnAdvanceMsgListener> advanceMsgListeners;
    private List<OnConversationListener> conversationListeners;
    private List<OnGrpListener> groupListeners;
    private List<OnFriendshipListener> friendshipListeners;
    private List<OnSignalingListener> signalingListeners;

    public void init() {
        connListeners = new ArrayList<>();
        advanceMsgListeners = new ArrayList<>();
        conversationListeners = new ArrayList<>();
        groupListeners = new ArrayList<>();
        friendshipListeners = new ArrayList<>();
        signalingListeners = new ArrayList<>();

        userListener();
        advanceMsgListener();
        friendshipListener();
        conversationListener();
        groupListeners();
        signalingListener();
    }

    private void signalingListener() {
        /*CRIMClient.getInstance().signalingManager.setSignalingListener(new OnSignalingListener() {
            @Override
            public void onInvitationCancelled(SignalingInfo s) {
                // 被邀请者收到：邀请者取消音视频通话
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onInvitationCancelled(s);
                }
            }

            @Override
            public void onInvitationTimeout(SignalingInfo s) {
                // 邀请者收到：被邀请者超时未接通
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onInvitationTimeout(s);
                }
            }

            @Override
            public void onInviteeAccepted(SignalingInfo s) {
                // 邀请者收到：被邀请者同意音视频通话
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onInviteeAccepted(s);
                }
            }

            @Override
            public void onInviteeAcceptedByOtherDevice(SignalingInfo s) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onInviteeAcceptedByOtherDevice(s);
                }
            }

            @Override
            public void onInviteeRejected(SignalingInfo s) {
                // 邀请者收到：被邀请者拒绝音视频通话
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onInviteeRejected(s);
                }
            }

            @Override
            public void onInviteeRejectedByOtherDevice(SignalingInfo s) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onInviteeRejectedByOtherDevice(s);
                }
            }

            @Override
            public void onReceiveNewInvitation(SignalingInfo s) {
                // 被邀请者收到：音视频通话邀请
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onReceiveNewInvitation(s);
                }
            }

            @Override
            public void onHangup(SignalingInfo s) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onHangup(s);
                }
            }

            @Override
            public void onRoomParticipantConnected(RoomCallingInfo s) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onRoomParticipantConnected(s);
                }
            }

            @Override
            public void onRoomParticipantDisconnected(RoomCallingInfo s) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onRoomParticipantDisconnected(s);
                }
            }

            @Override
            public void onMeetingStreamChanged(MeetingStreamEvent e) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onMeetingStreamChanged(e);
                }
            }

            @Override
            public void onReceiveCustomSignal(CustomSignalingInfo s) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onReceiveCustomSignal(s);
                }
            }

            @Override
            public void onStreamChange(String s) {
                for (OnSignalingListener signalingListener : signalingListeners) {
                    signalingListener.onStreamChange(s);
                }
            }
        });*/
    }

    public static synchronized IMEvent getInstance() {
        if (null == listener) listener = new IMEvent();
        return listener;
    }

    //信令监听
    public void addSignalingListener(OnSignalingListener onSignalingListener) {
        if (!signalingListeners.contains(onSignalingListener)) {
            signalingListeners.add(onSignalingListener);
        }
    }

    public void removeSignalingListener(OnSignalingListener onSignalingListener) {
        signalingListeners.remove(onSignalingListener);
    }

    //连接事件
    public void addConnListener(OnConnListener onConnListener) {
        if (!connListeners.contains(onConnListener)) {
            connListeners.add(onConnListener);
        }
    }

    public void removeConnListener(OnConnListener onConnListener) {
        connListeners.remove(onConnListener);
    }

    // 会话新增或改变监听
    public void addConversationListener(OnConversationListener onConversationListener) {
        if (!conversationListeners.contains(onConversationListener)) {
            conversationListeners.add(onConversationListener);
        }
    }

    public void removeConversationListener(OnConversationListener onConversationListener) {
        conversationListeners.remove(onConversationListener);
    }

    // 收到新消息，已读回执，消息撤回监听。
    public void addAdvanceMsgListener(OnAdvanceMsgListener onAdvanceMsgListener) {
        if (!advanceMsgListeners.contains(onAdvanceMsgListener)) {
            advanceMsgListeners.add(onAdvanceMsgListener);
        }
    }

    public void removeAdvanceMsgListener(OnAdvanceMsgListener onAdvanceMsgListener) {
        advanceMsgListeners.remove(onAdvanceMsgListener);
    }

    // 群组关系发生改变监听
    public void addGroupListener(OnGrpListener onGroupListener) {
        if (!groupListeners.contains(onGroupListener)) {
            groupListeners.add(onGroupListener);
        }
    }

    public void removeGroupListener(OnGrpListener onGroupListener) {
        groupListeners.remove(onGroupListener);
    }

    // 好友关系发生改变监听
    public void addFriendListener(OnFriendshipListener onFriendshipListener) {
        if (!friendshipListeners.contains(onFriendshipListener)) {
            friendshipListeners.add(onFriendshipListener);
        }
    }

    public void removeFriendListener(OnFriendshipListener onFriendshipListener) {
        friendshipListeners.remove(onFriendshipListener);
    }


    //连接事件
    public OnConnListener connListener = new OnConnListener() {
        private UserLogic userLogic=Easy.find(UserLogic.class);
        @Override
        public void onConnectFailed(int code, String error) {
            // 连接服务器失败，可以提示用户当前网络连接不可用
            L.d("连接服务器失败(" + error + ")");
            for (OnConnListener onConnListener : connListeners) {
                onConnListener.onConnectFailed(code, error);
            }
        }

        @Override
        public void onConnectSuccess() {
            // 已经成功连接到服务器
            L.d("已经成功连接到服务器");
            for (OnConnListener onConnListener : connListeners) {
                onConnListener.onConnectSuccess();
            }
        }

        @Override
        public void onConnecting() {
            // 正在连接到服务器，适合在 UI 上展示“正在连接”状态。
            L.d("正在连接到服务器...");
            for (OnConnListener onConnListener : connListeners) {
                onConnListener.onConnecting();
            }
        }

        @Override
        public void onKickedOffline() {
            // 当前用户被踢下线，此时可以 UI 提示用户“您已经在其他端登录了当前账号，是否重新登录？”
            L.d("当前用户被踢下线");
            Toast.makeText(BaseApp.inst(),
                BaseApp.inst().getString(io.crim.android.ouicore.R.string.kicked_offline_tips),
                Toast.LENGTH_SHORT).show();
            for (OnConnListener onConnListener : connListeners) {
                onConnListener.onKickedOffline();
            }
        }

        @Override
        public void onUserTokenExpired() {
            // 登录票据已经过期，请使用新签发的 UserSig 进行登录。
            L.d("登录票据已经过期");
            Toast.makeText(BaseApp.inst(),
                BaseApp.inst().getString(io.crim.android.ouicore.R.string.token_expired),
                Toast.LENGTH_SHORT).show();
            for (OnConnListener onConnListener : connListeners) {
                onConnListener.onUserTokenExpired();
            }
        }
    };


    // 群组关系发生改变监听
    private void groupListeners() {
        CRIMClient.getInstance().groupManager.setOnGrpListener(new OnGrpListener() {

            @Override
            public void onGrpReqAccepted(GrpReqInfo info) {
                // 发出或收到的组申请被接受
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onGrpReqAccepted(info);
                }
            }

            @Override
            public void onGrpReqAdded(GrpReqInfo info) {
                // 发出或收到的组申请有新增
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onGrpReqAdded(info);
                }
            }

            @Override
            public void onGrpReqDeleted(GrpReqInfo info) {
                // 发出或收到的组申请被删除
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onGrpReqDeleted(info);
                }
            }

            @Override
            public void onGrpReqRejected(GrpReqInfo info) {
                // 发出或收到的组申请被拒绝
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onGrpReqRejected(info);
                }
            }

            @Override
            public void onGrpDismissed(GrpInfo info) {

            }

            @Override
            public void onGrpInfoChanged(GrpInfo info) {
                // 组资料变更
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onGrpInfoChanged(info);
                }
            }

            @Override
            public void onGrpMemberAdded(GroupMembersInfo info) {
                // 组成员进入
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onGrpMemberAdded(info);
                }
            }

            @Override
            public void onGrpMemberDeleted(GroupMembersInfo info) {
                // 组成员退出
            }

            @Override
            public void onGrpMemberInfoChanged(GroupMembersInfo info) {
                // 组成员信息发生变化
            }

            @Override
            public void onJoinedGrpAdded(GrpInfo info) {
                // 创建群： 初始成员收到；邀请进群：被邀请者收到
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onJoinedGrpAdded(info);
                }
            }

            @Override
            public void onJoinedGrpDeleted(GrpInfo info) {
                // 退出群：退出者收到；踢出群：被踢者收到
                for (OnGrpListener onGroupListener : groupListeners) {
                    onGroupListener.onJoinedGrpDeleted(info);
                }
            }
        });
    }

    // 会话新增或改变监听
    private void conversationListener() {
        CRIMClient.getInstance().conversationManager.setOnConversationListener(new OnConversationListener() {

            @Override
            public void onConversationChanged(List<ConversationInfo> list) {
                for (ConversationInfo conversationInfo : list) {
                    promptSoundOrNotification(conversationInfo);
                }
                // 已添加的会话发生改变
                for (OnConversationListener onConversationListener : conversationListeners) {
                    onConversationListener.onConversationChanged(list);
                }
            }

            @Override
            public void onNewConversation(List<ConversationInfo> list) {
                // 新增会话
                for (OnConversationListener onConversationListener : conversationListeners) {
                    onConversationListener.onNewConversation(list);
                }
            }

            @Override
            public void onSyncServerFailed() {
                for (OnConversationListener onConversationListener : conversationListeners) {
                    onConversationListener.onSyncServerFailed();
                }
            }

            @Override
            public void onSyncServerFinish() {
                for (OnConversationListener onConversationListener : conversationListeners) {
                    onConversationListener.onSyncServerFinish();
                }

            }

            @Override
            public void onSyncServerStart() {
                for (OnConversationListener onConversationListener : conversationListeners) {
                    onConversationListener.onSyncServerStart();
                }
            }

            @Override
            public void onTotalUnreadMessageCountChanged(int i) {
                // 未读消息数发送变化
                for (OnConversationListener onConversationListener : conversationListeners) {
                    onConversationListener.onTotalUnreadMessageCountChanged(i);
                }
            }
        });
    }

    private void promptSoundOrNotification(ConversationInfo conversationInfo) {
        try {
            if (BaseApp.inst().loginCertificate.globalRecvMsgOpt == 2) return;
            Message msg= GsonHel.fromJson(conversationInfo.getLatestMsg(),Message.class);
            if (conversationInfo.getRecvMsgOpt() == 0
                && conversationInfo.getUnreadCount() != 0) {
                if (BaseApp.inst().isBackground())
                    IMUtil.sendNotice(msg.getClientMsgID().hashCode(), conversationInfo);
                else
                    IMUtil.playPrompt();
            }
        } catch (Exception ignored) {
        }
    }

    // 好关系发生变化监听
    private void friendshipListener() {
        CRIMClient.getInstance().friendshipManager.setOnFriendshipListener(new OnFriendshipListener() {
            @Override
            public void onBlacklistAdded(BlacklistInfo u) {
                // 拉入黑名单
            }

            @Override
            public void onBlacklistDeleted(BlacklistInfo u) {
                // 从黑名单删除
            }

            @Override
            public void onFriendApplicationAccepted(FriendReqInfo u) {
                // 发出或收到的好友申请已同意
                for (OnFriendshipListener friendshipListener : friendshipListeners) {
                    friendshipListener.onFriendApplicationAccepted(u);
                }
            }

            @Override
            public void onFriendApplicationAdded(FriendReqInfo u) {
                // 发出或收到的好友申请被添加
                for (OnFriendshipListener friendshipListener : friendshipListeners) {
                    friendshipListener.onFriendApplicationAdded(u);
                }
            }

            @Override
            public void onFriendApplicationDeleted(FriendReqInfo u) {
                // 发出或收到的好友申请被删除
                for (OnFriendshipListener friendshipListener : friendshipListeners) {
                    friendshipListener.onFriendApplicationDeleted(u);
                }
            }

            @Override
            public void onFriendApplicationRejected(FriendReqInfo u) {
                // 发出或收到的好友申请被拒绝
                for (OnFriendshipListener friendshipListener : friendshipListeners) {
                    friendshipListener.onFriendApplicationRejected(u);
                }
            }

            @Override
            public void onFriendInfoChanged(FriendInfo u) {
                // 朋友的资料发生变化
            }

            @Override
            public void onFriendAdded(FriendInfo u) {
                // 好友被添加
            }

            @Override
            public void onFriendDeleted(FriendInfo u) {
                // 好友被删除
            }
        });
    }

    // 收到新消息，已读回执，消息撤回监听。
    private void advanceMsgListener() {
        CRIMClient.getInstance().messageManager.setAdvancedMsgListener(new OnAdvanceMsgListener() {
            @Override
            public void onRecvNewMsg(Message msg) {
                // 收到新消息，界面添加新消息
                for (OnAdvanceMsgListener onAdvanceMsgListener : advanceMsgListeners) {
                    onAdvanceMsgListener.onRecvNewMsg(msg);
                }
            }

            @Override
            public void onRecv1v1ReadReceipt(List<ReadReceiptInfo> list) {
                // 消息被阅读回执，将消息标记为已读
                for (OnAdvanceMsgListener onAdvanceMsgListener : advanceMsgListeners) {
                    onAdvanceMsgListener.onRecv1v1ReadReceipt(list);
                }
            }

            @Override
            public void onRecvMsgRevokedV2(RevokedInfo info) {
                // 消息成功撤回，从界面移除消息
                for (OnAdvanceMsgListener onAdvanceMsgListener : advanceMsgListeners) {
                    onAdvanceMsgListener.onRecvMsgRevokedV2(info);
                }
            }

            @Override
            public void onRecvMsgExtensionsChanged(String msgID, List<KeyValue> list) {
                for (OnAdvanceMsgListener onAdvanceMsgListener : advanceMsgListeners) {
                    onAdvanceMsgListener.onRecvMsgExtensionsChanged(msgID, list);
                }
            }

            @Override
            public void onRecvMsgExtensionsDeleted(String msgID, List<String> list) {
                for (OnAdvanceMsgListener onAdvanceMsgListener : advanceMsgListeners) {
                    onAdvanceMsgListener.onRecvMsgExtensionsDeleted(msgID, list);
                }
            }

            @Override
            public void onRecvMsgExtensionsAdded(String msgID, List<KeyValue> list) {
                for (OnAdvanceMsgListener onAdvanceMsgListener : advanceMsgListeners) {
                    onAdvanceMsgListener.onRecvMsgExtensionsAdded(msgID, list);
                }
            }

            @Override
            public void onMsgDeleted(Message message) {

            }

            @Override
            public void onRecvOfflineNewMessage(List<Message> msg) {

            }

            @Override
            public void onRecvGrpReadReceipt(List<ReadReceiptInfo> list) {
                // 消息被阅读回执，将消息标记为已读
                for (OnAdvanceMsgListener onAdvanceMsgListener : advanceMsgListeners) {
                    onAdvanceMsgListener.onRecvGrpReadReceipt(list);
                }
            }
        });
    }


    // 用户资料变更监听
    private void userListener() {
        CRIMClient.getInstance().userInfoManager.setOnUserListener(new OnUserListener() {
            @Override
            public void onSelfInfoUpdated(UserInfo info) {
                // 当前登录用户资料变更回调
            }

            @Override
            public void onUserStatusChanged(String var1) {

            }
        });
    }
}


