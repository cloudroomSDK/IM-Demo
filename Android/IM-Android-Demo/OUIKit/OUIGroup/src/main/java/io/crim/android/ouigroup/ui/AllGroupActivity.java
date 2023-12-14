package io.crim.android.ouigroup.ui;

import android.os.Bundle;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.databinding.LayoutPopSelectedFriendsBinding;
import io.crim.android.ouicore.databinding.LayoutSelectedFriendsBinding;
import io.crim.android.ouicore.databinding.OftenRecyclerViewBinding;
import io.crim.android.ouicore.ex.MultipleChoice;
import io.crim.android.ouicore.vm.MultipleChoiceVM;
import io.crim.android.ouicore.vm.SocialityVM;

public class AllGroupActivity extends BaseActivity<SocialityVM, OftenRecyclerViewBinding> {

    private LayoutSelectedFriendsBinding selectedFriendsBinding;
    private MultipleChoiceVM choiceVM;
    private  RecyclerViewAdapter<GrpInfo, ViewHol.ItemViewHo> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVM(SocialityVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(OftenRecyclerViewBinding.inflate(getLayoutInflater()));

        init();
        initView();
        listener();
    }

    private void listener() {
        vm.groups.observe(this,v->{
           if ( v.isEmpty())return;
           adapter.setItems(v);
        });
    }

    void init() {
        vm.getAllGroup();
        try {
            choiceVM = Easy.find(MultipleChoiceVM.class);
            choiceVM.metaData.observe(this,v->adapter.notifyDataSetChanged());
        }catch (Exception ignored){}
    }

    private void initView() {
        selectedFriendsBinding = LayoutSelectedFriendsBinding.inflate(getLayoutInflater());
        view.parent.addView(selectedFriendsBinding.getRoot());
        if (null!=choiceVM){
            choiceVM.bindDataToView(selectedFriendsBinding);
            choiceVM.showPopAllSelectFriends(selectedFriendsBinding,
                LayoutPopSelectedFriendsBinding.inflate(getLayoutInflater()));
            choiceVM.submitTap(selectedFriendsBinding.submit);
        }
        view.top.title.setText(io.crim.android.ouicore.R.string.group);

        view.recyclerView.setLayoutManager(new LinearLayoutManager(this));
        view.recyclerView.setAdapter(adapter=new RecyclerViewAdapter<GrpInfo,ViewHol.ItemViewHo>
            (ViewHol.ItemViewHo.class) {

            @Override
            public void onBindView(@NonNull ViewHol.ItemViewHo holder, GrpInfo data,
                                   int position) {

                holder.view.avatar.load(data.getFaceURL(), true);
                holder.view.nickName.setText(data.getGroupName());

                holder.view.select.setVisibility(View.VISIBLE);
                holder.view.select.setChecked(choiceVM.contains(new MultipleChoice(data.getGroupID())));
                holder.view.getRoot().setOnClickListener(v -> {
                    holder.view.select.setChecked(!holder.view.select.isChecked());

                    if (holder.view.select.isChecked()){
                        MultipleChoice meta=new MultipleChoice(data.getGroupID());
                        meta.isGroup=true;
                        meta.name=data.getGroupName();
                        meta.icon=data.getFaceURL();
                        choiceVM.metaData.val().add(meta);
                        choiceVM.metaData.update();
                    }else {
                        choiceVM.removeMetaData(data.getGroupID());
                    }
                });
            }
        });
    }
}
