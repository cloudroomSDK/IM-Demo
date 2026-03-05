#ifndef IMCALLDEF_H
#define IMCALLDEF_H

enum IMCallType
{
	IMVideoCall = 0,
	IMAudioCall
};

const int CALL_TIMEOUT_SECOND = 30;

const QString &getIMCallTypeName(IMCallType type);
IMCallType getIMCallType(const QString &typeName);

bool bTheSameInvitation(const CRIMInvitationInfo &inf1, const CRIMInvitationInfo &inf2);
void clearInvitation(CRIMInvitationInfo &inf);

#endif