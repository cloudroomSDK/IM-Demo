#include "stdafx.h"
#include "ImIconMgr.h"

#define USER_FACE_DIR "UserFace"
const int DownloadCountAtSameTime = 2;	//同时下载的数量

const int ListItem_IconID = QListWidgetItem::UserType + 1200;
//////////////////////////////////////////////////////////////////////////
ListWidgetItemWithIMIcon::~ListWidgetItemWithIMIcon()
{
	//qDebug("~ListWidgetItemWithIMIcon:%p", this);
	ImIconMgr::instance()->unbindIcon(this);
}

ImIconMgr::ImIconMgr()
{
}

ImIconMgr::~ImIconMgr()
{

}

ImIconMgr *ImIconMgr::instance()
{
	static ImIconMgr ins;
	return &ins;
}

void ImIconMgr::init(const QString &acntDatPath)
{
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_selfInfoUpdated, this, &ImIconMgr::slot_selfInfoUpdated, Qt::UniqueConnection);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_friendInfoChanged2, this, &ImIconMgr::slot_friendInfoChanged, Qt::UniqueConnection);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpInfoChanged2, this, &ImIconMgr::slot_grpInfoChanged, Qt::UniqueConnection);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberInfoChanged2, this, &ImIconMgr::slot_grpMemberInfoChanged, Qt::UniqueConnection);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberDeleted, this, &ImIconMgr::slot_grpMemberDeleted, Qt::UniqueConnection);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_grpMemberAdded, this, &ImIconMgr::slot_grpMemberAdded, Qt::UniqueConnection);
	connect(CRIMManager_Qt::instance(), &CRIMManager_Qt::s_syncServerFinish, this, &ImIconMgr::slot_syncServerFinish, Qt::UniqueConnection);

	connect(&_qNetworkMgr, &QNetworkAccessManager::finished, this, &ImIconMgr::slot_networkRequestFinished, Qt::UniqueConnection);
	_faceImgPath = AddBackslashAtend(acntDatPath) + AddBackslashAtend(USER_FACE_DIR);
	//创建目录
	QDir qdir;
	qdir.mkpath(_faceImgPath);
}

void ImIconMgr::uninit()
{
	_downloadUrls.clear();
	_downloadings.clear();
	_bindInfs.clear();
	_pixCach.clear();
}

QPixmap ImIconMgr::getIconFromCach(const QString& id, const QSize &sz)
{
	auto cachPos = _pixCach.find(id);
	if (cachPos != _pixCach.end())
	{
		QMap<int, QPixmap> &szIcons = *cachPos;
		auto szPos = szIcons.find(QSizeHashKey(sz));
		if (szPos != szIcons.end())
		{
			return szPos.value();
		}
	}
	return QPixmap();
}

QPixmap ImIconMgr::getIconFor(const IMIconInfo &info)
{
	if (info._type == IMICON_GRP)
	{
		return getGrpIcon_inner(info._id, info._url, info._sz, info._bkColr);
	}
	else
	{
		return getUserIcon_inner(info._id, info._url, info._name, info._sz, info._bkColr);
	}
}

//更新图标缓存
void ImIconMgr::updateIconCache(const QString& id)
{
	//先清理缓存
	_pixCach.remove(id);

	//遍历bind对象，进行图像更新
	auto pos = _bindInfs.find(id);
	if (pos == _bindInfs.end())
		return;
	QMap<void*, BindInfo> &bInfos = *pos;
	for (BindInfo &inf : bInfos)
	{
		if (inf._func)
		{
			QPixmap pix = getIconFor(inf._iconInfo);
			inf._func(pix);
	}
}
}

