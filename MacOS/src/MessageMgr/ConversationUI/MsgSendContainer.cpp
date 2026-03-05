#include "stdafx.h"
#include "MsgSendContainer.h"
#include "CRIM_Manager.h"
#include "IMFaceWidget.h"
#include "ConversationBaseWidget.h"
#include "DlgFastSelMember.h"
#include "GroupMemberList.h"
#include "FaceHelper.h"

#define PROPERTY_USERID (DocumentTextObject::UserPropertyBegin + 1)
int compareNumberStr_mgr(QString a, QString b)
{
	bool nOK, mOK;
	int n = a.toInt(&nOK);
	int m = b.toInt(&mOK);
	if (nOK && mOK) return n - m;
	return a.compare(b);
}
bool compareFileName_mgr(const QString &s1, const QString &s2)
{
	QString a = s1.left(s1.lastIndexOf("."));
	QString b = s2.left(s2.lastIndexOf("."));
	QStringList l1 = a.split("-");
	QStringList l2 = b.split("-");
	int r1 = compareNumberStr_mgr(l1[0], l2[0]);
	if (r1 != 0) return r1 < 0;
	QString p1 = (l1.count() > 1 ? l1[1] : "");
	QString p2 = (l2.count() > 1 ? l2[1] : "");
	return compareNumberStr_mgr(p1, p2) < 0;
}

MsgSendContainer::MsgSendContainer(QWidget *parent): QWidget(parent)
{
	ui.setupUi(this);

	m_baseWidget = NULL;
	m_memberList = NULL;

	m_dlgFastSelMember = new DlgFastSelMember(ui.msgSendWidget, this);
	connect(m_dlgFastSelMember, &DlgFastSelMember::s_selFinished, this, &MsgSendContainer::slot_fastSelFinished);
	connect(ui.btnFace, &QAbstractButton::toggled, this, &MsgSendContainer::slot_btnFaceClicked);
	connect(ui.btnOpenPic, &QAbstractButton::clicked, this, &MsgSendContainer::slot_btnOpenPicClicked);
	connect(ui.btnOpenFile, &QAbstractButton::clicked, this, &MsgSendContainer::slot_btnOpenFileClicked);
	connect(ui.btnSendMsg, &QAbstractButton::clicked, this, &MsgSendContainer::slot_btnSendMsgClicked);
	connect(ui.msgSendWidget, &QTextEdit::textChanged, this, &MsgSendContainer::slot_sendEditTextChanged);
	ui.horizontalLayout_2->setContentsMargins(9, 3, 9, 3);
	ui.horizontalLayout_2->setSpacing(10);
	ui.horizontalLayout_3->setContentsMargins(12, 12, 12, 12);
	ui.stackedWidget->setCurrentWidget(ui.sendPage);
	//ui.sendEdit->setPlaceholderText(tr("Please input text"));
	ui.msgSendWidget->setSkipTab(true);
	ui.msgSendWidget->document()->setDocumentMargin(5);
	ui.msgSendWidget->installEventFilter(this);

	slot_sendEditTextChanged();
}

MsgSendContainer::~MsgSendContainer()
{

}

void MsgSendContainer::init(ConversationBaseWidget *w, GroupMemberList *memberList)
{
	m_baseWidget = w;
	if (m_memberList != NULL)
	{
		disconnect(m_memberList, &GroupMemberList::s_memberChanged, this, &MsgSendContainer::slot_memberChanged);
	}
	m_memberList = memberList;
	if (m_memberList != NULL)
	{
		connect(m_memberList, &GroupMemberList::s_memberChanged, this, &MsgSendContainer::slot_memberChanged);
	}

	clear();
	QTimer::singleShot(1, ui.msgSendWidget, SLOT(setFocus()));
	//ui.msgSendWidget->setFocus();
}

void MsgSendContainer::uninit()
{

}

QList<UIMsgInfo> MsgSendContainer::getCurrentMsg()
{
	return parseMsgFromSendedit();
}

void MsgSendContainer::insertText(const QString &txt)
{
	if (!ui.msgSendWidget->isEnabled())
	{
		return;
	}
	ui.msgSendWidget->insertPlainText(txt);
}

