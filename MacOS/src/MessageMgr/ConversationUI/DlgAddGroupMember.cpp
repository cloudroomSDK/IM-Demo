#include "stdafx.h"
#include "DlgAddGroupMember.h"

DlgAddGroupMember::DlgAddGroupMember(const QString &groupId, QWidget *parent)
	: QNoNCDialog(parent)
{
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
	this->setTitleBar(pTitleBar);
	QWidget *pClient = new QWidget(this);
	ui.setupUi(pClient);
	this->setClientWidget(pClient);

	m_groupId = groupId;

	const CRIMGrpInfo_Ex *gInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(groupId);
	SelectMembersWidget::SelectCfg cfg;
	for (auto &info : gInfo->_members)
	{
		cfg.disableIds.insert(info.userID);
	}
	cfg.maxSelect = qMax(100 - gInfo->memberCount, 0);
	ui.selectWidget->init(cfg);

	connect(ui.btnOk, &QPushButton::clicked, this, &DlgAddGroupMember::slot_btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &DlgAddGroupMember::slot_btnCancelClicked);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_inviteUserToGrpFailed, this, &DlgAddGroupMember::slot_inviteUserToGrpFailed);
}

DlgAddGroupMember::~DlgAddGroupMember()
{
}

void DlgAddGroupMember::slot_btnOkClicked()
{
	QList<QString> ids = ui.selectWidget->getSelectedIds();
	CRIMManager_Qt::instance()->inviteUserToGrp(m_groupId, ids, "");
	this->accept();
}

void DlgAddGroupMember::slot_btnCancelClicked()
{
	this->reject();
}

void DlgAddGroupMember::slot_inviteUserToGrpFailed(int code, const QString &error, const QString &cookie)
{
	CRMsgBox::information(this, tr("提示"), tr("邀请群聊成员失败"));
}