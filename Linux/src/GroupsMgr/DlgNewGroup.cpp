#include "stdafx.h"
#include "DlgNewGroup.h"

DlgNewGroup::DlgNewGroup(const QSet<QString> &initUsrIds, QWidget *parent)
	: QNoNCDialog(parent)
{
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, TitleBar::TBTN_CLOSE);
	//pTitleBar->setRightBtnsAlign(Qt::AlignCenter);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QWidget(this);
	ui.setupUi(pClient);
	this->setClientWidget(pClient);

	ui.gridLayout->setContentsMargins(25, 35, 25, 25);
	ui.gridLayout->setHorizontalSpacing(20);
	ui.gridLayout->setVerticalSpacing(15);
	ui.horizontalLayout->setContentsMargins(0, 0, 0, 0);
	ui.horizontalLayout->setSpacing(10);
	SelectMembersWidget::SelectCfg cfg;
	cfg.maxSelect = 99; //自已不在其内
	cfg.selectedIds = initUsrIds;
	ui.selectWidget->init(cfg);

	connect(ui.btnOk, &QPushButton::clicked, this, &DlgNewGroup::slot_btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &DlgNewGroup::slot_btnCancelClicked);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_createGrpSuccess, this, &DlgNewGroup::slot_createGroupSuccess);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_createGrpFailed, this, &DlgNewGroup::slot_createGroupFailed);
}

DlgNewGroup::~DlgNewGroup()
{
}

void DlgNewGroup::slot_btnOkClicked()
{
	QString groupName = ui.editGroupName->text();
	if (groupName.isEmpty())
	{
		CRMsgBox::information(this, tr("提示"), tr("群名称不能为空"));
		return;
	}
	CRIMGrpCreateInfo createInfo;
	createInfo.groupInfo.groupName = groupName;
	createInfo.groupInfo.groupType = CRIM_Group_Normal;
	
	QList<QString> ids = ui.selectWidget->getSelectedIds();
	ids.removeAll(CRIMManager_Qt::instance()->getUserID());
	createInfo.memberUserIDs = ids;
	createInfo.adminUserIDs = QStringList();
	createInfo.ownerUserID = CRIMManager_Qt::instance()->getUserID();

	CRIMManager_Qt::instance()->createGrp(createInfo);
	ui.btnOk->setEnabled(false);
	ui.editGroupName->setEnabled(false);
	ui.selectWidget->setEnabled(false);
}

void DlgNewGroup::slot_btnCancelClicked()
{
	this->reject();
}

void DlgNewGroup::slot_createGroupSuccess(const CRIMGrpInfo &info, const QString &cookie)
{
	CRIMGrpInfo newInfo = info;
	newInfo.needVerification = CRIM_GVT_Directly;
	CRIMManager_Qt::instance()->setGrpInfo(newInfo);

	this->accept();
	g_atMgr.getActionById(ACT_START_ChatGrp, info.groupID)->trigger();
}


void DlgNewGroup::slot_createGroupFailed(int code, const QString &error, const QString &cookie)
{
	ui.btnOk->setEnabled(true);
	ui.editGroupName->setEnabled(true);
	ui.selectWidget->setEnabled(true);
	CRMsgBox::information(this, tr("提示"), tr("创建群聊失败"));
}
