package io.crim.android.ouicore.entity;

import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;

public class MuteMemberNotification {
    /// 群信息
    public GrpInfo group;

    /// 操作者信息
    public GroupMembersInfo opUser;

    /// 被禁言的成员信息
    public GroupMembersInfo mutedUser;

    /// 禁言时间s
    public  int mutedSeconds;
}