void ImIconMgr::updateIconInfo(const QString& id, const QString &url, const QString &name)
{
	//先清理缓存
	_pixCach.remove(id);

	//遍历bind对象，进行图像更新
	auto pos = _bindInfs.find(id);
	if (pos == _bindInfs.end())
		return;
	QMap<void*, BindInfo> &bInfos = *pos;
	for (BindInfo &inf : bInfos)
	{
		inf._iconInfo._url = url;
		inf._iconInfo._name = name;
		if (inf._func)
		{
			QPixmap pix = getIconFor(inf._iconInfo);
			inf._func(pix);
		}
	}
}

QPixmap ImIconMgr::getUserIcon_inner(const QString &userID, const QString &url, const QString &name, const QSize& sz, const QColor &bkColor)
{
	//从缓存获取
	QPixmap pix = getIconFromCach(userID, sz);
	if (!pix.isNull())
	{
		return pix;
	}

	//从url获取
	if (pix.isNull() && !url.isEmpty())
	{
		pix = getIconFromUrl(url, sz, UrlCookie(userID, ""));
	}
	//从个人信息中获取
	if (pix.isNull())
	{
		const CRIMUserInfo* pSelfInfo = &(CRIMManager_Qt::instance()->getSelfUserInfo());
		if (pSelfInfo != nullptr && pSelfInfo->userID == userID)
		{
			pix = getIMUserIcon(pSelfInfo, sz, bkColor);
		}
	}
	//从好友信息中获取
	if (pix.isNull())
	{
		const CRIMFriendInfo *pInfo = CRIMManager_Qt::instance()->getFriendInfo(userID);
		if (pInfo != nullptr)
		{
			pix = getIMUserIcon(pInfo, sz, bkColor);
		}
	}
	//从群成员信息中获取
	if (pix.isNull())
	{
		const CRIMGrpMemberInfo *pInfo = CRIMManager_Qt::instance()->getGrpMemberInfoFromAll(userID);
		if (pInfo != nullptr)
		{
			pix = getIMUserIcon(pInfo, sz, bkColor);
		}
	}
	//生成昵称的图片
	if (pix.isNull() && !name.isEmpty())
	{
		pix = makeIconFor(name, sz, bkColor);
	}
	//默认图片
	if (pix.isNull())
	{
		pix = svgRes2Pixmap(":/Res/contact.svg", sz);
	}
	if (pix.size() != sz)
	{
		pix = pix.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}
	makeIconRoundBound(pix);

	//缓存图像
	if (!userID.isEmpty())
	{
		_pixCach[userID][QSizeHashKey(sz)] = pix;
	}
	return pix;
}

QPixmap ImIconMgr::getIMUserIcon(const CRIMPublicUserInfo *pInfo, const QSize& sz, const QColor &bkColor)
{
	return makeIconFor(pInfo->faceURL, pInfo->getDisplayName(), sz, UrlCookie(pInfo->userID, ""), bkColor);
}

QPixmap ImIconMgr::getGrpIcon_inner(const QString &grpID, const QString &url, const QSize& sz, const QColor &bkColor)
{
	//从缓存获取
	QPixmap pix = getIconFromCach(grpID, sz);
	if (!pix.isNull())
	{
		return pix;
	}

	//从url获取
	if (pix.isNull() && !url.isEmpty())
	{
		pix = getIconFromUrl(url, sz, UrlCookie("", grpID));
	}
	if (pix.isNull())
	{
		const CRIMGrpInfo_Ex *pInfo = CRIMManager_Qt::instance()->getJoinedGrpInfo(grpID);
		if (pInfo != nullptr)
		{
			pix = getIconFromUrl(pInfo->faceURL, sz, UrlCookie("", grpID));
			if (pix.isNull())
			{
				//通过群成员图标生成
				pix = makeIconFor(pInfo->_first4Members, sz, bkColor);
			}
		}
	}
	//默认图片
	if (pix.isNull())
	{
		pix = svgRes2Pixmap(":/Res/group.svg", sz);
	}
	if (pix.size() != sz)
	{
		pix = pix.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}
	makeIconRoundBound(pix, sz);

	//缓存图像
	if (!grpID.isEmpty())
	{
		_pixCach[grpID][QSizeHashKey(sz)] = pix;
	}
	return pix;
}