void MsgSendContainer::insertHtmlText(const QString &htmlTxt)
{
	if (!ui.msgSendWidget->isEnabled())
	{
		return;
	}
	ui.msgSendWidget->insertHtml(htmlTxt);
	ui.msgSendWidget->scaledResources();
}

void MsgSendContainer::insertAtText(const QString &atUserId)
{
	if (m_baseWidget->getType() != CRIM_Conversation_Group)
	{
		return;
	}
	if (!ui.msgSendWidget->isEnabled())
	{
		return;
	}
	QString atTxt, toolTip;
	if (atUserId == CRIMManager_Qt::instance()->getAtAllTag())
	{
		atTxt = QString("@%1").arg(tr("所有人"));
		toolTip = tr("所有人");
	}
	else
	{
		const CRIMGrpMemberInfo* groupMember = CRIMManager_Qt::instance()->getGrpMemberInfo(m_baseWidget->getID(), atUserId);
		atTxt = QString("@%1").arg(groupMember->nickname);
		toolTip = QString("%1(%2)").arg(groupMember->nickname).arg(groupMember->userID);
	}
	QTextCursor cursor = ui.msgSendWidget->textCursor();
	QTextCharFormat textCharFormat;
	textCharFormat.setObjectType(DocumentTextObject::getObjectType());
	textCharFormat.setToolTip(toolTip);
	textCharFormat.setProperty(PROPERTY_USERID, atUserId);
	textCharFormat.setProperty(DocumentTextObject::UUID, QUuid::createUuid().toString());
	textCharFormat.setProperty(DocumentTextObject::Text, atTxt);
	textCharFormat.setProperty(DocumentTextObject::Color, QColor(35, 132, 200));
	cursor.insertText(QString(QChar::ObjectReplacementCharacter), textCharFormat);
	cursor.insertText(" ");
}

bool MsgSendContainer::checkMessageExceedingLimitSz(const QList<UIMsgInfo> &lst)
{
	bool bExceedingLimitSz = true;
	while (true)
	{
		if (lst.size() > 10)
		{
			break;
		}

		for (auto &msg : lst)
		{
			QByteArray utf8Content = msg.getOrgContent().toUtf8();
			if (utf8Content.length() > CRIM_MAX_MSGLEN)
			{
				break;
			}
		}

		bExceedingLimitSz = false;
		break;
	}

	if (bExceedingLimitSz)
	{
		CRMsgBox::information(this, tr("提示"), tr("消息内容超长，请分条发送。"));
		return false;
	}
	return true;
}

QTextDocumentFragment MsgSendContainer::getTextFragment()
{
	return QTextDocumentFragment(ui.msgSendWidget->document());
}

void MsgSendContainer::setTextFragment(const QTextDocumentFragment &doc)
{
	ui.msgSendWidget->clear();
	ui.msgSendWidget->textCursor().insertFragment(QTextDocumentFragment(doc));
	ui.msgSendWidget->scaledResources();
}

void MsgSendContainer::clear()
{
	ui.msgSendWidget->clear();
}

UIMsgInfo MsgSendContainer::createTextMsgStruct(const QString &text, QMap<QString, QString> &atUsersInfo)
{
	if (text.contains("@") && atUsersInfo.size() > 0)
	{
		QList<CRIMAtInfo> newAtUserInfoList;
		CRIMAtInfo info;
		for (auto it = atUsersInfo.begin(); it != atUsersInfo.end(); it++)
		{
			const QString &atUserId = it.key();
			const QString &atUserName = it.value();
			info.atUserID = atUserId;
			info.groupNickname = atUserName;
			newAtUserInfoList.append(info);
		}
		UIMsgInfo msg = CRIMManager_Qt::instance()->createTextAtMsg(text, atUsersInfo.keys(), newAtUserInfoList);
		atUsersInfo.clear();
		return msg;
	}
	else
	{
		UIMsgInfo msg = CRIMManager_Qt::instance()->createTextMsg(text);
		return msg;
	}
}

