#include "stdafx.h"
#include "CRIM_Defines_Ex.h"

bool groupMembsersLessThan(const CRIMGrpMemberInfo *s1, const CRIMGrpMemberInfo *s2)
{
	return s1->joinTime < s2->joinTime;
}

void CRIMGrpInfo_Ex::updateFirst4Members()
{
	_first4Members.clear();
	for (auto &it : _members)
	{
		_first4Members.push_back(&it);
	}
	qSort(_first4Members.begin(), _first4Members.end(), groupMembsersLessThan);

	_first4Members = _first4Members.mid(0, 4);
}


bool CRIMGrpInfo_Ex::isFirst4Member(const QString &userId) const
{
	for (auto p : _first4Members)
	{
		if (p->userID == userId)
			return true;
	}
	return false;
}
