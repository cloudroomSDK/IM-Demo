package io.crim.android.ouicore.vm;

/**
 * Created by zjw on 2025/2/19.
 */
public class SelectTargetVM extends SocialityVM {
    //封装过的群成员 用于字母导航
//    public MutableLiveData<List<ExGroupMemberInfo>> exGroupMembers =
//        new MutableLiveData<>(new ArrayList<>());
    //群管理
//    public MutableLiveData<List<ExGroupMemberInfo>> exGroupManagement =
//        new MutableLiveData<>(new ArrayList<>());
    //群字母导航
//    public MutableLiveData<List<String>> groupLetters = new MutableLiveData<>(new ArrayList<>());
    //超级群成员分页加载
//    public MutableLiveData<List<ExGroupMemberInfo>> superGroupMembers = new MutableLiveData<>(new ArrayList<>());
    //群所有成员
//    public MutableLiveData<List<GroupMembersInfo>> groupMembers = new MutableLiveData<>(new ArrayList<>());

    /**
     * 获取群成员信息
     */
   /* public void getGroupMemberList(int count) {
        if (!superGroupMembers.getValue().isEmpty()) return; //表示走了超级大群逻辑

        exGroupMembers.getValue().clear();
        exGroupManagement.getValue().clear();
        groupLetters.getValue().clear();
        CRIMClient.getInstance().groupManager.getGrpMemberList(new OnBase<List<GroupMembersInfo>>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + "-" + code);
            }

            @Override
            public void onSuccess(List<GroupMembersInfo> data) {
                if (data.isEmpty()) return;
                groupMembers.setValue(data);

                List<ExGroupMemberInfo> exGroupMemberInfos = new ArrayList<>();
                List<ExGroupMemberInfo> otGroupMemberInfos = new ArrayList<>();
                for (GroupMembersInfo datum : data) {
                    ExGroupMemberInfo exGroupMemberInfo = new ExGroupMemberInfo();
                    exGroupMemberInfo.groupMembersInfo = datum;
                    if (datum.getRoleLevel() != GrpRole.MEMBER) {
                        //群管理单独存放
                        exGroupMemberInfo.sortLetter = "";
                        exGroupManagement.getValue().add(exGroupMemberInfo);
                        continue;
                    }
                    String nickName = "0";
                    if (!TextUtils.isEmpty(datum.getNickname())) nickName = datum.getNickname();
                    String letter = Pinyin.toPinyin(nickName.charAt(0));
                    letter = (letter.charAt(0) + "").trim().toUpperCase();
                    if (!Common.isAlpha(letter)) {
                        exGroupMemberInfo.sortLetter = "#";
                        otGroupMemberInfos.add(exGroupMemberInfo);
                    } else {
                        exGroupMemberInfo.sortLetter = letter;
                        exGroupMemberInfos.add(exGroupMemberInfo);
                    }
                    if (!groupLetters.getValue().contains(exGroupMemberInfo.sortLetter))
                        groupLetters.getValue().add(exGroupMemberInfo.sortLetter);
                }

                Collections.sort(groupLetters.getValue(), new LettersPinyinComparator());
                groupLetters.getValue().add(0, "↑");
                groupLetters.setValue(groupLetters.getValue());

                exGroupMembers.getValue().addAll(exGroupMemberInfos);
                exGroupMembers.getValue().addAll(otGroupMemberInfos);

                Collections.sort(exGroupMembers.getValue(), new PinyinComparator());
                Collections.sort(exGroupManagement.getValue(), (o1, o2) -> {
                    if (o2.groupMembersInfo.getRoleLevel() < o1.groupMembersInfo.getRoleLevel())
                        return 0;
                    return -1;
                });
                exGroupMembers.setValue(exGroupMembers.getValue());
            }
        }, groupId, 0, 0, 0);
    }*/
}
