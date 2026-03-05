#ifndef __UIACTION_MGR_H__
#define __UIACTION_MGR_H__

//Action附加数据类型
#define EXT_ID		"extID"
#define EXT_PARAMS	"extParams"

//与成员相关Action
enum ACTION_ID
{
	ACTION_ID_BEGIN = 0,

	ACT_START_Chat1v1,		//进入1v1聊天(EXT_ID存有目标用户ID)
	ACT_START_ChatGrp,		//进入群聊(EXT_ID存有目标群ID)
	ACT_START_AudioChat1v1,	//进入1v1语音聊天(EXT_ID存有目标用户ID)
	ACT_START_VideoChat1v1,	//进入1v1视频聊天(EXT_ID存有目标用户ID)
	ACT_START_AudioChatGrp,	//进入群语音聊天(EXT_ID存有目标群ID)
	ACT_START_VideoChatGrp,	//进入群视频聊天(EXT_ID存有目标群ID)

	ACT_NEW_GrpChat,		//新的群聊

	ACT_SearchFor_AddFriend,//搜索并添加好友
	ACT_NEW_Friend,			//新的好友
	ACT_Del_Friend,			//删除好友(EXT_ID存有目标用户ID)
	ACT_SHOW_FriendInfo,	//查看好友资料(EXT_ID存有目标用户ID)

	ACT_CFG_Friend,			//设置好友(EXT_ID存有目标用户ID)
	ACT_CFG_Grp,			//设置群组(EXT_ID存有目标群ID)

	ACT_IM_VIEWCONTEXT,			//聊天记录查看上下文,(EXT_PARAMS中存有 UIMsgInfo)
	ACT_ClearConverstationMsg,	//清空聊天记录， (EXT_ID存有ConverstationID)

	ACTION_ID_BUTT
};

//Action附加数据类型
#define EXT_GID		"extGID"
#define EXT_UID		"extUID"
//与群成员相关Action，包含EXT_GID：群id, EXT_UID：用户id，
enum GMEM_ACTION_ID
{
	GMEM_ACTION_ID_BEGIN = ACTION_ID_BUTT,

	GMEM_ACT_AT_USER = GMEM_ACTION_ID_BEGIN,			//@群成员
	GMEM_ACT_TRANS_OWNER,								//转让群主
	GMEM_ACT_SET_HOST,									//设置管理员
	GMEM_ACT_UNSET_HOST,								//收回管理员
	GMEM_ACT_KICK,										//请出群
	GMEM_ACT_SHOW_GrpMemberInfo,						//查看群成员资料

	GMEM_ACT_ACTION_ID_BUTT
};


class ActionMgr_Mgr : public QObject
{
	Q_OBJECT

public:
	ActionMgr_Mgr();
    static ActionMgr_Mgr* instance();
	void ReSetAllActionState();

	QAction* getActionById(ACTION_ID acId, const QString &id=QString(), const QVariant &var = QVariant());
	QAction* getActionById(GMEM_ACTION_ID acId, const QString &gid, const QString &uid);

private:
	QAction* m_Actions[GMEM_ACT_ACTION_ID_BUTT];

	void init();
	void udpateAction(QAction *pAction, int acId, const QString& uid, const QVariant &var);
	void udpateAction(QAction *pAction, int acId, const QString& gid, const QString& uid);
	void setActionV_E(QAction *pAction, bool bVisiable, bool bEnable);
	void resetActionState(QAction *pAction, void *actionData);
};

#define g_atMgr (*ActionMgr_Mgr::instance())

#endif	//__UIACTION_MGR_H__
