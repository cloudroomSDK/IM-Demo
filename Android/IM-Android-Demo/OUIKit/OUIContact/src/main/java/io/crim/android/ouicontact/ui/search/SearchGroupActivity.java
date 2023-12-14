package io.crim.android.ouicontact.ui.search;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.text.TextWatcher;

import java.util.List;

import androidx.recyclerview.widget.LinearLayoutManager;
import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.ouicontact.databinding.ActivityOftenSerchBinding;
import io.crim.android.ouicontact.ui.MyGroupActivity;
import io.crim.android.ouicontact.vm.SearchGroup;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;

public class SearchGroupActivity extends BaseActivity<SearchGroup, ActivityOftenSerchBinding> {

    private MyGroupActivity.ContentAdapter adapter;
    private Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(SearchGroup.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityOftenSerchBinding.inflate(getLayoutInflater()));
        sink();
        initView();
        listener();
    }

    private void listener() {
        view.cancel.setOnClickListener(v -> finish());
        vm.searchGroups.observe(this, groupInfos -> adapter.setItems(groupInfos));
        vm.searchKey.observe(this, s -> {
            vm.search(s);
        });
        view.searchView.getEditText().addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (s.toString().isEmpty()) vm.searchKey.setValue("");
            }

            @Override
            public void afterTextChanged(Editable s) {
                handler.removeCallbacksAndMessages(null);
                handler.postDelayed(() -> {
                    String input = s.toString();
                    vm.searchKey.setValue(input);
                }, 300);
            }
        });
    }

    private void initView() {
        view.recyclerview.setLayoutManager(new LinearLayoutManager(this));
        adapter = new MyGroupActivity.ContentAdapter(ViewHol.GroupViewHo.class);
        view.recyclerview.setAdapter(adapter);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        removeCacheVM();
    }

    public static void jumpThis(Context ctx, List<GrpInfo> groupInfos) {
        SearchGroup searchGroup = new SearchGroup();
        searchGroup.groups.setValue(groupInfos);
        BaseApp.inst().putVM(searchGroup);
        ctx.startActivity(new Intent(ctx, SearchGroupActivity.class));
    }
}
