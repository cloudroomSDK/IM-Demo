package io.crim.android.ouiconversation.ui;

import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.view.View;

import com.alibaba.android.arouter.facade.annotation.Route;
import com.rtc.sdk.RTCMeeting;
import com.rtc.sdk.model.MemberInfo;
import com.rtc.sdk.model.UsrVideoId;
import com.rtc.sdk.model.VSTATUS;

import java.util.ArrayList;

import androidx.annotation.Nullable;
import androidx.lifecycle.Observer;
import io.crim.android.ouiconversation.BuildConfig;
import io.crim.android.ouiconversation.R;
import io.crim.android.ouiconversation.databinding.ActivityCallBinding;
import io.crim.android.ouiconversation.vm.CallVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.utils.BeanUtil;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.ContactListVM;
import io.crim.android.sdk.models.CustomElemData;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.utils.JsonUtil;

/**
 * Created by zjw on 2024/9/18.
 */
@Route(path = Routes.Conversation.CALL)
public class CallActivity extends BaseActivity<CallVM, ActivityCallBinding> implements CallVM.CallViewAction {

    private final static String TAG = "CallActivity";
    private CountDownTimer callTimer = null;
    private MediaPlayer mMediaPlayer = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        logcat(TAG + " onCreate");
        bindVM(CallVM.class);
        vm.setCallState(true);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityCallBinding.inflate(getLayoutInflater()));
        sink();
        view.setCallVM(vm);
        vm.init();
        Intent intent = getIntent();
        vm.callIncoming = intent.getBooleanExtra(Constant.CALL_INCOMING, false);
        ContactListVM contactListVM = BaseApp.inst().getVMByCache(ContactListVM.class);
        if (vm.callIncoming && contactListVM != null) {
            vm.newInvitationMsg = JsonUtil.toObj(JsonUtil.toString(contactListVM.newInvitationMsg), Message.class);
            if (vm.newInvitationMsg == null) {
                toast("no invitation message");
                finish();
                return;
            }
            vm.userID = vm.newInvitationMsg.getSendID();
            vm.groupID = vm.newInvitationMsg.getGroupID();
            vm.nickname = vm.newInvitationMsg.getSenderNickname();
            vm.faceUrl = vm.newInvitationMsg.getSenderFaceUrl();
            CustomElemData customElemData = BeanUtil.getCustomElemData(vm.newInvitationMsg);
            vm.mediaType = customElemData.getData().getMediaType();
            vm.invitationMsgId = customElemData.getData().getInvitationMsgID();
            String room = customElemData.getData().getRoomID();
            if (room != null) {
                vm.roomID = Integer.parseInt(room);
            }
        }else{
            vm.userID = intent.getStringExtra(Constant.K_ID);
            vm.groupID = intent.getStringExtra(Constant.K_GROUP_ID);
            vm.nickname = intent.getStringExtra(Constant.K_NAME);
            vm.faceUrl = intent.getStringExtra(Constant.K_FACE_URL);
            vm.mediaType = intent.getStringExtra(Constant.K_MEDIA_TYPE);
            vm.invitationMsgId = intent.getStringExtra(Constant.K_INVITATION_MSG_ID);
            String room = intent.getStringExtra(Constant.K_ROOM_ID);
            if (room != null) {
                vm.roomID = Integer.parseInt(room);
            }
        }
        initListener();
        initView();
        vm.login();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        logcat(TAG + " onNewIntent");
    }

    @Override
    protected void onResume() {
        super.onResume();
        logcat(TAG + " onResume");
    }

    private void initView() {
        if (vm.callIncoming) {
            view.tvTip.setVisibility(View.GONE);
            if (!BuildConfig.DEBUG) {
                mMediaPlayer = MediaPlayer.create(this, R.raw.ring);
                mMediaPlayer.setLooping(true);
                mMediaPlayer.start();
            }
        } else {
            view.ivAccept.setVisibility(View.GONE);
        }
        view.tvName.setText(vm.nickname);
        view.avatar.load(vm.faceUrl);
        view.videoPeer.setScaleType(1);
//        view.callContainer.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
    }

    private void initListener() {
        view.ivAccept.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                releaseMediaPlayer();
                vm.accept();
            }
        });
        view.ivHangUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                logcat("ivHangUp click meetSecond="+vm.meetSecond + " meetMinute=" + vm.meetMinute+" callIncoming="+vm.callIncoming);
                if (vm.meetSecond > 0 || vm.meetMinute > 0) {
                    vm.hangup();
                } else if (vm.callIncoming) {
                    vm.rejectCall();
                } else {
                    vm.cancelCall();
                }
            }
        });
        view.flMic.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                vm.switchMic();
            }
        });
        view.flReverse.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                view.videoSelf.setUsrVideoId(vm.reverseCam());
            }
        });
        view.flCam.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                vm.switchCam();
            }
        });
        view.ivSpeaker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                vm.setSpeakerMute();
            }
        });
        vm.micOpen.observe(this, new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean aBoolean) {
                if (aBoolean) {
                    view.ivMic.setImageResource(R.mipmap.meet_mic);
                } else {
                    view.ivMic.setImageResource(R.mipmap.meet_mic_close);
                }
            }
        });
        vm.camOpen.observe(this, new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean aBoolean) {
                if (aBoolean) {
                    view.ivCam.setImageResource(R.mipmap.camera_meet);
                } else {
                    view.ivCam.setImageResource(R.mipmap.camera_meet_close);
                }
            }
        });
        vm.speakerMute.observe(this, new Observer<Boolean>() {
            @Override
            public void onChanged(Boolean aBoolean) {
                if (aBoolean) {
//                    toast("扬声器已关闭");
                    view.ivSpeaker.setImageResource(R.mipmap.voice_mute);
                } else {
//                    toast("扬声器已开启");
                    view.ivSpeaker.setImageResource(R.mipmap.voice_add);
                }
            }
        });
    }

    @Override
    public void err2Finish(String err, String cookie) {
        toast(err);
        finish();
    }

    @Override
    public void hangupCallSuccess() {
        finish();
    }

    @Override
    public void cancelCallSuccess() {
        finish();
    }

    @Override
    public void rejectCallSuccess() {
        finish();
    }

    @Override
    public void enterMeetingSuccess() {
        if (vm.callIncoming) {
            view.ivAccept.setVisibility(View.GONE);
            view.tvDurTime.setVisibility(View.VISIBLE);
            ArrayList<MemberInfo> members = RTCMeeting.getInstance().getAllMembers();
            for (MemberInfo member : members) {
                if (!member.userId.equals(RTCMeeting.getInstance().getMyUserID())) {
                    updateVideoWall(member.userId);
                    break;
                }
            }
        } else {
            releaseCallTimer();
            callTimer = new CountDownTimer(1000 * vm.timeout, 1000) {
                @Override
                public void onTick(long l) {

                }

                @Override
                public void onFinish() {
                    logcat("callTimer onFinish");
                    vm.cancelCall();
                }
            };
            callTimer.start();
            logcat("callTimer.start()=vm.timeout=" + vm.timeout);
        }
        view.flMic.setVisibility(View.VISIBLE);
        view.flSpeaker.setVisibility(View.VISIBLE);
        if (vm.mediaType.equals(Constant.MediaType.VIDEO)) {
            view.flCam.setVisibility(View.VISIBLE);
            view.flReverse.setVisibility(View.VISIBLE);
            view.videoSelf.setVisibility(View.VISIBLE);
            view.videoSelf.setUsrVideoId(new UsrVideoId(BaseApp.inst().loginCertificate.userID));
            if (vm.callIncoming) {
                hideInfoView();
            }
        }
    }

    @Override
    public void acceptCall() {
        releaseCallTimer();
        view.tvDurTime.setVisibility(View.VISIBLE);
        view.tvTip.setVisibility(View.GONE);
        if (vm.mediaType.equals(Constant.MediaType.VIDEO)) {
            hideInfoView();
        }
    }

    private void hideInfoView() {
        view.avatar.setVisibility(View.GONE);
        view.tvName.setVisibility(View.GONE);
        view.tvTip.setVisibility(View.GONE);
    }

    @Override
    public void hangupCall() {
        toast("对方已挂断");
        finish();
    }

    @Override
    public void cancelCall() {
        toast("对方已取消");
        finish();
    }

    @Override
    public void rejectCall() {
        toast("对方已拒绝");
        finish();
    }

    @Override
    public void userEnterMeeting(String userID) {
        if (userID.equals(vm.userID) || !userID.equals(RTCMeeting.getInstance().getMyUserID())) {
            updateVideoWall(userID);
        }
    }

    @Override
    public void userLeftMeeting(String userID) {
        logcat("userLeftMeeting====" + userID);
    }

    @Override
    public void videoStatusChanged(String userID, VSTATUS oldStatus, VSTATUS newStatus) {
        logcat("videoStatusChanged====" + userID + "  oldStatus=" + oldStatus + "=newStatus=" + newStatus + "=vm.userID=" + vm.userID);
        if (userID.equals(vm.userID) || !userID.equals(RTCMeeting.getInstance().getMyUserID())) {
            if (newStatus == VSTATUS.VOPEN || newStatus == VSTATUS.VOPENING) {
                view.videoPeer.setVisibility(View.VISIBLE);
            } else {
                view.videoPeer.setVisibility(View.GONE);
            }
            updateVideoWall(userID);
        }
    }

    @Override
    public void defVideoChanged(String userID, short videoID) {
        logcat("defVideoChanged===userID=" + userID + "==videoID=" + videoID + "=vm.userID=" + vm.userID);
        if (userID.equals(vm.userID) || !userID.equals(RTCMeeting.getInstance().getMyUserID())) {
            updateVideoWall(userID);
        }
    }

    @Override
    public void meetDurTimeChange(String time) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                vm.meetDurTime.setValue(time);
            }
        });
    }

    private void updateVideoWall(String userID) {
        /*ArrayList<MemberInfo> members = vm.getAllMembers();
        for (MemberInfo member : members) {
            logcat("updateVideoWall===member.userId==" + member.userId + "==MY ID=" + BaseApp.inst().loginCertificate.userID + "====duifang  ID==" + vm.userID);
        }*/
        logcat("updateVideoWall==setUsrVideoId=" + userID);
        view.videoPeer.setUsrVideoId(new UsrVideoId(userID));
    }

    @Override
    protected void onDestroy() {
        releaseCallTimer();
        releaseMediaPlayer();
        super.onDestroy();
        vm.setCallState(false);
    }

    private void releaseMediaPlayer() {
        if (mMediaPlayer != null) {
            if (mMediaPlayer.isPlaying()) {
                mMediaPlayer.pause();
            }
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    private void releaseCallTimer() {
        logcat("releaseCallTimer===");
        if (callTimer != null) {
            callTimer.cancel();
            callTimer = null;
        }
    }
}
