#include "stdafx.h"
#include "ImCallDef.h"

const QString& getIMCallTypeName(IMCallType type)
{
	static QString strv("video");
	static QString stra("audio");

	return (type == IMVideoCall) ? strv : stra;
}

IMCallType getIMCallType(const QString &typeName)
{
	return (typeName == getIMCallTypeName(IMVideoCall)) ? IMVideoCall : IMAudioCall;
}


bool bTheSameInvitation(const CRIMInvitationInfo &inf1, const CRIMInvitationInfo &inf2)
{
	return (inf1.invitationMsgId.length()>0
		&& inf1.invitationMsgId == inf2.invitationMsgId
		);
}

void clearInvitation(CRIMInvitationInfo &inf)
{
	inf.invitationMsgId.clear();
}