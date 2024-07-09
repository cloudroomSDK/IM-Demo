package io.crim.android.demo.services;

import android.content.Context;
import android.util.Log;

import com.igexin.sdk.GTIntentService;
import com.igexin.sdk.PushConsts;
import com.igexin.sdk.message.BindAliasCmdMessage;
import com.igexin.sdk.message.GTCmdMessage;
import com.igexin.sdk.message.GTNotificationMessage;
import com.igexin.sdk.message.GTTransmitMessage;

/**
 * Created by zjw on 2024/1/11.
 */
public class DemoIntentService extends GTIntentService {

    @Override
    public void onReceiveServicePid(Context context, int pid) {
    }

    /**
     * 此方法用于接收和处理透传消息。透传消息个推只传递数据，不做任何处理，客户端接收到透传消息后需要自己去做后续动作处理，如通知栏展示、弹框等。
     * 如果开发者在客户端将透传消息创建了通知栏展示，建议将展示和点击回执上报给个推。
     */
    @Override
    public void onReceiveMessageData(Context context, GTTransmitMessage msg) {
        byte[] payload = msg.getPayload();
        String data = new String(payload);
        Log.d(TAG, "receiver payload = " + data);//透传消息文本内容

        //taskid和messageid字段，是用于回执上报的必要参数。详情见下方文档“6.2 上报透传消息的展示和点击数据”
        String taskid = msg.getTaskId();
        String messageid = msg.getMessageId();

    }

    // 接收 cid
    @Override
    public void onReceiveClientId(Context context, String clientid) {
        Log.e(TAG, "onReceiveClientId -> " + "clientid = " + clientid);
    }

    // cid 离线上线通知
    @Override
    public void onReceiveOnlineState(Context context, boolean online) {
    }

    // 各种事件处理回执
    @Override
    public void onReceiveCommandResult(Context context, GTCmdMessage cmdMessage) {
        Log.d(TAG, "onReceiveCommandResult -> " + cmdMessage);
        int action = cmdMessage.getAction();

        if (action == PushConsts.BIND_ALIAS_RESULT) {
            String sn = ((BindAliasCmdMessage) cmdMessage).getSn();
            String code = ((BindAliasCmdMessage) cmdMessage).getCode();
        /*  code 结果说明
            0：成功
            10099：SDK 未初始化成功
            30001：绑定别名失败，频率过快，两次调用的间隔需大于 1s
            30002：绑定别名失败，参数错误
            30003：当前 cid 绑定别名次数超限
            30004：绑定别名失败，未知异常
            30005：绑定别名时，cid 未获取到
            30006：绑定别名时，发生网络错误
            30007：别名无效
            30008：sn 无效 */
            Log.d(TAG, "bind alias result sn = " + sn + ", code = " + code);
        }
    }

    // 通知到达，只有个推通道下发的通知会回调此方法
    @Override
    public void onNotificationMessageArrived(Context context, GTNotificationMessage msg) {
    }

    // 通知点击，只有个推通道下发的通知会回调此方法
    @Override
    public void onNotificationMessageClicked(Context context, GTNotificationMessage msg) {
    }
}
