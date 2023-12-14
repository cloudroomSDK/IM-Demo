package io.crim.android.ouicore.entity;

import java.util.List;

import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.sdk.models.GroupMembersInfo;

public class GroupNotification {
    /// 群信息
    public GrpInfo group;

    /// 当前事件操作者信息
    public GroupMembersInfo opUser;

    /// 群拥有者信息
    public GroupMembersInfo groupOwnerUser;

    /// 产生影响的群成员列表
    public List<GroupMembersInfo> memberList;

    /// 资料发生改变的成员
    GroupMembersInfo changedUser;
}
