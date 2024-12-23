package io.crim.android.ouiconversation.widget;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.text.Editable;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.text.style.ForegroundColorSpan;
import android.text.style.ImageSpan;
import android.view.KeyEvent;
import android.view.View;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.util.ArrayList;
import java.util.List;

import androidx.fragment.app.FragmentTransaction;
import androidx.lifecycle.LifecycleOwner;
import io.crim.android.ouiconversation.R;
import io.crim.android.ouiconversation.databinding.LayoutInputCoteBinding;
import io.crim.android.ouiconversation.ui.emoji.EmojiFragment;
import io.crim.android.ouiconversation.ui.fragment.InputExpandFragment;
import io.crim.android.ouiconversation.vm.ChatVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.entity.MsgExpand;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.EmojiUtil;
import io.crim.android.ouicore.utils.MediaFileUtil;
import io.crim.android.ouicore.utils.OnDedrepClickListener;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.ConversationType;
import io.crim.android.sdk.enums.GrpStatus;
import io.crim.android.sdk.models.AtUserInfo;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.models.SoundElem;

import static android.view.View.GONE;
import static android.view.View.VISIBLE;

/**
 * 聊天页面底部输入栏
 */
public class BottomInputCote {

    private boolean hasMicrophone = false;
    private ChatVM vm;
    private Context context;
    private TouchVoiceDialog touchVoiceDialog = null;
    InputExpandFragment inputExpandFragment;
    EmojiFragment mEmojiFragment;
    public LayoutInputCoteBinding view;
    //是否可发送内容
    private boolean isSend;


