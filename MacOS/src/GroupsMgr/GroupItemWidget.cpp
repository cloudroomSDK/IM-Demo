#include "stdafx.h"
#include "GroupItemWidget.h"
#include "CRIM_Manager.h"
#include "ConversationBaseWidget.h"

GroupListItem::GroupListItem(const QString &grpID, QListWidget *view)
{
	m_grpID = grpID;
	m_listView = view;
}

GroupListItem::~GroupListItem()
{
}

void GroupListItem::updateInfo(const QString &grpID)
{
	m_grpID = grpID;
}

bool GroupListItem::operator<(const QListWidgetItem &other) const
{
	if (m_grpID == g_createNewGrp)
		return true;

	const GroupListItem* pOther = static_cast<const GroupListItem*>(&other);
	if (pOther->m_grpID == g_createNewGrp)
		return false;

	const CRIMGrpInfo_Ex *pOtherInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(pOther->m_grpID);
	if (pOtherInfo == NULL)
		return false;
	const CRIMGrpInfo_Ex *pMyInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(m_grpID);
	if (pMyInfo == NULL)
		return false;

	return pMyInfo->createTime > pOtherInfo->createTime;
}

///////////////////////////////////////////////
GroupItemWidget::GroupItemWidget(GroupListItem *item, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_item = item;

	ui.lbGroupFace->setFixedSize(DEFUALT_ICONSZ);
	updateUI();
}

GroupItemWidget::~GroupItemWidget()
{

}


void GroupItemWidget::updateUI()
{
	if (m_item->getGrpID() == g_createNewGrp)
	{
		ui.lbGroupFace->setPixmap(svgRes2Pixmap(":/Res/newGroup.svg", ui.lbGroupFace->size()));
		ui.lbName->setText(tr("创建群组"));
		ui.lbNumber->hide();
		this->setToolTip(ui.lbName->text());
	}
	else
	{
		const CRIMGrpInfo_Ex *info = CRIMManager_Qt::instance()->getJoinedGrpInfo(m_item->getGrpID());
		if (info == NULL)
		{
			return;
		}
		ImIconMgr::instance()->bindIcon(ui.lbGroupFace, IMIconInfo(info->groupID, IMICON_GRP, ui.lbGroupFace->size()));
		ui.lbName->setText(info->groupName);
		ui.lbNumber->setText(tr("%1人").arg(info->memberCount));
		this->setToolTip(info->groupName);
	}
}