QPixmap ImIconMgr::getIconFromUrl(const QString &url, const QSize &sz, const UrlCookie &cookie)
{
	QPixmap pix;
	if (!url.isEmpty() && url.startsWith("http", Qt::CaseInsensitive))
	{
		QFileInfo finf(url);
		QString suffix = finf.suffix();
		suffix = suffix.split('?').value(0);
		if (suffix.isEmpty())
		{
			suffix = "jpg";
		}

		QString locFileName = _faceImgPath + MakeMd5(url.toUtf8()) + "." + suffix;
		if (QFile::exists(locFileName))
		{
			QSize snapshotSize = QSize(128, 128);
			QPixmap snapshotPix = getIconFromCach(url, snapshotSize);
			if (snapshotPix.isNull())
			{
				QString locSnapshotFileName = _faceImgPath + MakeMd5(url.toUtf8()) + ".snapshot." + suffix;
				if (!QFile::exists(locSnapshotFileName))
				{
					QImage img = getImageFromFile(locFileName, snapshotSize, Qt::KeepAspectRatioByExpanding);
					img.save(locSnapshotFileName);
					snapshotPix = QPixmap::fromImage(img);
				}
				else
				{
					snapshotPix = QPixmap(locSnapshotFileName);
				}
				//尺寸不符合，取中间部分
				if (snapshotSize != snapshotPix.size())
				{
					snapshotPix = snapshotPix.copy((snapshotPix.size().width() - snapshotSize.width()) / 2, (snapshotPix.size().height() - snapshotSize.height()) / 2, snapshotSize.width(), snapshotSize.height());
				}
				_pixCach[url][QSizeHashKey(snapshotSize)] = snapshotPix;
			}

			//等比缩放到目标大小
			pix = snapshotPix.scaled(sz, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
			//取中间部份
			if (pix.size() != sz)
			{
				pix = pix.copy((pix.width() - sz.width()) / 2, (pix.height() - sz.height()) / 2, sz.width(), sz.height());
			}
		}
		else //下载url
		{
			//已经在下载中，不再次下载
			auto fItem = _downloadUrls.find(url);
			if (fItem != _downloadUrls.end())
			{
				DownloadDat &dat = *fItem;
				dat._cookies.insert(cookie);
			}
			else
			{
				DownloadDat &dat = _downloadUrls[url];
				dat._httpID = makeUUID();
				dat._locFileName = locFileName;
				dat._cookies.insert(cookie);
				downloadNext();
			}
		}
	}
	return pix;
}

QPixmap ImIconMgr::makeIconFor(const QString &name, const QSize& sz, const QColor &bkColor)
{
	//生成icon
	QPixmap pix = QPixmap(sz);

	QPainter painter(&pix);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

	//画背景
	QRect rc(0, 0, pix.width(), pix.height());
	QBrush fillBrush(bkColor);
	painter.fillRect(rc, fillBrush);

	QColor fntColr(250, 250, 250);
	painter.setPen(QPen(fntColr));
	QString chr = name.left(1).toUpper();

	//准备字体
	QFont fnt = QApplication::font();
	fnt.setPixelSize(int(sz.height() * 3 / 5.0 + 0.5));
	painter.setFont(fnt);
	if (chr.at(0).unicode() > 256)
	{
		int offset = fnt.pixelSize() / 10;
		rc.adjust(0, -offset, 0, -offset);
	}
	painter.drawText(rc, Qt::AlignCenter, chr);
	return pix;
}


QPixmap ImIconMgr::makeIconFor(const QString &url, const QString &name, const QSize& sz, const UrlCookie &cookie, const QColor &bkColor)
{
	QPixmap pix = getIconFromUrl(url, sz, cookie);
	if (pix.isNull())
	{
		pix = makeIconFor(name, sz, bkColor);
	}
	return pix;
}


QPixmap ImIconMgr::makeIconFor(const QList<CRIMGrpMemberInfo*> &lst, const QSize& sz, const QColor &bkColor)
{
	if (lst.size() < 1)
	{
		return QPixmap();
	}
	if (lst.size() == 1)
	{
		CRIMGrpMemberInfo* pInfo = lst[0];
		return makeIconFor(pInfo->faceURL, pInfo->nickname, sz, UrlCookie(pInfo->userID, pInfo->groupID), bkColor);
	}

 	//生成icon
 	QPixmap iconPix(sz);
	iconPix.fill(Qt::transparent);

	//painter
	QPainter painter(&iconPix);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

	QRect rc(0, 0, iconPix.width(), iconPix.height());
	const int space = 1;
	QSize leftTopSize((sz.width() + 1 - space) / 2, (sz.height() + 1 - space) / 2);
	QSize rightBottomSize = rc.size() - leftTopSize - QSize(space, space);

	QList<QRect> dstRts;
	dstRts.push_back(QRect(0, 0, leftTopSize.width(), leftTopSize.height())); //左上
	dstRts.push_back(QRect(0, leftTopSize.height() + space, leftTopSize.width(), rightBottomSize.height())); //左下
	dstRts.push_back(QRect(leftTopSize.width() + space, 0, rightBottomSize.width(), leftTopSize.height())); //右上
	dstRts.push_back(QRect(leftTopSize.width() + space, leftTopSize.height() + space, rightBottomSize.width(), rightBottomSize.height())); //右下

	if (lst.size() <= 3) //左侧合并
	{
		//绘制到目标左边竖条
		dstRts[0].setBottom(dstRts[1].bottom());
		dstRts.removeAt(1);
	}
	if (lst.size() == 2) //右侧合并
	{
		//绘制到目标左边竖条
		dstRts[1].setBottom(dstRts[2].bottom());
		dstRts.removeAt(2);
	}

	QList<QPixmap> pics;
	for (int i = 0; i < lst.size(); i++)
	{
		CRIMGrpMemberInfo* pInfo = lst[i];
		pics.push_back(makeIconFor(pInfo->faceURL, pInfo->nickname, dstRts[i].size(), UrlCookie(pInfo->userID, pInfo->groupID), bkColor));
	}

	assert(pics.size() == dstRts.size());
	for (int i = 0; i < pics.size(); i++)
	{
		painter.drawPixmap(dstRts[i].topLeft(), pics[i]);
	}
	return iconPix;
}

void ImIconMgr::makeIconRoundBound(QPixmap &pix, const QSize &contentSz)
{
	if (pix.isNull())
		return;

	QPixmap rslt(pix.size());
	rslt.fill(QColor(Qt::transparent));

	QPainter painter(&rslt);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);

 	//配置裁剪区域
	QRectF rt(0.0, 0.0, pix.width(), pix.height());
	if (!contentSz.isEmpty())
	{
		rt.setSize(contentSz);
	}
	QPainterPath roundPath;
	roundPath.addRoundedRect(rt, pix.width() / 6.0, pix.height() / 6.0);
	painter.setClipPath(roundPath);

	//填充边缘为透明色
	painter.drawPixmap(0, 0, pix);
	rslt.swap(pix);
	return;
}

