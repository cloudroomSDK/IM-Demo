package io.crim.android.ouicontact.vm;

import android.text.TextUtils;

import java.util.ArrayList;
import java.util.List;

import androidx.lifecycle.MutableLiveData;
import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.ouicore.base.BaseViewModel;

public class SearchGroup extends BaseViewModel {
    //我加入的群
    public MutableLiveData<List<GrpInfo>> groups = new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<GrpInfo>> searchGroups = new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<String> searchKey = new MutableLiveData<>();

    public void search(String key) {
        searchGroups.getValue().clear();
        if (!TextUtils.isEmpty(key)) {
            for (GrpInfo groupInfo : groups.getValue()) {
                if (groupInfo.getGroupName().toUpperCase().contains(key.toUpperCase())) {
                    searchGroups.getValue().add(groupInfo);
                }
            }
        }
        searchGroups.setValue(searchGroups.getValue());
    }
}