    @SuppressLint("WrongConstant")
    public BottomInputCote(Context context, LayoutInputCoteBinding view) {
        this.context = context;
        this.view = view;

        view.root.setIntercept(false);

        initFragment();
        Common.UIHandler.postDelayed(() -> hasMicrophone = AndPermission.hasPermissions(context,
            Permission.Group.MICROPHONE), 300);
        view.tvSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!isSend) {
                    return;
                }
                List<Message> atMessages = vm.atMessages.getValue();
                final Message msg;
                if (null != vm.replyMessage.getValue()) {
                    msg =
                        CRIMClient.getInstance().messageManager
                            .createQuoteMsg(vm.inputMsg.getValue(), vm.replyMessage.getValue());
                } else if (atMessages.isEmpty())
                    msg =
                        CRIMClient.getInstance().messageManager.createTextMsg(vm.inputMsg.getValue());
                else {
                    List<String> atUserIDList = new ArrayList<>();
                    List<AtUserInfo> atUserInfoList = new ArrayList<>();

                    Editable msgEdit = view.chatInput.getText();
                    final ForegroundColorSpan spans[] = view.chatInput.getText().getSpans(0,
                        view.chatInput.getText().length(), ForegroundColorSpan.class);
                    for (Message atMessage : atMessages) {
                        atUserIDList.add(atMessage.getSendID());
                        AtUserInfo atUserInfo = new AtUserInfo();
                        atUserInfo.setAtUserID(atMessage.getSendID());
                        atUserInfo.setGroupNickname(atMessage.getSenderNickname());
                        atUserInfoList.add(atUserInfo);

                        try {
                            for (ForegroundColorSpan span : spans) {
                                if (span == null) continue;
                                MsgExpand msgExpand = (MsgExpand) atMessage.getExt();
                                if (msgExpand.spanHashCode == span.hashCode()) {
                                    final int spanStart =
                                        view.chatInput.getText().getSpanStart(span);
                                    final int spanEnd = view.chatInput.getText().getSpanEnd(span);
                                    msgEdit.replace(spanStart, spanEnd,
                                        " @" + atMessage.getSendID() + " ");
                                }
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                    msg =
                        CRIMClient.getInstance().messageManager.createTextAtMsg(msgEdit.toString(), atUserIDList, atUserInfoList, null);
                }
                if (null != msg) {
                    if (msg.getSessionType() == ConversationType.UNDEFINE) {
                        if (vm.isSingleChat) {
                            msg.setSessionType(ConversationType.SINGLE_CHAT);
                        } else {
                            msg.setSessionType(ConversationType.SUPER_GROUP_CHAT);
                        }
                    }
                    vm.sendMsg(msg);
                    reset();
                }
            }
        });
        view.chatMoreOrSend.setOnClickListener(new OnDedrepClickListener() {
            @Override
            public void click(View v) {
//                if (!isSend) {
                view.voice.setChecked(false);
                clearFocus();
                Common.hideKeyboard(BaseApp.inst(), v);
                view.fragmentContainer.setVisibility(VISIBLE);
                switchFragment(inputExpandFragment);
//                    return;
//                }

                /*List<Message> atMessages = vm.atMessages.getValue();
                final Message msg;
                if (null != vm.replyMessage.getValue()) {
                    msg =
                        CRIMClient.getInstance().messageManager
                            .createQuoteMsg(vm.inputMsg.getValue(), vm.replyMessage.getValue());
                } else if (atMessages.isEmpty())
                    msg =
                        CRIMClient.getInstance().messageManager.createTextMsg(vm.inputMsg.getValue());
                else {
                    List<String> atUserIDList = new ArrayList<>();
                    List<AtUserInfo> atUserInfoList = new ArrayList<>();

                    Editable msgEdit = view.chatInput.getText();
                    final ForegroundColorSpan spans[] = view.chatInput.getText().getSpans(0,
                        view.chatInput.getText().length(), ForegroundColorSpan.class);
                    for (Message atMessage : atMessages) {
                        atUserIDList.add(atMessage.getSendID());
                        AtUserInfo atUserInfo = new AtUserInfo();
                        atUserInfo.setAtUserID(atMessage.getSendID());
                        atUserInfo.setGroupNickname(atMessage.getSenderNickname());
                        atUserInfoList.add(atUserInfo);

                        try {
                            for (ForegroundColorSpan span : spans) {
                                if (span == null) continue;
                                MsgExpand msgExpand = (MsgExpand) atMessage.getExt();
                                if (msgExpand.spanHashCode == span.hashCode()) {
                                    final int spanStart =
                                        view.chatInput.getText().getSpanStart(span);
                                    final int spanEnd = view.chatInput.getText().getSpanEnd(span);
                                    msgEdit.replace(spanStart, spanEnd,
                                        " @" + atMessage.getSendID() + " ");
                                }
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                    msg =
                        CRIMClient.getInstance().messageManager.createTextAtMsg(msgEdit.toString(), atUserIDList, atUserInfoList, null);
                }
                if (null != msg) {
                    vm.sendMsg(msg);
                    reset();
                }*/
            }
        });
        view.voice.setOnCheckedChangeListener((v, isChecked) -> {
            if (isChecked) {
                if (AndPermission.hasPermissions(v.getContext(), Permission.RECORD_AUDIO)) {
                    view.touchSay.setVisibility(VISIBLE);
                    view.inputLy.setVisibility(GONE);
                } else {
                    AndPermission.with(v.getContext()).runtime().permission(Permission.RECORD_AUDIO).onGranted(permissions -> {
                        // Storage permission are allowed.
                        view.touchSay.setVisibility(VISIBLE);
                        view.inputLy.setVisibility(GONE);
                    }).onDenied(permissions -> {
                        // Storage permission are not allowed.
                        view.inputLy.setVisibility(VISIBLE);
                        view.touchSay.setVisibility(GONE);
                        view.voice.setChecked(!isChecked);
                    }).start();
                }
            } else {
                view.inputLy.setVisibility(VISIBLE);
                view.touchSay.setVisibility(GONE);
            }
        });
        view.touchSay.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View view) {
                touchVoiceDialog = new TouchVoiceDialog(view.getContext());
                touchVoiceDialog.setOnSelectResultListener(new TouchVoiceDialog.OnSelectResultListener() {
                    @Override
                    public void result(int code, Uri audioPath, int duration) {
                        if (code == TouchVoiceDialog.TYPE_RECORD) {
                            SoundElem elem = new SoundElem();
                            elem.setSoundPath(audioPath.getPath());
                            elem.setDataSize(MediaFileUtil.getFileSize(view.getContext(), audioPath));
                            elem.setDuration(duration);
                            Message msg = CRIMClient.getInstance().messageManager.createSoundMsgByURL(elem);
                            vm.sendMsg(msg);
                        }
                    }
                }).show();
                return false;
            }
        });
        view.touchSay.setOnTouchListener((view1, event) -> {
            if (touchVoiceDialog != null) {
                touchVoiceDialog.dispatchTouchEvent(event);
            }
            return false;
        });
        view.chatInput.setOnFocusChangeListener((v, hasFocus) -> {
            if (hasFocus) setExpandHide();
//            InputMethodManager inputMethodManager =
//                (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
//            inputMethodManager.toggleSoftInput(0, InputMethodManager.HIDE_NOT_ALWAYS);
        });

        view.emoji.setOnClickListener(v -> {
            clearFocus();
            Common.hideKeyboard(BaseApp.inst(), v);
            view.fragmentContainer.setVisibility(VISIBLE);
            switchFragment(mEmojiFragment);
        });
        view.cancelReply.setOnClickListener(v -> vm.replyMessage.setValue(null));
        view.chatInput.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                boolean isSend = !TextUtils.isEmpty(s) && !Common.isBlank(s);
                setSendButton(isSend);
            }
        });

    }

    private void setSendButton(boolean isSend) {
        if (BottomInputCote.this.isSend == isSend) return;
//        view.chatMoreOrSend.setImageResource(isSend ? R.mipmap.ic_c_send : R.mipmap.ic_chat_add);
        if (isSend) {
            view.tvSend.setTextColor(Color.parseColor("#ffffff"));
            view.tvSend.setBackgroundResource(R.drawable.bg_blue);
        } else {
            view.tvSend.setTextColor(Color.parseColor("#888888"));
            view.tvSend.setBackgroundResource(R.drawable.bg_gray);
        }
        BottomInputCote.this.isSend = isSend;
    }

    //消息发出后重置UI
    private void reset() {
        vm.inputMsg.setValue("");
        view.chatInput.setText("");
        vm.atMessages.getValue().clear();
        vm.emojiMessages.getValue().clear();
        vm.replyMessage.setValue(null);
    }

    private void initFragment() {
        inputExpandFragment = new InputExpandFragment();
        mEmojiFragment = new EmojiFragment();
        inputExpandFragment.setPage(1);
        mEmojiFragment.setPage(2);
    }

    public void setEmojiClickListener(EmojiFragment.EmojiClickListener listener) {
        if (mEmojiFragment != null) {
            mEmojiFragment.setEmojiClickListener(listener);
        }
    }

    public void clearFocus() {
        view.chatInput.clearFocus();
    }

    public void setChatVM(ChatVM vm) {
        this.vm = vm;
        inputExpandFragment.setChatVM(vm);
        mEmojiFragment.setChatVM(vm);
        view.chatInput.setChatVM(vm);
        view.setChatVM(vm);
        vmListener();
    }

    @SuppressLint("SetTextI18n")
    private void vmListener() {
        vm.atMessages.observe((LifecycleOwner) context, messages -> {
            if (messages.isEmpty()) return;
            SpannableString spannableString =
                new SpannableString("@" + messages.get(messages.size() - 1).getSenderNickname() + "\t");
            ForegroundColorSpan colorSpan = new ForegroundColorSpan(Color.parseColor("#009ad6"));
            spannableString.setSpan(colorSpan, 0, spannableString.length(),
                Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
            Message lastMsg = messages.get(messages.size() - 1);
            MsgExpand msgExpand = (MsgExpand) lastMsg.getExt();
            if (null != msgExpand) msgExpand.spanHashCode = colorSpan.hashCode();
            view.chatInput.append(spannableString);
        });
        vm.emojiMessages.observe((LifecycleOwner) context, messages -> {
            if (messages.isEmpty()) return;
            String emojiKey = messages.get(messages.size() - 1);
            SpannableStringBuilder spannableString = new SpannableStringBuilder(emojiKey);
            int emojiId = Common.getMipmapId(EmojiUtil.emojiFaces.get(emojiKey));
            Drawable drawable = BaseApp.inst().getResources().getDrawable(emojiId);
            drawable.setBounds(0, 0, Common.dp2px(22), Common.dp2px(22));
            ImageSpan imageSpan = new ImageSpan(drawable);
            spannableString.setSpan(imageSpan, 0, emojiKey.length(),
                Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
            view.chatInput.append(spannableString);
        });
        view.chatInput.setOnKeyListener((v, keyCode, event) -> {
            //监听删除操作，找到最靠近删除的一个Span，然后整体删除
            if (keyCode == KeyEvent.KEYCODE_DEL && event.getAction() == KeyEvent.ACTION_DOWN) {
                TailInputEditText.spansDelete((TailInputEditText) v, vm);
            }
            return false;
        });

        if (!vm.isSingleChat) {
            vm.groupInfo.observe((LifecycleOwner) context, groupInfo -> {
                if (null == groupInfo) return;
                if (groupInfo.getStatus() == GrpStatus.GROUP_MUTED
                    && !groupInfo.getOwnerUserID().equals(BaseApp.inst().loginCertificate.userID)) {
                    view.inputLy.setVisibility(VISIBLE);
                    setSendButton(true);
                    view.touchSay.setVisibility(GONE);
                    view.root.setIntercept(true);
                    view.root.setAlpha(0.5f);
                    view.notice.setVisibility(VISIBLE);
                } else {
                    view.root.setIntercept(false);
                    view.root.setAlpha(1f);
                    view.notice.setVisibility(GONE);
                }
            });
        }
        vm.replyMessage.observe((LifecycleOwner) context, message -> {
            if (null == message) {
                view.replyLy.setVisibility(GONE);
            } else {
                view.voice.setChecked(false);
                view.replyLy.setVisibility(VISIBLE);
                view.replyContent.setText(message.getSenderNickname() + ":" + IMUtil.getMsgParse(message));
            }
        });
    }

    //设置扩展菜单隐藏
    public void setExpandHide() {
        view.fragmentContainer.setVisibility(GONE);
    }

    private int mCurrentTabIndex;
    private BaseFragment lastFragment;


    private void switchFragment(BaseFragment fragment) {
        try {
            if (fragment != null && !fragment.isVisible() && mCurrentTabIndex != fragment.getPage()) {
                FragmentTransaction transaction =
                    ((BaseActivity) context).getSupportFragmentManager().beginTransaction();
                if (!fragment.isAdded()) {
                    transaction.add(view.fragmentContainer.getId(), fragment);
                }
                if (lastFragment != null) {
                    transaction.hide(lastFragment);
                }
                transaction.show(fragment).commit();
                lastFragment = fragment;
                mCurrentTabIndex = lastFragment.getPage();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
