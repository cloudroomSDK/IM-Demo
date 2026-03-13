package io.crim.android.ouicore.vm;

import com.alibaba.android.arouter.launcher.ARouter;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.base.IView;
import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.entity.MsgConversation;
import io.crim.android.ouicore.im.IMEvent;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.BeanUtil;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.ConversationType;
import io.crim.android.sdk.enums.CustomMsgType;
import io.crim.android.sdk.enums.InvitationOpType;
import io.crim.android.sdk.listener.OnAdvanceMsgListener;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.listener.OnConversationListener;
import io.crim.android.sdk.listener.OnMsgSendCallback;
import io.crim.android.sdk.models.ConversationInfo;
import io.crim.android.sdk.models.CustomElemData;
import io.crim.android.sdk.models.KeyValue;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.models.OfflinePushInfo;
import io.crim.android.sdk.models.ReadReceiptInfo;
import io.crim.android.sdk.models.RevokedInfo;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.sdk.utils.JsonUtil;

public class ContactListVM extends BaseViewModel<ContactListVM.ViewAction> implements OnConversationListener, OnAdvanceMsgListener {

    public static final String TAG = "ContactListVM";
    public static final String NOTIFY_ITEM_CHANGED = "notify_item_changed";
    public State<List<MsgConversation>> conversations = new State<>(new ArrayList<>());
    public State<List<UserInfo>> frequentContacts = new State<>(new ArrayList<>());
    public State<String> nickName = new State<>();
    public boolean isCalling = false;
    public Message newInvitationMsg;


    @Override
    protected void viewCreate() {
        IMEvent.getInstance().addConversationListener(this);
        IMEvent.getInstance().addAdvanceMsgListener(this);
        updateConversation();
    }

    public void deleteConversationAndDeleteAllMsg(String conversationId) {
        CRIMClient.getInstance().conversationManager
            .deleteConversationAndDeleteAllMsg(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {

                }

                @Override
                public void onSuccess(String data) {
                    updateConversation();
                }
            }, conversationId);
    }

    private void updateConversation() {
        CRIMClient.getInstance().conversationManager.getAllConversationList(new OnBase<List<ConversationInfo>>() {
            @Override
            public void onError(int code, String error) {
                getIView().onErr(error);
            }

            @Override
            public void onSuccess(List<ConversationInfo> data) {
                conversations.val().clear();
                for (ConversationInfo datum : data) {
                    Message msg = null;
                    if (null != datum.getLatestMsg()) {
                        msg = GsonHel.fromJson(datum.getLatestMsg(), Message.class);
                    }
                    conversations.val().add(new MsgConversation(msg, datum));
                }
                conversations.setValue(conversations.getValue());
            }
        });
    }

    public void setOneConversationPrivateChat(IMUtil.OnSuccessListener<String> OnSuccessListener,
                                              String cid, boolean isChecked) {
        /*CRIMClient.getInstance().conversationManager.setConversationPrivateChat(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().onErr(error);
            }

            @Override
            public void onSuccess(String data) {
                OnSuccessListener.onSuccess(data);
            }
        }, cid, isChecked);*/
    }

    /**
     * 更新常联系
     *
     * @param data
     */
    private void updateFrequentContacts(List<ConversationInfo> data) {
        List<UserInfo> uList = new ArrayList<>();
        for (ConversationInfo datum : data) {
            if (datum.getConversationType() == ConversationType.SINGLE_CHAT) {
                UserInfo u = new UserInfo();
                u.setUserID(datum.getUserID());
                u.setNickname(datum.getShowName());
                u.setFaceURL(datum.getFaceURL());
                uList.add(u);
            }
        }
        if (uList.isEmpty()) return;
        frequentContacts.setValue(uList.size() > 15 ? uList.subList(0, 15) : uList);
    }