//解析消息，得到需要发送的数据
QList<UIMsgInfo> MsgSendContainer::parseMsgFromSendedit()
{
	QList<UIMsgInfo> rsltMsgs;
	QString sendText;
	QMap<QString, QString> atUsersInfo;
	QTextDocument *d = ui.msgSendWidget->document();
	QTextBlock currentBlock = d->begin();
	while (currentBlock.isValid())
	{
		for (QTextBlock::iterator it = currentBlock.begin(); !(it.atEnd()); ++it)
		{
			QTextFragment currentFragment = it.fragment();
			QTextCharFormat currentFormat = currentFragment.charFormat();
			ChatInputEdit::CharFormatType type = ui.msgSendWidget->getFormatType(currentFormat);
			if (type == ChatInputEdit::CFT_UNDEFINE)
				continue;

			if (type >= ChatInputEdit::CFT_IMG && type <= ChatInputEdit::CFT_FILE)
			{
				QTextImageFormat tf = currentFormat.toImageFormat();
				QString localFileName = tf.name();
				//读取文件要转换成本地文件
				QUrl u(tf.name());
				if (u.isLocalFile())
				{
					localFileName = u.toLocalFile();
				}
				if (type == ChatInputEdit::CFT_FACE)
				{
					sendText += ChatPublic::getFaceID(localFileName);
				}
				else//图片、文件，需要将之前的文本消息拆分成一条消息，图片、文件单独一条消息
				{
					//文本
					if (!sendText.isEmpty())
					{
						UIMsgInfo msgInfo = createTextMsgStruct(sendText, atUsersInfo);
						rsltMsgs.append(msgInfo);
						sendText.clear();
					}

					UIMsgInfo fileMsgInfo;
					if (type == ChatInputEdit::CFT_IMG)
					{
						//图片
						fileMsgInfo = CRIMManager_Qt::instance()->createImageMsgFromFullPath(localFileName);
					}
					else if (type == ChatInputEdit::CFT_FILE)
					{
						QFileInfo fi(localFileName);
						QMimeDatabase db;
						QMimeType mime = db.mimeTypeForFile(fi);
						if (mime.name().startsWith("image/"))
						{
							//文件:图片类型
							fileMsgInfo = CRIMManager_Qt::instance()->createImageMsgFromFullPath(localFileName);
						}
// 						else if (mime.name().startsWith("video/"))
// 						{
// 							//文件:视频类型
// 							fileMsgInfo = CRIMManager_Qt::instance()->createVideoMessageFromFullPath(localFileName);
// 						}
 						else
 						{
							//文件
							fileMsgInfo = CRIMManager_Qt::instance()->createFileMsgFromFullPath(fi.absoluteFilePath(), fi.fileName());
						}
					}
					rsltMsgs.append(fileMsgInfo);
					sendText.clear();
				}
				continue;
			}
			else if (type == ChatInputEdit::CFT_CUSTOM_TEXT)//at文本处理
			{
				QString atUserId = currentFormat.property(PROPERTY_USERID).toString();
				QString atUserName = currentFormat.property(DocumentTextObject::Text).toString().remove(0,1);
				sendText += g_atTextFormat.arg(atUserId);
				atUsersInfo[atUserId] = atUserName;
			}
			else if (type == ChatInputEdit::CFT_TEXT)//文本处理
			{
				sendText += currentFragment.text();
			}
		}

		currentBlock = currentBlock.next();
		if (currentBlock.isValid())
		{
			sendText += "\n";
		}
	}
	if (!sendText.isEmpty())
	{
		//文本
		UIMsgInfo msgInfo = createTextMsgStruct(sendText, atUsersInfo);
		rsltMsgs.append(msgInfo);
		sendText.clear();
	}
	return rsltMsgs;
}

void MsgSendContainer::slot_btnSendMsgClicked()
{
// 	if (!ui.msgSendWidget->isEnabled())
// 	{
// 		return;
// 	}
	if (ui.msgSendWidget->document()->isEmpty())
	{
		CRMsgBox::information(this, tr("提示"), tr("不能发送空消息"));
		return;
	}

	QList<UIMsgInfo> sendPramas = getCurrentMsg();
	if (!checkMessageExceedingLimitSz(sendPramas))
	{
		return;
	}

	emit s_sendMsg(sendPramas);
	ui.msgSendWidget->clear();
	ui.msgSendWidget->setFocus();
}


