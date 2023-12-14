package io.crim.android.ouicore.services;

import com.alibaba.android.arouter.facade.template.IProvider;

import androidx.lifecycle.LifecycleOwner;
import io.crim.android.sdk.models.ConversationInfo;
import io.crim.android.ouicore.im.IMUtil;

public interface IConversationBridge extends IProvider {
    void deleteConversationFromLocalAndSvr(String groupID);

    void closeChatPage();


    void setConversationRecvMessageOpt(int status, String cid);

    void setNotDisturbStatusListener(LifecycleOwner owner, IMUtil.OnSuccessListener<Integer> OnSuccessListener);

    ConversationInfo getConversationInfo();

    void pinConversation(ConversationInfo conversationInfo, boolean is);

    void setConversationInfoChangeListener(LifecycleOwner owner, IMUtil.OnSuccessListener<ConversationInfo> OnSuccessListener);

    void clearCHistory(String id);
}
