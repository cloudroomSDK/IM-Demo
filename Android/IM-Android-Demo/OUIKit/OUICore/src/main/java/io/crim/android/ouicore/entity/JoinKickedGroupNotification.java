package io.crim.android.ouicore.entity;

import java.util.List;

import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.sdk.models.GroupMembersInfo;

public class JoinKickedGroupNotification {
    /// 群信息
    public GrpInfo group;

    /// 操作者信息
    public GroupMembersInfo opUser;

    /// 被邀请进群的成员信息
    public List<GroupMembersInfo> invitedUserList;
    ///  被踢出群的成员信息列表
    public List<GroupMembersInfo> kickedUserList;
}
