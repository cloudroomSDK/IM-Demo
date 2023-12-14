package io.crim.android.ouicore.entity;

import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;

public class EnterGroupNotification {
    /// 群信息
    public GrpInfo group;

    /// 进入群的成员信息
    public GroupMembersInfo entrantUser;
}
