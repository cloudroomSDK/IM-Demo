package io.crim.android.ouicontact.ui.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.ouicontact.ui.ForwardToActivity;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.databinding.ViewRecyclerViewBinding;
import io.crim.android.ouicore.vm.SocialityVM;
import io.crim.android.ouicore.widget.CommonDialog;

public class GroupFragment extends BaseFragment<SocialityVM> {
    private ForwardToActivity.ConfirmListener confirmListener;
    ViewRecyclerViewBinding view;
    private RecyclerViewAdapter<GrpInfo, ViewHol.GroupViewHo> adapter;

    public void setConfirmListener(ForwardToActivity.ConfirmListener confirmListener) {
        this.confirmListener = confirmListener;
    }

    public static GroupFragment newInstance() {

        Bundle args = new Bundle();

        GroupFragment fragment = new GroupFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        bindVM(SocialityVM.class);
        vm.getAllGroup();
        super.onCreate(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = ViewRecyclerViewBinding.inflate(inflater);
        initView();
        listener();
        return view.getRoot();
    }

    private void initView() {
        view.recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        adapter = new RecyclerViewAdapter<GrpInfo, ViewHol.GroupViewHo>(ViewHol.GroupViewHo.class) {

            @Override
            public void onBindView(@NonNull ViewHol.GroupViewHo holder, GrpInfo data, int position) {
                holder.view.avatar.load(data.getFaceURL());
                holder.view.title.setText(data.getGroupName());
                holder.view.description.setText(data.getMemberCount() + "人");

                holder.view.getRoot().setOnClickListener(v -> {
                    CommonDialog commonDialog = new CommonDialog(getContext());
                    commonDialog.getMainView().tips.setText("确认发送给：" + data.getGroupName());
                    commonDialog.getMainView().cancel.setOnClickListener(v1 -> commonDialog.dismiss());
                    commonDialog.getMainView().confirm.setOnClickListener(v1 -> {
                        if (null != confirmListener)
                            confirmListener.onListener(null, data.getGroupID());
                    });
                    commonDialog.show();
                });

            }
        };
        view.recyclerView.setAdapter(adapter);
    }

    private void listener() {
        vm.groups.observe(getActivity(), groupInfos -> {
            adapter.setItems(groupInfos);
        });
    }

}
