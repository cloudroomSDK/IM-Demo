#include "stdafx.h"
#include "ImCallMgr.h"
#include "ImCalloutDlg.h"
#include "ImCallinDlg.h"
#include "MainDialog.h"
#include "UserStatusMgr.h"

ImCallMgr::ImCallMgr(QObject *parent) : QObject(parent)
{
	connect(g_atMgr.getActionById(ACT_START_AudioChat1v1, QString()), &QAction::triggered, this, &ImCallMgr::slot_MEM_START_AudioChat1v1);
	connect(g_atMgr.getActionById(ACT_START_VideoChat1v1, QString()), &QAction::triggered, this, &ImCallMgr::slot_MEM_START_VideoChat1v1);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_notifyCallIn, this, &ImCallMgr::slot_notifyCallIn);
}

ImCallMgr::~ImCallMgr()
{
}

void ImCallMgr::slot_MEM_START_AudioChat1v1()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString userID = pAct->property(EXT_ID).toString();
	start1v1ImCall(IMAudioCall, userID);
}

void ImCallMgr::slot_MEM_START_VideoChat1v1()
{
	QAction *pAct = dynamic_cast<QAction*>(sender());
	if (pAct == NULL)
		return;

	QString userID = pAct->property(EXT_ID).toString();
	start1v1ImCall(IMVideoCall, userID);
}

void ImCallMgr::start1v1ImCall(IMCallType type, const QString &userID)
{
	if (UserStatusMgr::instance()->getAppStatus() != US_ONLINE)
	{
		CRMsgBox::information(MainDialog::instance(), tr("提示"), tr("当前已在会议中或通话中，不能再发起通话。"));
		return;
	}

	ImCalloutDlg *pDlg = static_cast<ImCalloutDlg*>(_outDlg.GetObject());
	if (pDlg != NULL)
	{
		pDlg->show();
		pDlg->raise();
		return;
	}

	pDlg = new ImCalloutDlg(type, userID, MainDialog::instance());
	_outDlg.SetObject(pDlg);
	pDlg->show();
	return;
}

void ImCallMgr::slot_notifyCallIn(const CRIMInvitationInfo &info)
{
	if (UserStatusMgr::instance()->getAppStatus() != US_ONLINE)
	{
		CRIMInvitationInfo rspInfo(info);
		rspInfo.opType = CRIVT_OP_Busy;
		rspInfo.opDesc = "busy";
		CRIMManager_Qt::instance()->rejectCall(rspInfo);
		return;
	}

	ImCallinDlg *pDlg = static_cast<ImCallinDlg*>(_inDlg.GetObject());
	if (pDlg != NULL)
	{
		pDlg->show();
		pDlg->raise();
		return;
	}

	pDlg = new ImCallinDlg(info, MainDialog::instance());
	_inDlg.SetObject(pDlg);
	pDlg->show();
	return;
}

