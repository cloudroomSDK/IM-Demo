package io.crim.android.ouiconversation.ui.fragment;


import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.alibaba.android.arouter.launcher.ARouter;
import com.bumptech.glide.Glide;
import com.bumptech.glide.request.target.SimpleTarget;
import com.bumptech.glide.request.transition.Transition;
import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;
import com.zhihu.matisse.Matisse;
import com.zhihu.matisse.MimeType;
import com.zhihu.matisse.engine.impl.GlideEngine;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import io.crim.android.ouiconversation.R;
import io.crim.android.ouiconversation.databinding.FragmentInputExpandBinding;
import io.crim.android.ouiconversation.databinding.ItemExpandMenuBinding;
import io.crim.android.ouiconversation.ui.ShootActivity;
import io.crim.android.ouiconversation.vm.ChatVM;
import io.crim.android.ouiconversation.widget.LocationConfirmDialog;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.services.CallingService;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.GetFilePathFromUri;
import io.crim.android.ouicore.utils.MThreadTool;
import io.crim.android.ouicore.utils.MediaFileUtil;
import io.crim.android.ouicore.utils.PermissionUtil;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.GroupVM;
import io.crim.android.ouicore.widget.AMapWebViewActivity;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.models.SignalingInfo;


public class InputExpandFragment extends BaseFragment<ChatVM> {
    public static List<Integer> menuIcons = Arrays.asList(
        io.crim.android.ouicore.R.mipmap.ic_chat_photo
        , io.crim.android.ouicore.R.mipmap.ic_chat_shoot
        , io.crim.android.ouicore.R.mipmap.ic_chat_file
        , io.crim.android.ouicore.R.mipmap.ic_chat_location
        , io.crim.android.ouicore.R.mipmap.ic_chat_card
//        ,R.mipmap.ic_tools_video_call
    );
    public static List<String> menuTitles = Arrays.asList(BaseApp.inst().getString(
            io.crim.android.ouicore.R.string.album)
        , BaseApp.inst().getString(io.crim.android.ouicore.R.string.shoot)
        , BaseApp.inst().getString(io.crim.android.ouicore.R.string.file)
        , BaseApp.inst().getString(io.crim.android.ouicore.R.string.location)
        , BaseApp.inst().getString(io.crim.android.ouicore.R.string.business_card)
//        , BaseApp.inst().getString(io.crim.android.ouicore.R.string.video_calls)
    );

