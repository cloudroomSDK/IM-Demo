#include "stdafx.h"
#include "IMMsgWidget.h"
#include "CRIM_Manager.h"
#include "ImDownloadFile.h"
#include "mypicswidget.h"

static QSet<QString> g_safeOpenFiles = {
	".doc", ".docx", ".dot", ".dotx", ".rtf", ".xps", ".odt",
	".ppt", ".pptx", ".pps", ".ppsx", ".pot", ".potx", ".odp",
	".xls", ".xlsx", ".xlt", ".xltx", ".xlw", ".xlwx",
	".et", ".ett", ".wps", ".wpt", ".dps", ".dpt",
	".txt", ".log", ".ini", ".cvs", 
	".xml", ".md", ".pdf", 
	".bmp", ".png", ".jpg", ".jpeg", ".gif", ".svg", ".ico", ".tif", ".psd", ".eps", ".jpf", ".jpx", ".jp2", ".j2k", ".pbm", ".pgm", ".ppm", ".xbm", ".xpm",
	".mp4", ".m4v", ".3gp", ".mpg", ".flv", ".f4v", ".swf", ".avi", ".wmv", ".rmvb", ".mov", ".mts", ".m2t", ".webm", ".ogg", ".mkv",
	".mp3", ".wav", ".mid", ".aif", ".aiff", ".wma", ".ra", ".vqf", ".mov",
	".dwg", ".dxf", ".dwt", ".dws",
	".pcbdoc", ".schdoc", ".prjpcb",
};

IMMsgWidget::IMMsgWidget(QWidget *parent): ChatWidget(parent)
{
	m_type = MSGS_NULL; 
	m_act_copy = NULL;
	m_downloadCookie = makeUUID();
	connect(ImDownloadFile::instance(), &ImDownloadFile::s_Err, this, &IMMsgWidget::slot_downloadFileErr);
	connect(ImDownloadFile::instance(), &ImDownloadFile::s_Progress, this, &IMMsgWidget::slot_downloadFileProgress);
	connect(ImDownloadFile::instance(), &ImDownloadFile::s_finished, this, &IMMsgWidget::slot_downloadFileFinished);
	connect(this, &ChatWidget::s_faceClicked, this, &IMMsgWidget::slot_faceClicked);
	connect(this, &ChatWidget::s_contentClicked, this, &IMMsgWidget::slot_contentClicked);
	connect(this, &ChatWidget::s_rightClicked, this, &IMMsgWidget::slot_rightClicked);
	initShortcut();
}

IMMsgWidget::~IMMsgWidget()
{

}

void IMMsgWidget::initShortcut()
{
	m_act_copy = new QAction(tr("复制"), this);
	m_act_copy->setShortcut(QKeySequence::Copy);
	connect(m_act_copy, &QAction::triggered, this, &IMMsgWidget::slot_act_copyClicked);
	addAction(m_act_copy);
}

void IMMsgWidget::setType(WidgetType type)
{
	m_type = type;
}

bool IMMsgWidget::isSessionMsg(const CRIMMessage &msg)
{
	//会话类型不一样
	if (m_conversationInfo.conversationType != msg.sessionType)
	{
		return false;
	}
	//只支持单聊和群聊
	if (msg.sessionType != CRIM_Conversation_1v1 && msg.sessionType != CRIM_Conversation_Group)
	{
		return false;
	}
	//不支持的消息类型
	if (!isSupportMsgType(msg.contentType))
	{
		return false;
	}

	//单聊
	if (msg.sessionType == CRIM_Conversation_1v1)
	{
		//发送人是该单聊对象
		if (msg.sendID == m_conversationInfo.userID)
		{
			return true;
		}
		// 其他端当前登录用户向该单聊对象发送的消息
		if (msg.sendID == CRIMManager_Qt::instance()->getUserID() && msg.recvID == m_conversationInfo.userID)
		{
			return true;
		}
	}
	else//群聊
	{
		if (msg.groupID == m_conversationInfo.groupID)
		{
			return true;
		}
	}

	return false;
}

QString IMMsgWidget::getDownloadFileName(const UIMsgInfo &info)
{
	QString fileName = info.getLocFilePath();
	//如果是文件，需要唯一性处理
	if (info.contentType == CRIM_MsgType_File)
	{
		QString fileStorage = info.getFileStorageDir();
		QFileInfo fi(info.getFileName());
		int index = 0;
		while (1)
		{
			if (index == 0)
			{
				fileName = QString("%1%2.%3").arg(fileStorage).arg(fi.baseName()).arg(fi.suffix());
			}
			else
			{
				fileName = QString("%1%2(%3).%4").arg(fileStorage).arg(fi.baseName()).arg(index).arg(fi.suffix());
			}
			//如果不存在同名的文件和同名的下载文件，则可以使用，如果存在，则需要继续找到一个没有被占用的文件名
			if (!QFileInfo::exists(fileName) && !QFileInfo::exists(fileName + DOWNLOAD_FILE_SUFFIX))
			{
				break;
			}

			index++;
		}
	}
	return fileName;
}

