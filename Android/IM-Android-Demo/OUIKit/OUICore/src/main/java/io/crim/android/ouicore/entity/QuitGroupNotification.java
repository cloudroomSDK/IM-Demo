package io.crim.android.ouicore.entity;

import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;

public class QuitGroupNotification {
    /// 群信息
    public GrpInfo group;

    /// 退群的成员信息
    public GroupMembersInfo quitUser;
}