void ImIconMgr::downloadNext()
{
	for (auto pos = _downloadUrls.begin(); pos != _downloadUrls.end(); pos++)
	{
		if (_downloadings.size() >= DownloadCountAtSameTime)
			return;
		if (_downloadings.contains(pos.key()))
			return;

		QString strUrl = pos.key();
		_downloadings.insert(strUrl);
		MainLogDebug("download face img: %s", strUrl.toUtf8().constData());


		QUrl qurl(strUrl);
		QNetworkRequest qreq(qurl);
		qreq.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
		cfgQNetworkRequestForHttps(&qreq, App::instance()->getCfgInfo().skipVerifyCert);

		QNetworkReply *rsp = _qNetworkMgr.get(qreq);
		rsp->setProperty("strUrl", strUrl);
	}
}


void ImIconMgr::slot_networkRequestFinished(QNetworkReply *reply)
{
	if (reply == nullptr)
		return;
	reply->deleteLater();

	QString strUrl = reply->property("strUrl").toString();
	auto fItem = _downloadUrls.find(strUrl);
	if (fItem == _downloadUrls.end())
		return;

	//保存数据
	DownloadDat dat = fItem.value();
	//清理下载记录
	_downloadings.remove(fItem.key());
	_downloadUrls.erase(fItem);
	//下载其它
	downloadNext();

	//下载失败
	QNetworkReply::NetworkError netErr = reply->error();
	int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if ((netErr != QNetworkReply::NoError) || (200 != httpCode))
	{
		MainLogWarn("download face img failed, netErr:%d, httpCode:%d, url:%s", netErr, httpCode, strUrl.toUtf8().constData());
		return;
	}

	//保存文件
	QByteArray replyDat = reply->readAll();
	if (!WriteDataToFile(replyDat, dat._locFileName))
	{
		MainLogWarn("download face img success, size:%d, but save failed, url:%s, file:%s", replyDat.size(), strUrl.toUtf8().constData(), dat._locFileName.toUtf8().constData());
		return;
	}

	MainLogDebug("download face img success, url:%s", strUrl.toUtf8().constData());
	for (auto cookie : dat._cookies)
	{
		if (cookie._userId.isEmpty() && cookie._groupId.isEmpty())
		{
			continue;
		}
		//如果有群id、用户id，则可能群成员url下载完成，需要更新群图标缓存
		if (!cookie._userId.isEmpty() && !cookie._groupId.isEmpty())
		{
			updateIconCache(cookie._groupId);
		}
		else
		{
			updateIconCache(!cookie._userId.isEmpty() ? cookie._userId : cookie._groupId);
		}
	}
}