void IMMsgWidget::downloadFile(const QString &msgId)
{
	UIMsgInfo info = this->getMsgInfo(msgId);
	if (info.clientMsgID.isEmpty())
		return;
	
	//不是未接收的文件不用处理
// 	if (info.fileState != UFS_NORECV)
// 	{
// 		return;
// 	}
	//我自己发送的消息无需下载
// 	if (info.isMyMsg())
// 		return;

	//更新UI
	info.fileState = UFS_RECVING;
	info.fileProgress = (float)0.01;
	this->updateMsg(info);

	//获取一个唯一的下载文件名，下载成功后将文件名更新到db和内存
	QString downFileName = getDownloadFileName(info);
	QString downloadUrl = info.getDownloadUrl();

	QVariantMap params;
	params["msgId"] = msgId;
	params["cookie"] = m_downloadCookie;
	ImDownloadFile::instance()->download(downloadUrl, downFileName, params);
}

void IMMsgWidget::slot_downloadFileErr(const QString &url, const QString &errMsg, const QVariant &cookie)
{
	QVariantMap params = cookie.toMap();
	QString msgId = params["msgId"].toString();
	bool isMyDownload = params["cookie"].toString() == m_downloadCookie;

	UIMsgInfo info = this->getMsgInfo(msgId);
	if (info.clientMsgID.isEmpty())
		return;

	info.fileState = UFS_RECV_FAILED;
	this->updateMsg(info);
}

void IMMsgWidget::slot_downloadFileProgress(const QString &url, qint64 pos, qint64 size, const QVariant &cookie)
{
	QVariantMap params = cookie.toMap();
	QString msgId = params["msgId"].toString();
	bool isMyDownload = params["cookie"].toString() == m_downloadCookie;

	UIMsgInfo info = this->getMsgInfo(msgId);
	if (info.clientMsgID.isEmpty())
		return;

	info.fileState = UFS_RECVING;
	info.fileProgress = (float)pos / size;
	this->updateMsg(info);
}

void IMMsgWidget::slot_downloadFileFinished(const QString &url, const QString &locPathFileName, const QVariant &cookie)
{
	QVariantMap params = cookie.toMap();
	QString msgId = params["msgId"].toString();
	bool isMyDownload = params["cookie"].toString() == m_downloadCookie;

	UIMsgInfo info = this->getMsgInfo(msgId);
	if (info.clientMsgID.isEmpty())
		return;

	//如果是文件类型，需要更新本地文件名
	if (info.contentType == CRIM_MsgType_File)
	{
		info.setLocFilePath(locPathFileName, m_conversationInfo.conversationID);
	}
	info.fileState = UFS_NORMAL/*QFileInfo::exists(locFilePath) ? UFS_NORMAL : UFS_NORECV*/;
	this->updateMsg(info);
}

void IMMsgWidget::slot_faceClicked(const UIMsgInfo &msg)
{
	g_atMgr.getActionById(GMEM_ACT_SHOW_GrpMemberInfo, msg.groupID, msg.sendID)->trigger();
}

void IMMsgWidget::slot_contentClicked(CursorPosType type, const QString &cursorText, const UIMsgInfo &msg)
{
	contentClickEvent(type, cursorText, msg);
}

void IMMsgWidget::contentClickEvent(CursorPosType type, const QString &cursorText, const UIMsgInfo &msg)
{	
	//点击超链接
	if (type == CURSOR_ANCHOR)
	{
		//点击在web连接上
		QRegExp UrlRx("((www\\.|(http|https|ftp)://)([.@]?[a-z0-9/_:@=+?,&;#%~-]{1,}){1,})");
		UrlRx.setCaseSensitivity(Qt::CaseInsensitive);
		if (cursorText.contains(UrlRx))
		{
			QDesktopServices::openUrl(cursorText);
		}
		return;
	}
	
	if (type == CURSOR_IMAGE)//图片
	{
		QUrl u(cursorText);
		QString filePathName;
		if (u.isLocalFile())
		{
			filePathName = u.toLocalFile();
		}

		if (QFile::exists(filePathName))
		{
			MypicsWidget::showInstance(filePathName, this);
		}
		else
		{
			downloadFile(msg.clientMsgID);
		}
		return;
	}

	if (type == CURSOR_FILE)//文件
	{
		QUrl u(cursorText);
		QString filePathName;
		if (u.isLocalFile())
		{
			filePathName = u.toLocalFile();
		}
		//文件存在则打开文件夹，不存在需要下载
		if (!QFile::exists(filePathName))
		{
			downloadFile(msg.clientMsgID);
			return;
		}

		//对系统无风险的文件，直接打开
		QFileInfo fInfo(filePathName);
		QString fileExt = fInfo.suffix().toLower();
		if (g_safeOpenFiles.contains(fileExt))
		{
			//使用关联程序打开
			QUrl fileUrl = QUrl::fromLocalFile(filePathName);
			if (QDesktopServices::openUrl(fileUrl))
			{
				return;
			}
		}

		//打开失败时，打开文件夹选定对应文件
		OpenAndSelectFile(filePathName);
	}
}

