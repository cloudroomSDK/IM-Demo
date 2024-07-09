package io.crim.android.ouicore.entity;

import java.util.Objects;

import io.crim.android.sdk.enums.MsgType;
import io.crim.android.sdk.models.ConversationInfo;
import io.crim.android.sdk.models.Message;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.net.bage.GsonHel;

//解析最后的消息
public class MsgConversation {
    public CharSequence lastMsg;
    public ConversationInfo conversationInfo;
    public NotificationMsg notificationMsg;

    public MsgConversation(Message lastMsg, ConversationInfo conversationInfo) {
        if (null != lastMsg) {
            IMUtil.buildExpandInfo(lastMsg);
            this.lastMsg = IMUtil.getMsgParse(lastMsg);
        } else
            this.lastMsg = "";
        this.conversationInfo = conversationInfo;
        try {
            if (lastMsg.getContentType() == MsgType.GROUP_INFO_SET_NTF) {
                notificationMsg = GsonHel.fromJson(lastMsg.getNotificationElem().getDetail(),
                    NotificationMsg.class);
            }
        } catch (Exception ignored) {}
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof MsgConversation)) return false;
        MsgConversation that = (MsgConversation) o;
        return conversationInfo.equals(that.conversationInfo);
    }

    @Override
    public int hashCode() {
        return Objects.hash(conversationInfo.getConversationID());
    }
}
