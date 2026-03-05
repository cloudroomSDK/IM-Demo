#include "stdafx.h"
#include "ImGroupMenuCreator.h"

void IMGroupMenuCreator::FillGroupMemberLstMenu(QMenu *pMenu, const QString &groupId, const QString &userId)
{
	const CRIMGrpMemberInfo *pInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(groupId, userId);
	if (NULL == pInfo)
		return;

	FillForUser(pMenu, pInfo);
}

void IMGroupMenuCreator::FillForUser(QMenu *pMenu, const CRIMGrpMemberInfo* pMemberInfo)
{
	const CRIMGrpMemberInfo *pMyInfo = CRIMManager_Qt::instance()->getGrpMemberInfo(pMemberInfo->groupID, CRIMManager_Qt::instance()->getUserID());
	if (pMyInfo == NULL)
		return;

	//是否为自己
	bool bSelf = (pMemberInfo->userID == CRIMManager_Qt::instance()->getUserID());
	if (!bSelf)
	{
		pMenu->addAction(g_atMgr.getActionById(ACT_START_Chat1v1, pMemberInfo->userID));
	}
	pMenu->addAction(g_atMgr.getActionById(GMEM_ACT_SHOW_GrpMemberInfo, pMemberInfo->groupID, pMemberInfo->userID));
	if (!bSelf)
	{
		pMenu->addAction(g_atMgr.getActionById(GMEM_ACT_AT_USER, pMemberInfo->groupID, pMemberInfo->userID));
		
		const CRIMGrpInfo_Ex *pGrpInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(pMemberInfo->groupID);
		if (pGrpInfo != NULL)
		{
			//我是群主
// 			if (pMyInfo->isOwner())
// 			{
// 				pMenu->addSeparator();
// 
// 			}
			
			//我是管理员
			if (pMyInfo->isManager())
			{
				bool bAddSeparator = true;
				//转让群主
				if (pMyInfo->isOwner())
				{
					if (bAddSeparator)
					{
						pMenu->addSeparator();
						bAddSeparator = false;
					}
					//转让群主
					pMenu->addAction(g_atMgr.getActionById(GMEM_ACT_TRANS_OWNER, pMemberInfo->groupID, pMemberInfo->userID));
				}

				//设置/收回管理员
				if (!pMemberInfo->isOwner())
				{
					if (bAddSeparator)
					{
						pMenu->addSeparator();
						bAddSeparator = false;
					}
					//对方是管理员
					if (pMemberInfo->isManager())
					{
						pMenu->addAction(g_atMgr.getActionById(GMEM_ACT_UNSET_HOST, pMemberInfo->groupID, pMemberInfo->userID));
					}
					else//对方不是管理员
					{
						pMenu->addAction(g_atMgr.getActionById(GMEM_ACT_SET_HOST, pMemberInfo->groupID, pMemberInfo->userID));
					}
				}
				
				//踢人
				if (!pMemberInfo->isOwner())
				{
					pMenu->addSeparator();
					pMenu->addAction(g_atMgr.getActionById(GMEM_ACT_KICK, pMemberInfo->groupID, pMemberInfo->userID));
				}
			}
		}
	}
}