void ImIconMgr::slot_selfInfoUpdated(const CRIMUserInfo& oldInfo, const CRIMUserInfo &newInfo)
{
	if (oldInfo.faceURL == newInfo.faceURL
		&& oldInfo.getDisplayName() == newInfo.getDisplayName()
		)
	{
		return;
	}
	updateIconInfo(newInfo.userID, newInfo.faceURL, newInfo.getDisplayName());
}

void ImIconMgr::slot_friendInfoChanged(const CRIMFriendInfo &oldInfo, const CRIMFriendInfo &newInfo)
{
	if (oldInfo.faceURL == newInfo.faceURL
		&& oldInfo.getDisplayName() == newInfo.getDisplayName()
		)
	{
		return;
	}

	updateIconInfo(newInfo.userID, newInfo.faceURL, newInfo.getDisplayName());
}

void ImIconMgr::slot_grpInfoChanged(const CRIMGrpInfo &oldInfo, const CRIMGrpInfo &newInfo)
{
	if (oldInfo.faceURL == newInfo.faceURL)
	{
		return;
	}

	updateIconInfo(newInfo.groupID, newInfo.faceURL, "");
}

void ImIconMgr::slot_grpMemberInfoChanged(const CRIMGrpMemberInfo &oldInfo, const CRIMGrpMemberInfo &newInfo)
{
	if (oldInfo.faceURL == newInfo.faceURL
		&& oldInfo.nickname == newInfo.nickname
		)
	{
		return;
	}

	const CRIMGrpInfo_Ex *pGrp = CRIMManager_Qt::instance()->getJoinedGrpInfo(newInfo.groupID);
	if (pGrp == nullptr)
	{
		return;
	}
	if (!pGrp->isFirst4Member(newInfo.userID))
	{
		return;
	}
	//群成员信息变化，需要更新群图标的缓存
	updateIconCache(newInfo.groupID);
}