void MsgSendContainer::slot_btnFaceClicked(bool bChecked)
{
	if (bChecked)
	{
		IMFaceWidget::ImgDef imgDef;
		imgDef.iconSize = QSize(28, 28);

		QString strPath = ":/Res/IM/Face/";
		QDir dir(strPath);
		if (dir.exists())
		{
			QStringList iconFiles = dir.entryList();
			qSort(iconFiles.begin(), iconFiles.end(), compareFileName_mgr);

			QStringList::const_iterator it;
			for (it = iconFiles.constBegin(); it != iconFiles.constEnd(); ++it)
			{
				imgDef.imgs.push_back(strPath + *it);
			}
		}

		IMFaceWidget *facePan = new IMFaceWidget(this, ui.btnFace, imgDef);
		connect(facePan, &IMFaceWidget::s_faceSelSig, this, &MsgSendContainer::slot_faceSel);
	}
}

void MsgSendContainer::slot_faceSel(const QString& strName)
{
	if (!ui.msgSendWidget->isEnabled())
	{
		return;
	}
	ui.msgSendWidget->insertImage(strName);
}

void MsgSendContainer::slot_btnOpenPicClicked()
{
	QString filters = tr("图片 (*.bmp;*.jpg;*.jpeg;*.png)");
	QStringList openPicNmaes = QFileDialog::getOpenFileNames(this, tr("打开"), ""/*QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)*/, filters);
	if ( openPicNmaes.size()>10 )
	{
		CRMsgBox::information(this, tr("提示"), tr("选择的图片不能超过10张！"));
		return;
	}

	for (auto &picName : openPicNmaes)
	{
		//jpg的transformation处理
		if (picName.endsWith(".jpg", Qt::CaseInsensitive) || picName.endsWith(".jpeg", Qt::CaseInsensitive))
		{
			QImageReader reader(picName, "JPG");
			if (reader.transformation() != QImageIOHandler::TransformationNone)
			{
				reader.setAutoTransform(true);
				QImage img = reader.read();
				picName = ChatPublic::makeANewPicName();
				img.save(picName, "JPG", 95);
			}
		}
		//bmp以jpg发送
		else if (picName.endsWith(".bmp", Qt::CaseInsensitive))
		{
			QImage img(picName);
			picName = ChatPublic::makeANewPicName();
			img.save(picName, "JPG", 95);
		}
		ui.msgSendWidget->insertImage(picName);
	}
}

void MsgSendContainer::slot_btnOpenFileClicked()
{
	QString filters = tr("所有文件 (*.*)");
	QStringList openFileNmaes = QFileDialog::getOpenFileNames(this, tr("打开"), ""/*QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)*/, filters);
	if (openFileNmaes.size() > 10)
	{
		CRMsgBox::information(this, tr("提示"), tr("选择的文件不能超过10个！"));
		return;
	}
	for (auto &fileName : openFileNmaes)
	{
		ui.msgSendWidget->insertFile(fileName);
	}
}

bool MsgSendContainer::eventFilter(QObject *watched, QEvent *event)
{
	if ((watched == ui.msgSendWidget) && (event->type() == QEvent::KeyPress))
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if (m_dlgFastSelMember->isVisible())
		{
			if (Qt::Key_Return == keyEvent->key() || Qt::Key_Enter == keyEvent->key() || Qt::Key_Escape == keyEvent->key())
			{
				QApplication::sendEvent(m_dlgFastSelMember, keyEvent);
				return true;
			}
		}
		bool bEnterPressed = ((Qt::Key_Return == keyEvent->key()) || (Qt::Key_Enter == keyEvent->key()));
		bool bCtrlEnterPressed = (keyEvent->modifiers() & Qt::ControlModifier) && bEnterPressed;
		bool isCtrlEnter =/* ui.menu_ctrl_enter->isChecked()*/false;
		if (isCtrlEnter)//按Ctrl+Enter发送
		{
			if (bCtrlEnterPressed)
			{
				this->slot_btnSendMsgClicked();
				return true;
			}
			else if (bEnterPressed)
			{
				ui.msgSendWidget->insertPlainText("\n");
			}
		}
		else//按Enter发送
		{
			if (bCtrlEnterPressed)
			{
				ui.msgSendWidget->insertPlainText("\n");
			}
			else if (bEnterPressed)
			{
				this->slot_btnSendMsgClicked();
				return true;
			}
		}
	}
	return QWidget::eventFilter(watched, event);
}