    FragmentInputExpandBinding v;
    //权限
    boolean hasStorage, hasShoot, hasLocation;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        MThreadTool.executorService.execute(() -> {
            hasStorage = AndPermission.hasPermissions(getActivity(), PermissionUtil.getReadImgPermission());
            hasShoot = AndPermission.hasPermissions(getActivity(), Permission.CAMERA, Permission.RECORD_AUDIO);
            hasLocation = AndPermission.hasPermissions(getActivity(), Permission.ACCESS_FINE_LOCATION, Permission.ACCESS_COARSE_LOCATION);
        });

    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        v = FragmentInputExpandBinding.inflate(inflater);
        init();
        return v.getRoot();
    }

    private void init() {
        v.getRoot().setLayoutManager(new GridLayoutManager(getContext(), 4));
        RecyclerViewAdapter adapter = new RecyclerViewAdapter<Object, ExpandHolder>(ExpandHolder.class) {

            @Override
            public void onBindView(@NonNull ExpandHolder holder, Object data, int position) {
                holder.v.ivImg.setImageResource(menuIcons.get(position));
                holder.v.tvDec.setText(menuTitles.get(position));
//                holder.v.menu.setCompoundDrawablesRelativeWithIntrinsicBounds(null, getContext().getDrawable(menuIcons.get(position)), null, null);
//                holder.v.menu.setText(menuTitles.get(position));
                holder.v.llTab.setOnClickListener(v -> {
                    switch (position) {
                        case 0:
                            showMediaPicker();
                            break;
                        case 1:
                            goToShoot();
                            break;
                        case 2:
                            selectFile();
                            break;
                        case 3:
//                            goToCall();
                            sendLocation();
                            break;
                        case 4:
                            recommend();
                            break;
                    }
                });
            }
        };
        v.getRoot().setAdapter(adapter);
        adapter.setItems(menuIcons);
    }

    @SuppressLint("WrongConstant")
    private void goToCall() {
        Common.permission(getContext(), () -> {
            hasStorage = true;
            CallingService callingService = (CallingService) ARouter.getInstance().build(Routes.Service.CALLING).navigation();
            if (null == callingService) return;
            IMUtil.showBottomPopMenu(getContext(), (v1, keyCode, event) -> {
                vm.isVideoCall = keyCode != 1;
                if (vm.isSingleChat) {
                    List<String> ids = new ArrayList<>();
                    ids.add(vm.userID);
                    SignalingInfo signalingInfo = IMUtil.buildSignalingInfo(vm.isVideoCall, vm.isSingleChat, ids, null);
                    callingService.call(signalingInfo);
                } else {
                    toSelectMember();
                }
                return false;
            });
        }, hasStorage, PermissionUtil.getReadImgPermission());
    }

    private void sendLocation() {
        Common.permission(getContext(), () -> {
            new LocationConfirmDialog(getContext()).setClickListener(new LocationConfirmDialog.OnLocationConfirmListener() {
                @Override
                public void confirm(boolean confirm) {
                    if (confirm) {
//                        ARouter.getInstance().build(Routes.Core.AMAP)
//                            .navigation(getActivity(), Constant.Event.MAP_POSITION);
                        getActivity().startActivityForResult(new Intent(getActivity(), AMapWebViewActivity.class), Constant.Event.MAP_POSITION);
                    }
                }
            }).show();
        }, hasLocation, Permission.ACCESS_FINE_LOCATION, Permission.ACCESS_COARSE_LOCATION);
    }

    private void recommend() {
        ARouter.getInstance().build(Routes.Contact.ALL_FRIEND)
            .withBoolean("formChat", true)
            .withString("userID", vm.userID)
            .withString("groupID", vm.groupID)
            .navigation(getActivity(), Constant.Event.RECOMMEND_FROM_CHAT);
    }

    public void toSelectMember() {
        GroupVM groupVM = new GroupVM();
        groupVM.groupId = vm.groupID;
        BaseApp.inst().putVM(groupVM);
        ARouter.getInstance().build(Routes.Group.SUPER_GROUP_MEMBER)
            .withBoolean(Constant.IS_SELECT_MEMBER, true)
            .withInt(Constant.K_SIZE, 9).navigation(getActivity(), Constant.Event.CALLING_REQUEST_CODE);
    }


    //去拍摄
    private void goToShoot() {
        if (hasShoot)
            shootLauncher.launch(new Intent(getActivity(), ShootActivity.class));
        else {
            AndPermission.with(this).runtime().permission(Permission.CAMERA, Permission.RECORD_AUDIO).onGranted(permissions -> {
                // Storage permission are allowed.
                hasShoot = true;
                shootLauncher.launch(new Intent(getActivity(), ShootActivity.class));
            }).onDenied(permissions -> {
                // Storage permission are not allowed.
            }).start();
        }
    }

    private void selectFile() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        fileSelectLauncher.launch(intent);
    }

    private final ActivityResultLauncher<Intent> fileSelectLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
        sendFile(true, result);
    });

    private final ActivityResultLauncher<Intent> captureLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
        sendFile(false, result);
    });

    private void sendFile(boolean fromFileSelect, ActivityResult result) {
        if (result.getResultCode() == Activity.RESULT_OK) {
            Intent data = result.getData();
            List<String> files = Matisse.obtainPathResult(data);
            if (files == null) {
                files = new ArrayList<>();
                files.add(GetFilePathFromUri.getFileAbsolutePath(getContext(), data.getData()));
            }
            for (String file : files) {
                Message msg = null;
                if (MediaFileUtil.isImageType(file)) {
                    msg = CRIMClient.getInstance().messageManager.createImageMsgFromFullPath(file);
                } else if (MediaFileUtil.isVideoType(file)) {
                    boolean canSend = true;
                    long duration = MediaFileUtil.getDuration(file) / 1000;
                    if (duration > 120) {
                        toast("不能选择超过120秒的视频");
                        canSend = false;
                    }
                    if (canSend) {
                        Glide.with(this).asBitmap().load(file).into(new SimpleTarget<Bitmap>() {
                            @Override
                            public void onResourceReady(@NonNull Bitmap resource, @Nullable Transition<? super Bitmap> transition) {
                                String firstFame = MediaFileUtil.saveBitmap(resource, Constant.PICTURE_DIR, false);
                                Message msg = CRIMClient.getInstance().messageManager.createVideoMsgFromFullPath(file, MediaFileUtil.getFileType(file).mimeType, duration, firstFame);
                                vm.sendMsg(msg);
                            }
                        });
                    }
                    continue;
                } else if (fromFileSelect) {
                    String fileName = file;
                    String[] split = file.split(File.separator);
                    fileName = split[split.length - 1];
                    msg = CRIMClient.getInstance().messageManager.createFileMsgFromFullPath(file, fileName);
                }
                if (null == msg) {
                    msg = CRIMClient.getInstance().messageManager.createTextMsg("[" + getString(R.string.unsupported_type) + "]");
                }
                vm.sendMsg(msg);
            }
        }
    }

    private final ActivityResultLauncher<Intent> shootLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
        if (result.getResultCode() == Activity.RESULT_OK) {
            String fileUrl = result.getData().getStringExtra("fileUrl");
            if (MediaFileUtil.isImageType(fileUrl)) {
                Message msg = CRIMClient.getInstance().messageManager.createImageMsgFromFullPath(fileUrl);
                vm.sendMsg(msg);
            }
            if (MediaFileUtil.isVideoType(fileUrl)) {
                String firstFrameUrl = result.getData().getStringExtra("firstFrameUrl");
                MediaFileUtil.MediaFileType mediaFileType = MediaFileUtil.getFileType(fileUrl);
                long duration = MediaFileUtil.getDuration(fileUrl) / 1000;
                Message msg = CRIMClient.getInstance().messageManager
                    .createVideoMsgFromFullPath(fileUrl, mediaFileType.mimeType, duration, firstFrameUrl);
                vm.sendMsg(msg);
            }
        }
    });

    @SuppressLint("WrongConstant")
    private void showMediaPicker() {
        if (hasStorage) goMediaPicker();
        else
            AndPermission.with(this).runtime().permission(PermissionUtil.getReadImgPermission()).onGranted(permissions -> {
                // Storage permission are allowed.
                hasStorage = true;
                goMediaPicker();
            }).onDenied(permissions -> {
                // Storage permission are not allowed.
            }).start();
    }

    private void goMediaPicker() {
        Matisse.from(getActivity()).choose(MimeType.ofAll()).countable(true).maxSelectable(9)
            .restrictOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED).thumbnailScale(0.85f)
            .imageEngine(new GlideEngine()).forResult(captureLauncher);
    }


    public void setChatVM(ChatVM vm) {
        this.vm = vm;
    }

    public static class ExpandHolder extends RecyclerView.ViewHolder {
        public ItemExpandMenuBinding v;

        public ExpandHolder(@NonNull View itemView) {
            super(ItemExpandMenuBinding.inflate(LayoutInflater.from(itemView.getContext())).getRoot());
            v = ItemExpandMenuBinding.bind(this.itemView);
        }
    }
}
