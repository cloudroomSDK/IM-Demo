package io.crim.android.ouicore.vm;

import android.os.Handler;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.widget.EditText;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import androidx.lifecycle.MutableLiveData;
import io.crim.android.ouicore.api.OneselfService;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.entity.UserList;
import io.crim.android.ouicore.net.RXRetrofit.N;
import io.crim.android.ouicore.net.RXRetrofit.NetObserver;
import io.crim.android.ouicore.net.RXRetrofit.Parameter;
import io.crim.android.ouicore.utils.L;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.MsgType;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.models.FriendInfo;
import io.crim.android.sdk.models.FriendshipInfo;
import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.sdk.models.SearchResult;
import io.crim.android.sdk.models.SearchResultItem;
import io.crim.android.sdk.models.UserInfo;

public class SearchVM extends BaseViewModel {
    public MutableLiveData<List<SearchResultItem>> messageItems =
        new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<SearchResultItem>> fileItems =
        new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<GrpInfo>> groupsInfo = new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<UserInfo>> userInfo = new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<FriendInfo>> friendInfo = new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<FriendshipInfo>> friendshipInfo =
        new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<GroupMembersInfo>> groupMembersInfo =
        new MutableLiveData<>(new ArrayList<>());

    public MutableLiveData<String> hail = new MutableLiveData<>();
    public MutableLiveData<String> remark = new MutableLiveData<>();
    //用户 或群组id
    public MutableLiveData<String> searchContent = new MutableLiveData<>("");

    //true 搜索人 false 搜索群
    public boolean isPerson = false;
    public int page;
    public int pageSize;
    private final Handler handler = new Handler();

    public void searchPerson() {
        searchPerson(null);
    }

    public void searchPerson(List<String> ids) {
        if (null == ids) {
            ids = new ArrayList<>(); // 用户ID集合
            ids.add(searchContent.getValue());
        }
        //兼容旧版
        CRIMClient.getInstance().userInfoManager.getUsersInfo(new OnBase<List<UserInfo>>() {
            @Override
            public void onError(int code, String error) {
                L.e(error + "-" + code);
                userInfo.setValue(null);
            }

            @Override
            public void onSuccess(List<UserInfo> data) {
                if (data.isEmpty()) return;
                userInfo.setValue(data);
            }
        }, ids);
    }


    public void checkFriend(List<UserInfo> data) {
        List<String> uIds = new ArrayList<>();
        uIds.add(data.get(0).getUserID());
        CRIMClient.getInstance().friendshipManager.checkFriend(new OnBase<List<FriendshipInfo>>() {
            @Override
            public void onError(int code, String error) {

            }

            @Override
            public void onSuccess(List<FriendshipInfo> data) {
                friendshipInfo.setValue(data);
            }
        }, uIds);

    }

    //uid、昵称、备注、手机号
    public void searchUser(String keyword) {
        Parameter parameter = new Parameter();
        parameter.add("keyword", keyword);
//        "pagination": {
//            "pageNumber": 0,
//                "showNumber": 10
//        },
        Map<String, Integer> pa = new HashMap<>();
        pa.put("pageNumber", 1);
        pa.put("showNumber", 100);
        parameter.add("pagination", pa);
        parameter.add("keyword", keyword);
        parameter.add("keyword", keyword);

        N.API(OneselfService.class).searchUser(parameter.buildJsonBody())
            .map(OneselfService.turn(UserList.class))
            .compose(N.IOMain())
            .subscribe(new NetObserver<UserList>("") {


                @Override
                public void onSuccess(UserList o) {
                 try {
                     if (o.users.size() > 0)
                         userInfo.setValue(o.users);
                 }catch (Exception e){e.printStackTrace();}
                }

                @Override
                protected void onFailure(Throwable e) {
                    getIView().onError("搜索失败");
                }
            });
    }

