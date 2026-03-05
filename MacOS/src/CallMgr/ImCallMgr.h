#ifndef ImCallMgr_h
#define ImCallMgr_h

#include "ImCallDef.h"
#include "QObjectGuard.h"

class ImCallMgr : public QObject
{
	Q_OBJECT

public:
	ImCallMgr(QObject *parent = 0);
	~ImCallMgr();

protected slots:
	void slot_MEM_START_AudioChat1v1();
	void slot_MEM_START_VideoChat1v1();

	void slot_notifyCallIn(const CRIMInvitationInfo &info);


protected:
	void start1v1ImCall(IMCallType type, const QString &userID);

private:
	QObjectGuard _outDlg;
	QObjectGuard _inDlg;

};

#endif // ImCallMgr_h
