package io.crim.android.ouicontact.ui;


import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.alibaba.android.arouter.launcher.ARouter;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import io.crim.android.sdk.models.FriendReqInfo;
import io.crim.android.ouicontact.databinding.ActivityNewFriendBinding;
import io.crim.android.ouicontact.databinding.ItemFriendNoticeBinding;
import io.crim.android.ouicontact.vm.ContactVM;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.utils.SinkHelper;

public class NewFriendActivity extends BaseActivity<ContactVM, ActivityNewFriendBinding> {
    RecyclerViewAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVM(ContactVM.class, true);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityNewFriendBinding.inflate(getLayoutInflater()));
        setLightStatus();
        SinkHelper.get(this).setTranslucentStatus(view.getRoot());

        vm.getRecvFriendApplicationList();
        initView();
        listener();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        removeCacheVM();
    }

    private void initView() {
        view.recyclerView.setLayoutManager(new LinearLayoutManager(this));


        adapter = new RecyclerViewAdapter<FriendReqInfo, ViewHo>(ViewHo.class) {

            @Override
            public void onBindView(@NonNull ViewHo holder, FriendReqInfo data, int position) {
                holder.view.avatar.load(data.getFromFaceURL());
                holder.view.nickName.setText(data.getFromNickname());
                holder.view.hil.setText(data.getReqMsg());
                holder.view.handle.setBackgroundColor(getResources().getColor(android.R.color.white));

                if (data.getHandleResult() == 0) {
                    holder.view.handle.setBackgroundResource(io.crim.android.ouicore.R.drawable.sty_radius_3_stroke_418ae5);
                    holder.view.handle.setText(getString(io.crim.android.ouicore.R.string.accept));

                    holder.view.getRoot().setOnClickListener(v -> {
                        vm.friendDetail.setValue(data);
                        startActivity(new Intent(NewFriendActivity.this, FriendRequestDetailActivity.class));
                    });
                } else if (data.getHandleResult() == -1) {
                    holder.view.getRoot().setOnClickListener(null);
                    holder.view.handle.setText(getString(io.crim.android.ouicore.R.string.rejected));
                    holder.view.handle.setTextColor(Color.parseColor("#999999"));
                } else {
                    holder.view.handle.setText(getString(io.crim.android.ouicore.R.string.hil));
                    holder.view.getRoot().setOnClickListener(v -> ARouter.getInstance().build(Routes.Conversation.CHAT)
                        .withString(Constant.K_ID, data.getFromUserID())
                        .withString(Constant.K_NAME, data.getFromNickname())
                        .navigation());
                }

            }
        };
        view.recyclerView.setAdapter(adapter);
    }

    private void listener() {
        vm.friendApply.observe(this, v -> {
            adapter.setItems(v);
        });
    }


    public void toBack(View v) {
        finish();
    }


    public static class ViewHo extends RecyclerView.ViewHolder {
        public ItemFriendNoticeBinding view;

        public ViewHo(@NonNull View parent) {
            super((ItemFriendNoticeBinding.inflate(LayoutInflater.from(parent.getContext()), (ViewGroup) parent, false).getRoot()));
            view = ItemFriendNoticeBinding.bind(this.itemView);
        }
    }
}
