#include "stdafx.h"
#include "EditMyInfoDlg.h"
#include "ui_EditMyInfoDlg.h"
#include "UICommon_img.h"
#include "DemoSvrWebAPI.h"

#define ICONSIZE	64

EditMyInfoDlg::EditMyInfoDlg(const DemoSvrWebAPI::UserInfo &info, QWidget *parent) : QNoNCDialog(parent)
{
	StandardTitleBar *pTitleBar = new StandardTitleBar(this, StandardTitleBar::TBTN_CLOSE);
	this->setTitleBar(pTitleBar);

	QWidget *pClient = new QWidget(this);
	ui = new Ui::EditMyInfoDlg();
	ui->setupUi(pClient);
	this->setClientWidget(pClient);
	ui->photoBtn->setIconSize(QSize(ICONSIZE, ICONSIZE));
	ui->photoBtn->setFixedSize(ui->photoBtn->iconSize());
	ui->photoBtn->setCursor(Qt::PointingHandCursor);

	_uInfo = info;
	ImIconMgr::instance()->bindIcon(ui->photoBtn, IMIconInfo(_uInfo.userID, IMICON_USR, ui->photoBtn->iconSize()));
	ui->phone->setText(_uInfo.phoneNumber);
	ui->nickname->setText(_uInfo.nickname);
	int genderIndex = (_uInfo.gender == 1 || _uInfo.gender == 2) ? _uInfo.gender - 1 : -1;
	ui->gender->setCurrentIndex(genderIndex);
	ui->birth->setDate(_uInfo.birth != 0 ? QDateTime::fromMSecsSinceEpoch(_uInfo.birth).date() : QDateTime::currentDateTime().date());
	ui->email->setText(_uInfo.email);

	_curTaskID = makeUUID();
	connect(ui->photoBtn, &QToolButton::clicked, this, &EditMyInfoDlg::slot_photoBtnClicked);
	connect(ui->saveBtn, &QToolButton::clicked, this, &EditMyInfoDlg::slot_saveBtnClicked);
	connect(ui->cancelBtn, &QToolButton::clicked, this, &EditMyInfoDlg::close);

	connect(DemoSvrWebAPI::instance(), &DemoSvrWebAPI::s_updateSelfInfoRslt, this, &EditMyInfoDlg::slot_updateSelfInfoRslt);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_uploadFileFailed, this, &EditMyInfoDlg::slot_uploadFileFailed);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_uploadFileSuccess, this, &EditMyInfoDlg::slot_uploadFileSuccess);
}

EditMyInfoDlg::~EditMyInfoDlg()
{
	delete ui;
}

void EditMyInfoDlg::slot_photoBtnClicked()
{
    QString filters = tr("图片 (*.bmp *.jpg *.jpeg *.png)");
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择头像"), "", filters);
	if (fileName.isEmpty())
		return;

	//限制头像尺寸512x512
	QSize sz(512, 512);
	QImage img = getImageFromFile(fileName, sz, Qt::KeepAspectRatioByExpanding);
	if (img.isNull())
	{
		CRMsgBox::information(this, tr("提示"), tr("读取图片失败！"));
		return;
	}
	//尺寸不符合，取中间部分
	if (sz != img.size())
	{
		img = img.copy((img.size().width() - sz.width()) / 2, (img.size().height() - sz.height()) / 2, sz.width(), sz.height());
	}
	bool bHaveAlpha = img.hasAlphaChannel();
	_newPhoto = QPixmap::fromImage(img);
	ui->photoBtn->setIcon(QIcon(_newPhoto));
}

void EditMyInfoDlg::setUIDisabled(bool bDisable)
{
	ui->photoBtn->setDisabled(bDisable);
	ui->phone->setDisabled(bDisable);
	ui->nickname->setDisabled(bDisable);
	ui->gender->setDisabled(bDisable);
	ui->birth->setDisabled(bDisable);
	ui->email->setDisabled(bDisable);
	ui->saveBtn->setDisabled(bDisable);
}


void EditMyInfoDlg::slot_saveBtnClicked()
{
	_uInfo.nickname = ui->nickname->text().trimmed();
	_uInfo.gender = ui->gender->currentIndex() + 1;
	_uInfo.birth = ui->birth->dateTime().toMSecsSinceEpoch();
	_uInfo.email = ui->email->text().trimmed();
	if (_uInfo.nickname.isEmpty())
	{
		CRMsgBox::information(this, tr("提示"), tr("请输入您的名字！"));
		ui->nickname->setFocus();
		return;
	}

	this->setUIDisabled(true);
	_curTaskID = makeUUID();

	//更换了图像
	if (!_newPhoto.isNull())
	{
		uploadImage();
		return;
	}
	else
	{
		updateMyAcntInfo();
	}
}

void EditMyInfoDlg::uploadImage()
{
	bool bPng = _newPhoto.hasAlpha();

	//保存在内存，
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	_newPhoto.save(&buffer, (bPng ? "PNG" : "JPG"), 95);

	//计算md5
	QString fileName = QString("%1face_%2.%3").arg(App::instance()->getAcntChatTempPath()).arg(MakeMd5(bytes)).arg(bPng ? "png" : "jpg");
	WriteDataToFile(bytes, fileName);

	//上传到im
	_uploadTask = fileName;
	CRIMManager_Qt::instance()->uploadFile(fileName);
}


void EditMyInfoDlg::slot_uploadFileFailed(const QString &filePath, int code, const QString &error, const QString &cookie)
{
	if (filePath != _uploadTask)
	{
		return;
	}

	QString msg = tr("头像上传失败！");
	CRMsgBox::information(this, tr("提示"), msg);

	this->setUIDisabled(false);
}

void EditMyInfoDlg::slot_uploadFileSuccess(const QString &filePath, const QString &url, const QString &cookie)
{
	if (filePath != _uploadTask)
	{
		return;
	}

	_uInfo.faceUrl = url;
	updateMyAcntInfo();
}


void EditMyInfoDlg::updateMyAcntInfo()
{
	//由demo服务器完成对im用户信息的更新
	DemoSvrWebAPI::instance()->updateSelfInfo(_uInfo, _curTaskID);
}


void EditMyInfoDlg::slot_updateSelfInfoRslt(const QString &errMsg, const QString &cookie)
{
	if (_curTaskID != cookie)
	{
		return;
	}
	this->setUIDisabled(false);

	if (!errMsg.isEmpty())
	{
		QString msg = tr("保存信息失败！(%1)").arg(errMsg);
		CRMsgBox::information(this, tr("提示"), msg);
		return;
	}

	const CRIMUserInfo &myInfo = CRIMManager_Qt::instance()->getSelfUserInfo();
	CRIMManager_Qt::instance()->setSelfLocInfo(_uInfo.nickname, _uInfo.faceUrl, myInfo.ex);
	this->accept();
}
