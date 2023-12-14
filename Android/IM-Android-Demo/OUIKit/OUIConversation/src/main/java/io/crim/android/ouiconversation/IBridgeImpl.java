package io.crim.android.ouiconversation;

import android.content.Context;

import com.alibaba.android.arouter.facade.annotation.Route;

import androidx.lifecycle.LifecycleOwner;
import io.crim.android.sdk.models.ConversationInfo;
import io.crim.android.ouiconversation.vm.ChatVM;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.entity.MsgConversation;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.services.IConversationBridge;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.ContactListVM;

@Route(path = Routes.Service.CONVERSATION)
public class IBridgeImpl implements IConversationBridge {

    @Override
    public void init(Context context) {

    }


    @Override
    public void deleteConversationFromLocalAndSvr(String groupID) {
        ContactListVM contactListVM = BaseApp.inst().getVMByCache(ContactListVM.class);
        if (null != contactListVM) {
            String conversationID = "";
            for (MsgConversation msgConversation : contactListVM.conversations.getValue()) {
                if (msgConversation.conversationInfo.getGroupID().equals(groupID))
                    conversationID = msgConversation.conversationInfo.getConversationID();
            }
            if (!conversationID.isEmpty())
                contactListVM.deleteConversationAndDeleteAllMsg(conversationID);
        }

    }


    @Override
    public void closeChatPage() {
        ChatVM chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
        if (null != chatVM) {
            chatVM.closePage();
        }
    }


    @Override
    public void setConversationRecvMessageOpt(int status, String cid) {
        ChatVM chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
        if (null != chatVM)
            chatVM.setConversationRecvMessageOpt(status, cid);
    }

    @Override
    public void setNotDisturbStatusListener(LifecycleOwner owner, IMUtil.OnSuccessListener<Integer> OnSuccessListener) {
        try {
            ChatVM chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
            if (null != chatVM) {
                chatVM.notDisturbStatus.observe((LifecycleOwner) chatVM.getContext(), integer -> {
                    OnSuccessListener.onSuccess(integer);
                });
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    @Override
    public ConversationInfo getConversationInfo() {
        try {
            ChatVM chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
            if (null != chatVM) {
                return chatVM.conversationInfo.getValue();
            }
        } catch (Exception e) {
            e.printStackTrace();

        }
        return null;
    }

    @Override
    public void pinConversation(ConversationInfo conversationInfo, boolean is) {
        ContactListVM contactListVM = BaseApp.inst().getVMByCache(ContactListVM.class);
        if (null != contactListVM) {
            contactListVM.pinConversation(conversationInfo, is);
        }
    }

    @Override
    public void setConversationInfoChangeListener(LifecycleOwner owner, IMUtil.OnSuccessListener<ConversationInfo> OnSuccessListener) {
        ChatVM chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
        if (null != chatVM) {
            chatVM.conversationInfo.observe(owner, conversationInfo -> {
                OnSuccessListener.onSuccess(conversationInfo);
            });
        }
    }

    @Override
    public void clearCHistory(String id) {
        ChatVM chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
        if (null != chatVM) {
            chatVM.clearCHistory(id);
        }
    }


}
