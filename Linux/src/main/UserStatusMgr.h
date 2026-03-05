#ifndef USER_STATUS_MGR_H
#define USER_STATUS_MGR_H


class UserStatusMgr : public QObject
{
	Q_OBJECT

public:
	static UserStatusMgr *instance();

	void setLoingState(bool bLogined);
	void setStatusManual(USER_STATUS status);
	void setIMCallStatus(bool inIMCall);

	void setMeetingStatus(bool inMeeting);
	bool getMeetingStatus() { return _inMeeting; }

	USER_STATUS getAppStatus() { return _appStatus; }

signals:
	void s_appStatusChanged(USER_STATUS oldStatus, USER_STATUS newStatus);


private:
	UserStatusMgr();
	~UserStatusMgr();

	void updateAppStatus();


private:
	bool			_bLogined{ false };
	bool			_inIMCall{ false };
	bool			_inMeeting{ false };
	USER_STATUS		_manualStatus{ US_ONLINE };
	USER_STATUS		_appStatus{ US_OFFLINE };		//当前APP的状态
};



#endif // USER_STATUS_MGR_H