    public void searchGroupMemberByNickname(String groupId, String key) {
        List<String> keys = new ArrayList<>(); // 用户ID集合
        keys.add(key);
        CRIMClient.getInstance().groupManager.searchGrpMembers(new OnBase<List<GroupMembersInfo>>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(List<GroupMembersInfo> data) {
                if (key.isEmpty()) return;
                groupMembersInfo.getValue().addAll(data);
                groupMembersInfo.setValue(groupMembersInfo.getValue());
            }
        }, groupId, keys, false, true, page, pageSize);
    }

    public void addFriend() {
        OnBase<String> callBack = new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                Toast.makeText(getContext(), error+code, Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onSuccess(String data) {
                Toast.makeText(getContext(), "发送成功", Toast.LENGTH_SHORT).show();
                getIView().onSuccess("");
//                String remarkStr = remark.getValue();
//                if (!TextUtils.isEmpty(remarkStr)) {
//                    CRIMClient.getInstance().friendshipManager.setFriendRemark(null,
//                        searchContent.getValue(), remarkStr);
//                }
            }
        };
        if (isPerson)
            CRIMClient.getInstance().friendshipManager.addFriend(callBack,
                searchContent.getValue(), hail.getValue());
        else
            CRIMClient.getInstance().groupManager.joinGrp(callBack, searchContent.getValue(),
                hail.getValue(), 2);
    }

    public void search() {
        if (isPerson) searchPerson();
        else searchGroup(searchContent.getValue());
    }

    public void searchGroup(String gid) {
        List<String> groupIds = new ArrayList<>(); // 群ID集合
        groupIds.add(gid);
        CRIMClient.getInstance().groupManager.getSpecifiedGrpsInfo(new OnBase<List<GrpInfo>>() {
            @Override
            public void onError(int code, String error) {

            }

            @Override
            public void onSuccess(List<GrpInfo> data) {
                groupsInfo.setValue(data);
            }
        }, groupIds);
    }

    public void searchFriendV2() {
        CRIMClient.getInstance().friendshipManager.searchFriends(new OnBase<List<FriendInfo>>() {
            @Override
            public void onError(int code, String error) {

            }

            @Override
            public void onSuccess(List<FriendInfo> data) {
                if (page == 1) {
                    friendInfo.getValue().clear();
                }
                if (!data.isEmpty()) {
                    friendInfo.getValue().addAll(data);
                }
                friendInfo.setValue(friendInfo.getValue());
            }
        }, buildKeyWord(), true, true, true);
    }

    public void searchGroupV2() {
        CRIMClient.getInstance().groupManager.searchGrps(new OnBase<List<GrpInfo>>() {
            @Override
            public void onError(int code, String error) {
            }

            @Override
            public void onSuccess(List<GrpInfo> data) {
                if (page == 1) {
                    groupsInfo.getValue().clear();
                }
                if (!data.isEmpty()) {
                    groupsInfo.getValue().addAll(data);
                }
                groupsInfo.setValue(groupsInfo.getValue());
            }
        }, buildKeyWord(), false, true);
    }

    public void searchLocalMessages(String key, Integer... messageTypes) {
        List<String> keys = null;
        if (!TextUtils.isEmpty(key)) {
            keys = new ArrayList<>();
            keys.add(key);
        }
        List<Integer> messageTypeLists;
        if (0 == messageTypes.length) {
            messageTypeLists = new ArrayList<>();
            messageTypeLists.add(MsgType.TEXT);
            messageTypeLists.add(MsgType.AT_TEXT);
        } else {
            messageTypeLists = Arrays.asList(messageTypes);
        }
        MutableLiveData<List<SearchResultItem>> items;
        List<Integer> type;
        if ((type = Arrays.asList(messageTypes)).size() == 1
            && type.get(0) == MsgType.FILE) {
            items = fileItems;
        } else {
            items = messageItems;
        }
        CRIMClient.getInstance().messageManager.searchLocalMsgs(new OnBase<SearchResult>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(SearchResult data) {
                if (page == 1) {
                    items.getValue().clear();
                }
                if (data.getTotalCount() != 0) {
                    items.getValue().addAll(data.getSearchResultItems());
                }
                items.setValue(items.getValue());
            }
        }, null, keys, 0, null, messageTypeLists, 0, 0, page, pageSize);
    }

    private List<String> buildKeyWord() {
        List<String> keyWords = new ArrayList<>();
        keyWords.add(searchContent.getValue());
        return keyWords;
    }


    public void clearData() {
        messageItems.getValue().clear();
        fileItems.getValue().clear();
        groupsInfo.getValue().clear();
        userInfo.getValue().clear();
        friendInfo.getValue().clear();
    }

    public void addTextChangedListener(EditText editText) {
        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (s.toString().isEmpty()) searchContent.setValue("");
            }

            @Override
            public void afterTextChanged(Editable s) {
                handler.removeCallbacksAndMessages(null);
                handler.postDelayed(() -> {
                    String input = s.toString();
                    page = 0;
                    searchContent.setValue(input);
                }, 500);
            }
        });
    }
}
