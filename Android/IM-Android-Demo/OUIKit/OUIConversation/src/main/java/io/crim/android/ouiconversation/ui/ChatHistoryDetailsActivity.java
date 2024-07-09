package io.crim.android.ouiconversation.ui;

import android.content.Intent;
import android.os.Bundle;

import com.alibaba.android.arouter.launcher.ARouter;

import java.lang.reflect.Type;
import java.util.List;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import io.crim.android.sdk.enums.MsgType;
import io.crim.android.sdk.models.Message;
import io.crim.android.ouiconversation.databinding.ActivityChatHistoryDetailsBinding;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.GetFilePathFromUri;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.utils.TimeUtil;

public class ChatHistoryDetailsActivity extends BaseActivity<BaseViewModel, ActivityChatHistoryDetailsBinding> {


    private RecyclerViewAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityChatHistoryDetailsBinding.inflate(getLayoutInflater()));
        sink();
        initView();
        init();
    }

    private void initView() {
        view.recyclerview.setLayoutManager(new LinearLayoutManager(this));
        view.recyclerview.setAdapter(adapter = new RecyclerViewAdapter<Message,
            ViewHol.ContactItemHolder>(ViewHol.ContactItemHolder.class) {

            @Override
            public void onBindView(@NonNull ViewHol.ContactItemHolder holder, Message data, int position) {
                holder.viewBinding.avatar.load(data.getSenderFaceUrl());
                holder.viewBinding.nickName.setText(data.getSenderNickname());
                holder.viewBinding.time.setText(TimeUtil.getTimeString(data.getSendTime()));

                holder.viewBinding.lastMsg.setText(IMUtil.getMsgParse(data));

                holder.viewBinding.getRoot().setOnClickListener(v -> {
                    String url;
                    switch (data.getContentType()) {
                        case MsgType.MERGER:
                            startActivity(new Intent(ChatHistoryDetailsActivity.this,
                                ChatHistoryDetailsActivity.class).putExtra(Constant.K_RESULT,
                                GsonHel.toJson(data.getMergeElem().getMultiMessage())));
                            break;
                        case MsgType.PICTURE:
                            url = data.getPictureElem().getSourcePicture().getUrl();
                            startActivity(
                                new Intent(v.getContext(),
                                    PreviewActivity.class).putExtra(PreviewActivity.MEDIA_URL, url));
                            break;
                        case MsgType.VIDEO:
                            String snapshotUrl = data.getVideoElem().getSnapshotUrl();
                            url = data.getVideoElem().getVideoUrl();
                            v.getContext().startActivity(
                                new Intent(v.getContext(), PreviewActivity.class)
                                    .putExtra(PreviewActivity.MEDIA_URL, url)
                                    .putExtra(PreviewActivity.FIRST_FRAME, snapshotUrl));
                            break;
                        case MsgType.CARD:
                            ARouter.getInstance().build(Routes.Main.PERSON_DETAIL)
                                .withString(Constant.K_ID, data.getCardElem().getUserID())
                                .navigation();
                            break;
                        case MsgType.LOCATION:
                            Common.toMap(data, v);
                            break;
                        case MsgType.FILE:
                            GetFilePathFromUri.openFile(v.getContext(), data);
                            break;
                    }

                });
            }
        });
    }

    void init() {
        String extra = getIntent().getStringExtra(Constant.K_RESULT);
        try {
            Type listType = new GsonHel.ParameterizedTypeImpl(List.class, new Class[]{Message.class});
            List<Message> messages = GsonHel.getGson().fromJson(extra, listType);
            adapter.setItems(messages);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
