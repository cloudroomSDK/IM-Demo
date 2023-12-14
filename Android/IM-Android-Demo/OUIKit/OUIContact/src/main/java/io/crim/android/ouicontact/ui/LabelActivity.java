package io.crim.android.ouicontact.ui;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;

import com.yanzhenjie.recyclerview.SwipeMenuCreator;
import com.yanzhenjie.recyclerview.SwipeMenuItem;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import io.crim.android.ouicontact.databinding.ActivityLabelBinding;
import io.crim.android.ouicontact.vm.LabelVM;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.entity.UserLabel;
import io.crim.android.ouicore.entity.UserLabelChild;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.widget.SyLinearLayoutManager;

import static android.view.ViewGroup.LayoutParams.MATCH_PARENT;

public class LabelActivity extends BaseActivity<LabelVM, ActivityLabelBinding> {

    private RecyclerViewAdapter<UserLabel, ViewHol.LabelItem> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVM(LabelVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityLabelBinding.inflate(getLayoutInflater()));
        sink();

        init();
        listener();
    }

    private ActivityResultLauncher<Intent> launcher =
        registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
            if (result.getResultCode() == RESULT_OK) {
                vm.getUserTags();
            }
        });

    private void listener() {
        vm.userLabels.observe(this, userLabels -> {
            adapter.setItems(userLabels);
        });
        view.add.setOnClickListener(v -> {
            launcher.launch(new Intent(this, CreateLabelActivity.class));
        });
    }

    void init() {
        view.recyclerView.setLayoutManager(new SyLinearLayoutManager(this));
        SwipeMenuCreator mSwipeMenuCreator = (leftMenu, rightMenu, position) -> {
            SwipeMenuItem delete = new SwipeMenuItem(this);
            delete.setText(io.crim.android.ouicore.R.string.remove);
            delete.setHeight(MATCH_PARENT);
            delete.setWidth(Common.dp2px(73));
            delete.setTextSize(16);
            delete.setTextColor(this.getResources().getColor(android.R.color.white));
            delete.setBackgroundColor(Color.parseColor("#FFAB41"));

            rightMenu.addMenuItem(delete);
        };
        view.recyclerView.setSwipeMenuCreator(mSwipeMenuCreator);
        view.recyclerView.setOnItemMenuClickListener((menuBridge, adapterPosition) -> {
            menuBridge.closeMenu();
            UserLabel userLabel = adapter.getItems().get(adapterPosition);
            vm.removeTag(userLabel, true);
        });
        view.recyclerView.setAdapter(adapter = new RecyclerViewAdapter<UserLabel,
            ViewHol.LabelItem>(ViewHol.LabelItem.class) {

            @Override
            public void onBindView(@NonNull ViewHol.LabelItem holder, UserLabel data,
                                   int position) {
                holder.view.name.setText(data.getTagName());
                StringBuilder member = new StringBuilder();

                if (null!=data.getUserList()){
                    for (UserLabelChild userLabelChild : data.getUserList()) {
                        member.append(userLabelChild.getUserName());
                        member.append("、");
                    }
                    holder.view.content.setText(member.substring(0, member.length() - 1));
                }

            }
        });
        vm.getUserTags();
    }
}

