package io.crim.android.ouiconversation.ui.emoji;

import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;

import com.bumptech.glide.Glide;
import com.bumptech.glide.request.target.CustomTarget;
import com.bumptech.glide.request.transition.Transition;
import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.GridLayoutManager;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnFileUploadProgressListener;
import io.crim.android.sdk.models.PutArgs;
import io.crim.android.ouiconversation.databinding.ActivityCustomEmojiManageBinding;
import io.crim.android.ouiconversation.vm.CustomEmojiVM;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.entity.CustomEmoji;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.L;
import io.crim.android.ouicore.widget.GridSpaceItemDecoration;
import io.crim.android.ouicore.widget.PhotographAlbumDialog;
import io.crim.android.ouicore.widget.WaitDialog;

public class CustomEmojiManageActivity extends BaseActivity<BaseViewModel,
    ActivityCustomEmojiManageBinding> {

    private CustomEmojiVM customEmojiVM;
    //正在编辑？
    private boolean isEdit = false;
    private RecyclerViewAdapter<Object, ViewHol.SelectImageViewHolder> customEmojiAdapter;
    private List<Object> customEmojis = new ArrayList<>();
    private boolean hasStorage;
    private PhotographAlbumDialog albumDialog;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        runOnUiThread(() -> hasStorage = AndPermission.hasPermissions(this,
            Permission.Group.STORAGE));
        customEmojiVM = Easy.find(CustomEmojiVM.class);
        bindViewDataBinding(ActivityCustomEmojiManageBinding.inflate(getLayoutInflater()));

        init();

        listener();
    }

    private void listener() {
        customEmojiVM
            .customEmojis
            .observe(this, v -> {
                customEmojis.clear();
                customEmojis.add(0, CustomEmojiVM.addID);
                customEmojis.addAll(v);
                customEmojiAdapter.setItems(customEmojis);

                view.count.setText(String.format(getString(io.crim.android.ouicore.R.string.emoji_count),
                    v.size() + ""));
            });
        customEmojiVM.deletedEmojiUrl.observe(this, v -> {
            view.deleteCount.setText(getString(io.crim.android.ouicore.R.string.delete) + "(" + v.size() + ")");
        });

        view.manage.setOnClickListener(v -> {
            isEdit = v.getTag() != null && (boolean) v.getTag();
            isEdit = !isEdit;
            v.setTag(isEdit);
            customEmojiAdapter.notifyDataSetChanged();

            view.deleteCount.setVisibility(isEdit ? View.VISIBLE : View.GONE);
        });

        view.deleteCount.setOnClickListener(v -> customEmojiVM.toDelete());
    }

    void init() {
        albumDialog = new PhotographAlbumDialog(this);
        view.recyclerView.setLayoutManager(new GridLayoutManager(this, 4));
        GridSpaceItemDecoration divItemDecoration = new GridSpaceItemDecoration(Common.dp2px(25));
        view.recyclerView.addItemDecoration(divItemDecoration);
        view.recyclerView.setAdapter(customEmojiAdapter = new RecyclerViewAdapter<Object,
            ViewHol.SelectImageViewHolder>(ViewHol.SelectImageViewHolder.class) {

            @Override
            public void onBindView(@NonNull ViewHol.SelectImageViewHolder holder, Object obj,
                                   int position) {
                holder.view.getRoot().getLayoutParams().height = Common.dp2px(65);
                if (obj instanceof CustomEmoji) {
                    CustomEmoji data = (CustomEmoji) obj;
                    String url = data.getThumbnailUrl();
                    if (TextUtils.isEmpty(url))
                        url = data.getSourceUrl();
                    Glide.with(CustomEmojiManageActivity.this).load(url).centerCrop().into(holder.view.img);
                    holder.view.getRoot().setOnClickListener(null);

                    holder.view.select.setVisibility(isEdit ? View.VISIBLE : View.GONE);
                    holder.view.select.setChecked(customEmojiVM.isDeletedList(data.getSourceUrl()));
                    holder.view.select.setOnCheckedChangeListener((buttonView, isChecked) -> {
                        if (isChecked)
                            customEmojiVM.addDeleted(data.getSourceUrl());
                        else
                            customEmojiVM.removeDeleted(data.getSourceUrl());
                    });
                    holder.view.getRoot().setOnClickListener(null);
                } else {
                    Glide.with(CustomEmojiManageActivity.this).load(obj).centerCrop().into(holder.view.img);
                    holder.view.select.setVisibility(View.GONE);
                    holder.view.getRoot().setOnClickListener(v1 -> {
                        Common.permission(CustomEmojiManageActivity.this, () -> {
                            hasStorage = true;
                            goMediaPicker();
                        }, hasStorage, Permission.Group.STORAGE);
                    });
                }
            }
        });
    }

    private void goMediaPicker() {
        WaitDialog waitDialog = new WaitDialog(this);
        waitDialog.setNotDismiss();
        albumDialog.setMaxSelectable(1);
        albumDialog.show();
        albumDialog.setOnSelectResultListener(path -> {
            waitDialog.show();
            PutArgs putArgs=new PutArgs(path[0]);

            CRIMClient.getInstance().uploadFile(new OnFileUploadProgressListener() {
                @Override
                public void onError(int code, String error) {
                    waitDialog.dismiss();
                    toast(error + code);
                    L.e(error + code);
                }

                @Override
                public void onProgress(long progress) {}

                @Override
                public void onSuccess(String s) {
                    CustomEmoji customEmoji = new CustomEmoji();
                    customEmoji.setUserID(BaseApp.inst().loginCertificate.userID);
                    Map<String,String> hashMap=GsonHel.fromJson(s,HashMap.class);
                    customEmoji.setSourceUrl(hashMap.get("url"));

                    Glide.with(CustomEmojiManageActivity.this)
                        .asBitmap().load(putArgs.filepath).into(new CustomTarget<Bitmap>() {
                            @Override
                            public void onLoadFailed(@Nullable Drawable errorDrawable) {
                                super.onLoadFailed(errorDrawable);
                                waitDialog.dismiss();
                            }
                            @Override
                            public void onLoadCleared(@Nullable Drawable placeholder) {
                                waitDialog.dismiss();
                            }
                            @Override
                            public void onResourceReady(@NonNull Bitmap resource,
                                                        @Nullable Transition<? super Bitmap> transition) {
                                waitDialog.dismiss();
                                customEmoji.setSourceW(resource.getWidth());
                                customEmoji.setSourceH(resource.getHeight());
                                customEmojiVM.insertEmojiDb(new ArrayList<>(Collections.singleton(customEmoji)));
                            }
                        });

                }
            },null, putArgs);

        });
    }

}
