package io.crim.android.ouicore.vm;

import java.util.List;

import io.crim.android.ouicore.R;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.vm.injection.BaseVM;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.MsgType;
import io.crim.android.sdk.models.Message;

public class ForwardVM extends BaseVM {
    public Message forwardMsg;
    //转发的内容 用于显示
    public String tips;
    //留言
    public Message leaveMsg;
    public String cardNickname;

    public void createLeaveMsg(String leave) {
        leaveMsg = CRIMClient.getInstance().messageManager.createTextMsg(leave);
    }

    public void createForwardMessage(Message message) {
        reset();
        tips = IMUtil.getMsgParse(message).toString();
        if (message.getContentType()== MsgType.CARD){
            cardNickname = message.getCardElem().getNickname();
        }
        forwardMsg = CRIMClient.getInstance().messageManager.createForwardMsg(message);
    }

    public void createMergerMessage(boolean isSingleChat, String showName, List<Message> messages) {
        reset();
        if (isSingleChat) {
            tips =
                LoginCertificate.getCache(BaseApp.inst()).nickName + BaseApp.inst().getString(R.string.and) + showName + BaseApp.inst().getString(R.string.chat_history);
        } else {
            tips = BaseApp.inst().getString(R.string.group_chat_history);
        }
        forwardMsg = IMUtil.createMergerMessage(tips, messages);
    }

    public void reset() {
        forwardMsg = null;
        leaveMsg=null;
        tips="";
        cardNickname="";
    }
}