void MsgSendContainer::slot_sendEditTextChanged()
{
	ui.btnSendMsg->setEnabled(!ui.msgSendWidget->toPlainText().isEmpty());
	//群聊@
	if (m_baseWidget != NULL && m_baseWidget->getType() == CRIM_Conversation_Group)
	{
		QString atTxt;
		QTextCursor cursor = ui.msgSendWidget->textCursor();
		QTextCursor tc_at = ui.msgSendWidget->document()->find("@", cursor.position(), QTextDocument::FindBackward);
		if (tc_at.position() != -1)
		{
			cursor.setPosition(tc_at.position() - 1, QTextCursor::KeepAnchor);
			atTxt = cursor.selectedText();
		}
		QRect rt = ui.msgSendWidget->cursorRect();

		QPoint cursorPos = ui.msgSendWidget->mapToGlobal(rt.topLeft());
		if (atTxt.contains("@"))
		{
			QString findStr = atTxt;
			findStr = findStr.remove(0, 1);
			m_curAtCursor = cursor;
			m_dlgFastSelMember->showFindWnd(findStr, cursorPos);
		}
		else
		{
			m_dlgFastSelMember->close();
		}
	}
}

//输入框区域选择一个成员@
void MsgSendContainer::slot_fastSelFinished(const QString &str)
{
	if (m_memberList == NULL)
	{
		return;
	}
	QString userID;
	if (str == tr("所有人"))
	{
		userID = CRIMManager_Qt::instance()->getAtAllTag();
	}
	else
	{
		userID = m_memberList->getUserIDByNicknmae(str);
	}
	if (userID.isEmpty())
	{
		return;
	}
	m_curAtCursor.removeSelectedText();
	this->insertAtText(userID);
}

void MsgSendContainer::slot_memberChanged(int changeType, const QString &userId, const QString &newName, const QString &oldName)
{
	//更新搜索控件中保存的数据
	if (changeType == 0)//列表被清空
	{
		m_dlgFastSelMember->clear();
		const CRIMGrpMemberInfo* groupMember = CRIMManager_Qt::instance()->getGrpMemberInfo(m_baseWidget->getID(), CRIMManager_Qt::instance()->getUserID());
		if (groupMember != NULL && groupMember->isManager())
		{
			m_dlgFastSelMember->insertName(tr("所有人"));
		}
	}
	else if (changeType == 1)//列表成员被添加一个
	{
		//不能@自已
		if (userId == CRIMManager_Qt::instance()->getUserID())
			return;

		m_dlgFastSelMember->insertName(newName);
	}
	else if (changeType == 2)//列表成员被删除一个
	{
		m_dlgFastSelMember->removeName(newName);
	}
	else if (changeType == 3)//列表成员更新
	{
		m_dlgFastSelMember->removeName(newName);
		m_dlgFastSelMember->insertName(newName);
	}
}

void MsgSendContainer::insertAtMsg(const UIMsgInfo &msg)
{
	if (msg.contentType != CRIM_MsgType_At)
	{
		return;
	}
	const QString &strContent = msg.atTextElem.text;
	QString findAtStr = " @";
	int curIndex = 0;
	int findIndex = 0;
	do
	{
		int atStartIndex = strContent.indexOf(findAtStr, findIndex);
		if (atStartIndex < 0)
		{
			break;
		}
		atStartIndex += findAtStr.size();
		int atEndIndex = strContent.indexOf(" ", atStartIndex);
		if (atEndIndex < 0)
		{
			break;
		}
		QString atUserId = strContent.mid(atStartIndex, atEndIndex - atStartIndex);
		if (msg.atTextElem.atUserList.contains(atUserId))
		{
			QString text = strContent.mid(curIndex, atStartIndex - curIndex - findAtStr.size());
			if (!text.isEmpty())
			{
				insertText(text);
			}

			insertAtText(atUserId);
			curIndex = atEndIndex + 1;
		}
		findIndex = atEndIndex;
	} while (curIndex < strContent.length());

	if (curIndex < strContent.length())
	{
		QString text = strContent.mid(curIndex);
		insertText(text);
	}
}

void MsgSendContainer::slot_reEdit(const UIMsgInfo &msg)
{
	if (msg.contentType == CRIM_MsgType_Text)
	{
		insertHtmlText(msg.getDisplayContent());
	}
	else if (msg.contentType == CRIM_MsgType_At)
	{
		insertAtMsg(msg);
	}
}