//    @RequiresApi(api = Build.VERSION_CODES.N)
//    private void insertDBContact(List<ConversationInfo> data) {
//        RealmList<UserInfoDB> uList = new RealmList<>();
//        for (ConversationInfo datum : data) {
//            if (datum.getConversationType() == Constant.SessionType.SINGLE_CHAT) {
//                UserInfoDB u = new UserInfoDB();
//                u.setUserID(datum.getUserID());
//                u.setNickname(datum.getShowName());
//                u.setFaceURL(datum.getFaceURL());
//                uList.add(u);
//            }
//        }
//        if (uList.isEmpty()) return;
//        BaseApp.inst().realm.executeTransactionAsync(realm -> {
//            RealmResults<UserInfoDB> realmResults = realm.where(UserInfoDB.class).findAll();
//            if (realmResults.isEmpty()) {
//                realm.insert(uList.size() > 15 ? uList.subList(0, 15) : uList);
//            } else {
//                realm.where(UserInfoDB.class)
//                    .in("userID", (String[]) uList.stream()
//                        .map(UserInfoDB::getUserID)
//                        .distinct().toArray()).findAll().deleteAllFromRealm();
//
//                for (UserInfoDB userInfoDB : uList) {
//                    UserInfoDB task = realm.where(UserInfoDB.class)
//                        .equalTo("userID", userInfoDB.getUserID()).findFirst();
//                    if (null != task)
//                        task.deleteFromRealm();
//                }
//            }
//        });
//    }

    @Override
    public void onConversationChanged(List<ConversationInfo> list) {
        for (ConversationInfo info : list) {
            Message message = GsonHel.fromJson(info.getLatestMsg(),
                Message.class);
            MsgConversation msgConversation =
                new MsgConversation(message, info);
            int index = conversations.val()
                .indexOf(msgConversation);
            if (index != -1) {
                conversations.val().set(index, msgConversation);
                subject(NOTIFY_ITEM_CHANGED, index);
            }
        }
        sortConversation(list);
    }

    private void sortConversation(List<ConversationInfo> list) {
        List<MsgConversation> msgConversations = new ArrayList<>();
        for (ConversationInfo info : list) {
            Iterator<MsgConversation> iterator = conversations.val().iterator();
            Message message = GsonHel.fromJson(info.getLatestMsg(),
                Message.class);
            msgConversations.add(new MsgConversation(message, info));
            while (iterator.hasNext()) {
                if (iterator.next().conversationInfo.getConversationID()
                    .equals(info.getConversationID()))
                    iterator.remove();
            }
        }
        conversations.val().addAll(msgConversations);
        Collections.sort(conversations.val(), IMUtil.simpleComparator());
        conversations.setValue(conversations.val());
    }

    public void getSelfUserInfo() {
        CRIMClient.getInstance().userInfoManager.getSelfInfo(new OnBase<UserInfo>() {
            @Override
            public void onError(int code, String error) {
            }

            @Override
            public void onSuccess(UserInfo data) {
                nickName.setValue(data.getNickname());
            }
        });

    }

    @Override
    public void onNewConversation(List<ConversationInfo> list) {
        sortConversation(list);
    }

    @Override
    public void onSyncServerFailed(boolean b) {

    }

    @Override
    public void onSyncServerFinish(boolean b) {

    }

    @Override
    public void onSyncServerStart(boolean b) {

    }

    @Override
    public void onSyncServerProgress(long l) {

    }


    @Override
    public void onTotalUnreadMessageCountChanged(int i) {

    }

    @Override
    public void onRecvNewMsg(Message msg) {
        logcat(TAG + "===onRecvNewMsg===" + JsonUtil.toString(msg));
        if (BaseApp.inst().loginCertificate.userID.equals(msg.getRecvID())) {
            CustomElemData info = BeanUtil.getCustomElemData(msg);
            if (info != null) {
                logcat(TAG + "=SignalingInvitationInfo.getCustomType=" + info.getCustomType());
                if (info.getCustomType()==CustomMsgType.NewInvitation && info.getData()!=null) {
                    writeLog(TAG + ": recv call msg");
                    if (isCalling) {
                        rejectCall(msg);
                    } else {
                        newInvitationMsg = msg;
                        ARouter.getInstance().build(Routes.Conversation.CALL)
                            /*.withString(Constant.K_ID, msg.getSendID())
                            .withString(Constant.K_NAME, msg.getSenderNickname())
                            .withString(Constant.K_FACE_URL, msg.getSenderFaceUrl())
                            .withString(Constant.K_GROUP_ID, msg.getGroupID())
                            .withString(Constant.K_MEDIA_TYPE, info.getData().getMediaType())
                            .withString(Constant.K_ROOM_ID, info.getData().getRoomID())
                            .withString(Constant.K_INVITATION_MSG_ID, info.getData().getInvitationMsgID())*/
                            .withBoolean(Constant.CALL_INCOMING, true)
                            .navigation();
                    }
                }
            }
        }
    }

    public void rejectCall(Message inviteMsg) {
        Message message = CRIMClient.getInstance().signalingManager.createSignalingReject(inviteMsg,InvitationOpType.CRIVT_OP_Busy);
        writeLog(TAG + ": rejectCall message: " + JsonUtil.toString(message));
        logcat(TAG + ": rejectCall message=" + JsonUtil.toString(message));
        CRIMClient.getInstance().messageManager.sendMsg(new OnMsgSendCallback() {
            @Override
            public void onError(int code, String error) {
                logcat("rejectCall===onError=code: " + code + " error: " + error);
                writeLog(TAG + ": rejectCall error: " + error);
            }

            @Override
            public void onProgress(long l) {

            }

            @Override
            public void onSuccess(Message message) {
                logcat("rejectCall===success=" + JsonUtil.toString(message));
                writeLog(TAG + ": rejectCall success");
            }
        }, message, inviteMsg.getSendID(), inviteMsg.getGroupID(), new OfflinePushInfo(), false);
    }

    @Override
    public void onRecv1v1ReadReceipt(List<ReadReceiptInfo> list) {

    }

    @Override
    public void onRecvGrpReadReceipt(List<ReadReceiptInfo> list) {

    }

    @Override
    public void onRecvMsgRevokedV2(RevokedInfo info) {

    }

    @Override
    public void onRecvMsgExtensionsChanged(String msgID, List<KeyValue> list) {

    }

    @Override
    public void onRecvMsgExtensionsDeleted(String msgID, List<String> list) {

    }

    @Override
    public void onRecvMsgExtensionsAdded(String msgID, List<KeyValue> list) {

    }

    @Override
    public void onMsgDeleted(Message message) {

    }

    @Override
    public void onRecvOfflineNewMessage(List<Message> msg) {

    }

    @Override
    public void onRecvOnlineOnlyMsg(List<Message> list) {

    }

    //置顶/取消置顶 会话
    public void pinConversation(ConversationInfo conversationInfo, boolean isPinned) {
        CRIMClient.getInstance().conversationManager.pinConversation(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
            }

            @Override
            public void onSuccess(String data) {
                conversationInfo.setPinned(isPinned);
            }
        }, conversationInfo.getConversationID(), isPinned);
    }

    public interface ViewAction extends IView {
        void onErr(String msg);
    }
}