void IMMsgWidget::slot_rightClicked(ChatWidget::MenuType menuType, const UIMsgInfo &msgInfo, const QVariantMap &exParams)
{
	//不是正常显示的消息，则不显示菜单
	if (!msgInfo.isNormalMsg())
	{
		return;
	}
	CursorPosType cursorType = (CursorPosType)exParams["type"].toInt();
	QString cursorText = exParams["cursorText"].toString();

	QMenu m(this);
	if (menuType == ChatWidget::MENU_CONTENT)
	{
		QAction *act_copy = new QAction(tr("复制"), &m);
		QAction *act_checkMulti = new QAction(tr("多选"), &m);
		QAction *act_del = new QAction(tr("删除"), &m);
		act_del->setProperty("msgId", msgInfo.clientMsgID);
		QAction *act_clear = new QAction(tr("清空"), &m);
		QAction *act_saveAs = new QAction(tr("另存为..."), &m);
		QAction *act_openDir = new QAction(tr("在文件夹中显示"), &m);
		connect(act_copy, &QAction::triggered, this, &IMMsgWidget::slot_act_copyClicked);
		connect(act_checkMulti, &QAction::triggered, this, &IMMsgWidget::slot_act_checkMultiClicked);
		connect(act_del, &QAction::triggered, this, &IMMsgWidget::slot_act_delClicked);
		connect(act_clear, &QAction::triggered, this, &IMMsgWidget::slot_act_clearClicked);
		connect(act_saveAs, &QAction::triggered, this, &IMMsgWidget::slot_act_saveAsClicked);
		connect(act_openDir, &QAction::triggered, this, &IMMsgWidget::slot_act_openDirClicked);
		m.addAction(act_copy);
		// 	m.addAction(act_reply);
		// 	m.addAction(act_del);
		//	m.addAction(act_checkMulti);
		//	m.addAction(act_clear);

		if (this->getCurSelCount() <= 1)
		{
			QUrl u(cursorText);
			QString filePathName;
			if (u.isLocalFile())
			{
				filePathName = u.toLocalFile();
			}
			bool isFileExists = QFile::exists(filePathName);
			//如果是图片、文件类型并且存在，则需要显示另存为
			if (isFileExists)
			{
				if (cursorType == CURSOR_IMAGE)
				{
					act_saveAs->setProperty("fileName", filePathName);
					m.addAction(act_saveAs);
				}
				else if (cursorType == CURSOR_FILE)
				{
					act_saveAs->setProperty("fileName", filePathName);
					act_openDir->setProperty("fileName", filePathName);
					m.addAction(act_saveAs);
					m.addAction(act_openDir);
				}
			}
			else
			{
				if (cursorType == CURSOR_IMAGE)
				{
					m.removeAction(act_copy);
					downloadFile(msgInfo.clientMsgID);
				}
			}
		}
	}
	
	//子类自定义菜单
	onMenuExec(&m, menuType, msgInfo);
	if (!m.isEmpty())
	{
		m.exec(QCursor::pos());
	}
}

void IMMsgWidget::slot_act_copyClicked()
{
	this->copyCurSelect();
}

void IMMsgWidget::slot_act_checkMultiClicked()
{
	//多选
}

void IMMsgWidget::slot_act_delClicked()
{
	//删除
	QAction *act_del = static_cast<QAction*>(sender());
	if (act_del == NULL)
	{
		return;
	}

	QString msgId = act_del->property("msgId").toString();
	this->removeMsg(msgId);
}

void IMMsgWidget::slot_act_clearClicked()
{
	this->clear();
	clear();
	emit s_onMsgCleared();
}

void IMMsgWidget::slot_act_saveAsClicked()
{
	//图片另存为
	QAction *act_saveAs = static_cast<QAction*>(sender());
	if (act_saveAs == NULL)
	{
		return;
	}
	QString fileName = act_saveAs->property("fileName").toString();
	if (fileName.isEmpty())
	{
		return;
	}
	QFileInfo fi(fileName);
	QString saveFileNmae = QFileDialog::getSaveFileName(this, tr("另存为..."), QDir::rootPath() + fi.fileName(), fi.suffix());
	if (saveFileNmae.isEmpty())
	{
		return;
	}
	QFile::copy(fileName, saveFileNmae);
}

void IMMsgWidget::slot_act_openDirClicked()
{
	QAction *act_openDir = static_cast<QAction*>(sender());
	if (act_openDir == NULL)
	{
		return;
	}
	QString fileName = act_openDir->property("fileName").toString();
	if (fileName.isEmpty())
	{
		return;
	}
	QFileInfo fi(fileName);
	OpenAndSelectFile(fileName);
}
