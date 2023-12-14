package io.crim.android.ouicore.entity;

import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;

public class GroupRightsTransferNotification {
    /// 群信息
    public GrpInfo group;

    /// 操作者信息
    public GroupMembersInfo opUser;

    /// 群新的拥有者信息
    public GroupMembersInfo newGroupOwner;
}