void ImIconMgr::slot_grpMemberDeleted(const CRIMGrpMemberInfo &info)
{
	const CRIMGrpInfo_Ex *pGrp = CRIMManager_Qt::instance()->getJoinedGrpInfo(info.groupID);
	if (pGrp == nullptr)
	{
		return;
	}
	if (!pGrp->isFirst4Member(info.userID))
	{
		return;
	}
	//群成员信息变化，需要更新群图标的缓存
	updateIconCache(info.groupID);
}

void ImIconMgr::slot_grpMemberAdded(const CRIMGrpMemberInfo &info)
{
	const CRIMGrpInfo_Ex *pGrp = CRIMManager_Qt::instance()->getJoinedGrpInfo(info.groupID);
	if (pGrp == nullptr)
	{
		return;
	}
	if (!pGrp->isFirst4Member(info.userID))
	{
		return;
	}

	//群成员信息变化，需要更新群图标的缓存
	updateIconCache(info.groupID);
}


void ImIconMgr::slot_syncServerFinish()
{
	//同步完成需要更新所有缓存
	QStringList ids = _pixCach.keys();
	for (auto id : ids)
	{
		updateIconCache(id);
	}
}

void ImIconMgr::bindIcon(QObject *p, const IMIconInfo &iconInfo, const std::function<void(const QPixmap &pix)> &upImgFunc)
{
	QString oldID = p->property("_ImIconMgr_id").toString();
	if (!oldID.isEmpty())
	{
		//已经绑定过，需要清理缓存
		_pixCach.remove(oldID);
	}
	unbindIcon(p);

	p->setProperty("_ImIconMgr_id", iconInfo._id);
	connect(p, &QObject::destroyed, this, &ImIconMgr::slot_qobjDestroyed, Qt::UniqueConnection);

	QMap<void*, BindInfo>& infs = _bindInfs[iconInfo._id];
	BindInfo &bInfo = infs[(void*)p];
	bInfo._type = BindQObj;
	bInfo._iconInfo = iconInfo;
	bInfo._func = upImgFunc;

	upImgFunc(getIconFor(iconInfo));
}

void ImIconMgr::unbindIcon(QObject *p)
{
	QString id = p->property("_ImIconMgr_id").toString();
	if (id.isEmpty())
	{
		return;
	}
	QMap<void*, BindInfo>& infs = _bindInfs[id];
	infs.remove((void*)p);
}

void ImIconMgr::bindIcon(ListWidgetItemWithIMIcon *p, const IMIconInfo &iconInfo)
{
	//qDebug("bindIcon, id:%s, ptr:%p", iconInfo._id.toUtf8().constData(), p);
	QString oldID = p->data(ListItem_IconID).toString();
	if (!oldID.isEmpty())
	{
		//已经绑定过，需要清理缓存
		_pixCach.remove(oldID);
	}
	unbindIcon(p);

	p->setData(ListItem_IconID, iconInfo._id);
	p->setIcon(getIconFor(iconInfo));

	QMap<void*, BindInfo>& infs = _bindInfs[iconInfo._id];
	BindInfo &bInfo = infs[(void*)p];
	bInfo._type = BindListItem;
	bInfo._iconInfo = iconInfo;
	bInfo._func = [p](const QPixmap &pix){
		p->setIcon(QIcon(pix));
	};
}

void ImIconMgr::unbindIcon(ListWidgetItemWithIMIcon *p)
{
	//qDebug("unbindIcon, ptr:%p", p);
	QString id = p->data(ListItem_IconID).toString();
	if (id.isEmpty())
	{
		return;
	}
	QMap<void*, BindInfo>& infs = _bindInfs[id];
	infs.remove((void*)p);
}

void ImIconMgr::slot_qobjDestroyed(QObject *pObj)
{
	unbindIcon(pObj);
}

