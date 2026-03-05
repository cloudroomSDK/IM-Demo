#ifndef CRIM_DEFINES_EX_H
#define CRIM_DEFINES_EX_H

#include "CRIM_Defines.h"

class CRIMManager_Qt;
struct CRIMGrpInfo_Ex : public CRIMGrpInfo
{
	QMap<QString, CRIMGrpMemberInfo>	_members;
	QList<CRIMGrpMemberInfo*>			_first4Members;	//前4个入会者

	CRIMGrpInfo_Ex() : _bMembersGetState(0) {}

	bool isFirst4Member(const QString &userId) const;

protected:
	friend class CRIMManager_Qt;
	void updateFirst4Members();
	int _bMembersGetState;	//内部使用，-1:数据异常，0:未获取，1：获取中，2：已获取
};


#endif // CRIM_DEFINES_EX_H
