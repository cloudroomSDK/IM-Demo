#include "stdafx.h"
#include "UserStatusMgr.h"
#include "MainDialog.h"

UserStatusMgr *UserStatusMgr::instance()
{
	static UserStatusMgr ins;
	return &ins;
}

UserStatusMgr::UserStatusMgr()
{
	qRegisterMetaType<USER_STATUS>("USER_STATUS");
}

UserStatusMgr::~UserStatusMgr()
{

}

void UserStatusMgr::setLoingState(bool bLogined)
{
	MainLogDebug("setLoingState: %d", bLogined);
	_bLogined = bLogined;
	updateAppStatus();
}

void UserStatusMgr::setStatusManual(USER_STATUS status)
{
	MainLogDebug("UserStatusMgr, status: %d", status);
	_manualStatus = status;
	updateAppStatus();
}

void UserStatusMgr::setIMCallStatus(bool inIMCall)
{
	MainLogDebug("setIMCallStatus, inIMCall: %d", inIMCall);
	_inIMCall = inIMCall;
	updateAppStatus();
}

void UserStatusMgr::setMeetingStatus(bool inMeeting)
{
	MainLogDebug("setMeetingStatus, inMeeting: %d", inMeeting);
	_inMeeting = inMeeting;
	updateAppStatus();
}

void UserStatusMgr::updateAppStatus()
{
	USER_STATUS oldStatus = _appStatus;

	if (!_bLogined)
	{
		_appStatus = US_OFFLINE;
	}
	else
	{
		_appStatus = _manualStatus;
		if (_inMeeting)
		{
			_appStatus = US_INMEETING;
		}
		else if (_inIMCall)
		{
			_appStatus = US_BUSY;
		}
	}

	if (oldStatus != _appStatus)
	{
		MainLogDebug("app status changed, %d -> %d", oldStatus, _appStatus);
		emit s_appStatusChanged(oldStatus, _appStatus);
	}
}
