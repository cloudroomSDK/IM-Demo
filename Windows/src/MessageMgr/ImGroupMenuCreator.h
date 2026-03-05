#pragma once

class IMGroupMenuCreator
{
public:
	static void FillGroupMemberLstMenu(QMenu *pMenu, const QString &groupId, const QString &userId);

private:
	static void FillForUser(QMenu *pMenu, const CRIMGrpMemberInfo* pMemberInfo);
};
