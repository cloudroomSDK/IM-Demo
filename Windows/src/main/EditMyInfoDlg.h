#ifndef EDITMYINFODLG_H
#define EDITMYINFODLG_H

#include "DemoSvrWebAPI.h"
namespace Ui {class EditMyInfoDlg;};

class EditMyInfoDlg : public QNoNCDialog
{
	Q_OBJECT

public:
	EditMyInfoDlg(const DemoSvrWebAPI::UserInfo &info, QWidget *parent = 0);
	~EditMyInfoDlg();

protected slots:
	void slot_photoBtnClicked();
	void slot_saveBtnClicked();
	void slot_uploadFileFailed(const QString &filePath, int code, const QString &error, const QString &cookie = QString());
	void slot_uploadFileSuccess(const QString &filePath, const QString &url, const QString &cookie = QString());
	void slot_updateSelfInfoRslt(const QString &errMsg, const QString &cookie);

protected:
	void setUIDisabled(bool bDisable);
	void uploadImage();
	void updateMyAcntInfo();

private:
	Ui::EditMyInfoDlg *ui;
	QString _curTaskID;

	QPixmap	_newPhoto;
	QString _uploadTask;
	DemoSvrWebAPI::UserInfo _uInfo;
};

#endif // EDITMYINFODLG_H
