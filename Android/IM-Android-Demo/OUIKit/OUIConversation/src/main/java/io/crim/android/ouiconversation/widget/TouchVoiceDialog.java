package io.crim.android.ouiconversation.widget;

import android.content.Context;
import android.net.Uri;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;

import java.io.File;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.crim.android.ouiconversation.R;
import io.crim.android.ouiconversation.audio.AudioRecordManager;
import io.crim.android.ouiconversation.audio.IAudioRecordListener;
import io.crim.android.ouiconversation.databinding.LayoutTouchVoiceBinding;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseDialog;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.Constant;

public class TouchVoiceDialog extends BaseDialog {
    //和布局里保持一致
    private static final float VOICE_HEIGHT = 129;
    public static final int TYPE_RECORD = 0;
    public static final int TYPE_CANCEL = 1;
    public static final int TYPE_TO_TEXT = 2;

    public TouchVoiceDialog(@NonNull Context context) {
        super(context);
        initView();
    }

    public TouchVoiceDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
        initView();
    }

    protected TouchVoiceDialog(@NonNull Context context, boolean cancelable, @Nullable OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
        initView();
    }

    private int screenWidth, screenHeight;
    private LayoutTouchVoiceBinding view;
    private int audioDB = 0;

    private void initView() {
        view = LayoutTouchVoiceBinding.inflate(getLayoutInflater());
        setContentView(view.getRoot());

        WindowManager wm = (WindowManager) BaseApp.inst()
            .getSystemService(Context.WINDOW_SERVICE);
        screenWidth = wm.getDefaultDisplay().getWidth();
        screenHeight = wm.getDefaultDisplay().getHeight();

        WindowManager.LayoutParams params = getWindow().getAttributes();
        params.gravity = Gravity.CENTER;
        params.height = ViewGroup.LayoutParams.MATCH_PARENT;
        params.width = ViewGroup.LayoutParams.MATCH_PARENT;
        params.dimAmount = 0.0f;
        getWindow().setAttributes(params);
        getWindow().setBackgroundDrawableResource(android.R.color.transparent);

        //默认时长秒
        AudioRecordManager.getInstance(getContext()).setMaxVoiceDuration(60);
        //该库内不对文件夹是否存在进行判断，所以请在你的项目中自行判断
        File mAudioDir = new File(Constant.AUDIO_DIR);
        if (!mAudioDir.exists()) {
            mAudioDir.mkdirs();
        }
        AudioRecordManager.getInstance(getContext()).setAudioSavePath(mAudioDir.getAbsolutePath());

        AudioRecordManager.getInstance(getContext()).setAudioRecordListener(new IAudioRecordListener() {
            @Override
            public void initTipView() {

            }

            @Override
            public void setTimeoutTipView(int counter) {
            }

            @Override
            public void setRecordingTipView() {

            }

            @Override
            public void setAudioShortTipView() {

            }

            @Override
            public void setCancelTipView() {

            }

            @Override
            public void destroyTipView() {

            }

            @Override
            public void onStartRecord() {

            }

            @Override
            public void onFinish(Uri audioPath, int duration) {
                if (null != onSelectResultListener) {
                    onSelectResultListener.result(code, audioPath, duration);
                }
                dismiss();
            }

            @Override
            public void onAudioDBChanged(int db) {
                audioDB = db;
            }
        });
        setOnAudioDBChanged();
    }

    @Override
    public void show() {
        super.show();
        initRecord();
        AudioRecordManager.getInstance(getContext()).startRecord();
    }

    @Override
    public void dismiss() {
        AudioRecordManager.getInstance(getContext()).stopRecord();
        if (code == 1 || code == 2)
            AudioRecordManager.getInstance(getContext()).destroyRecord();

        super.dismiss();
    }

    /**
     * 0 录音 1 取消录音 2 录音转文字
     */
    private int code = 0;

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        float x = event.getRawX();
        float y = event.getRawY();
        if (event.getAction() == MotionEvent.ACTION_MOVE) {
            /*if (x > (screenWidth / 2)) {
                view.voice.setBackground(getContext().getDrawable(R.mipmap.bg_chat_voice2));
                view.speechTxt.setVisibility(View.VISIBLE);
                view.cancelTips.setVisibility(View.GONE);
                view.voiceCancel.setVisibility(View.GONE);
                view.recordTips.setVisibility(View.GONE);
                view.sendTips.setVisibility(View.GONE);
                code = TYPE_TO_TEXT;
            } else {
                view.voice.setBackground(getContext().getDrawable(R.mipmap.bg_chat_voice2));
                view.voiceCancel.setVisibility(View.VISIBLE);
                view.cancelTips.setVisibility(View.VISIBLE);
                view.speechTxt.setVisibility(View.GONE);
                view.recordTips.setVisibility(View.GONE);
                view.sendTips.setVisibility(View.GONE);
                code = TYPE_CANCEL;
            }*/
            view.voice.setBackground(getContext().getDrawable(R.mipmap.bg_chat_voice2));
            view.voiceCancel.setVisibility(View.VISIBLE);
            view.cancelTips.setVisibility(View.VISIBLE);
            view.speechTxt.setVisibility(View.GONE);
            view.recordTips.setVisibility(View.GONE);
            view.sendTips.setVisibility(View.GONE);
            code = TYPE_CANCEL;
            if (y > (screenHeight - Common.dp2px(VOICE_HEIGHT))) {
                initRecord();
                code = TYPE_RECORD;
            }
        }
        if (event.getAction() == MotionEvent.ACTION_UP) {
            dismiss();
        }
        return true;
    }

    @Override
    public boolean onTouchEvent(@NonNull MotionEvent event) {
        return true;
    }

    //show 时视图初始化
    private void initRecord() {
        view.voice.setBackground(getContext().getDrawable(R.mipmap.bg_chat_voice));
        view.recordTips.setVisibility(View.VISIBLE);
        view.sendTips.setVisibility(View.VISIBLE);
        view.voiceCancel.setVisibility(View.GONE);
        view.cancelTips.setVisibility(View.GONE);
        view.speechTxt.setVisibility(View.GONE);
    }

    private OnSelectResultListener onSelectResultListener;

    public TouchVoiceDialog setOnSelectResultListener(OnSelectResultListener onSelectResultListener) {
        this.onSelectResultListener = onSelectResultListener;
        return this;
    }


    public void setOnAudioDBChanged() {
        view.recordWaveView.setAmpListener(() -> audioDB);
        view.cancelRecordWaveView.setAmpListener(() -> audioDB);
    }


    public interface OnSelectResultListener {
        /**
         * @param code      0 录音 1 取消录音 2 录音转文字
         * @param audioPath 只有code=0时才有效
         * @param duration  只有code=0时才有效
         */
        void result(int code, Uri audioPath, int duration);
    }
}
