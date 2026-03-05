#ifndef IMICONMGR_H
#define IMICONMGR_H

#include <QHash>
const QColor g_IconBkColor(57, 129, 252);
#define DEFUALT_ICONSZ QSize(ScreenScale(40), ScreenScale(40))

class ListWidgetItemWithIMIcon : public QListWidgetItem
{
public:
	ListWidgetItemWithIMIcon(QListWidget *view = NULL) : QListWidgetItem(view) {}
	ListWidgetItemWithIMIcon(const QString &text, QListWidget *view = NULL) : QListWidgetItem(text, view) {}
	ListWidgetItemWithIMIcon(const QIcon &icon, const QString &text, QListWidget *view = NULL) : QListWidgetItem(icon, text, view) {}
	~ListWidgetItemWithIMIcon();
};

enum IMIconType { IMICON_UNKNOW = 0, IMICON_USR, IMICON_GRP };
struct IMIconInfo
{
	friend class ImIconMgr;
public:
	//用于确定有好友、群成员等关系链，不需要传入url信息，内部有维护url为最新
	IMIconInfo(const QString &id, IMIconType type = IMICON_USR, const QSize &sz = DEFUALT_ICONSZ) : _id(id), _type(type), _sz(sz), _bkColr(g_IconBkColor){}

	//用于不能确定是否有关系链，需要传入url信息，如果url与最新的url有冲突，则会出现异常
	IMIconInfo(const QString &id, const QString &url, const QString &name, IMIconType type = IMICON_USR, const QSize &sz = DEFUALT_ICONSZ, const QColor &bkColr = g_IconBkColor) : _id(id), _url(url), _name(name), _type(type), _sz(sz), _bkColr(bkColr){}


protected:
	IMIconInfo() : _type(IMICON_UNKNOW){}
	IMIconType	_type;
	QString		_id;
	QString		_url;
	QString		_name;
	QSize		_sz;
	QColor		_bkColr;

};

class ImIconMgr : public QObject
{
	Q_OBJECT

private:
	ImIconMgr();
	~ImIconMgr();

public:
	static ImIconMgr *instance();
	void init(const QString &acntDatPath);
	void uninit();

	//用于确定有好友、群成员等关系链，不需要传入url信息，内部有维护url为最新
	QPixmap getUserIcon(const QString &userID, const QSize& sz = DEFUALT_ICONSZ){
		return getIconFor(IMIconInfo(userID, IMICON_USR, sz));
	}
	QPixmap getGrpIcon(const QString &grpID, const QSize& sz = DEFUALT_ICONSZ){
		return getIconFor(IMIconInfo(grpID, IMICON_GRP, sz));
	}

	//用于不能确定是否有关系链，需要传入url信息，如果url与最新的url有冲突，则会出现异常
	QPixmap getUserIcon(const QString &userID, const QString &url, const QString &name, const QSize& sz = DEFUALT_ICONSZ, const QColor &bkColor = g_IconBkColor){
		IMIconInfo info(userID, url, name, IMICON_USR, sz, bkColor);
		return getIconFor(IMIconInfo(userID, url, name, IMICON_USR, sz, bkColor));
	}
	QPixmap getGrpIcon(const QString &grpID, const QString &url, const QSize& sz = DEFUALT_ICONSZ, const QColor &bkColor = g_IconBkColor){
		IMIconInfo info(grpID, url, "", IMICON_GRP, sz, bkColor);
		return getIconFor(IMIconInfo(grpID, url, "", IMICON_GRP, sz, bkColor));
	}

public:
	void bindIcon(QObject *p, const IMIconInfo &iconInfo, const std::function<void(const QPixmap &pix)> &upImgFunc);
	void unbindIcon(QObject *p);

	void bindIcon(QLabel *p, const IMIconInfo &iconInfo) { bindIcon(p, iconInfo, [p](const QPixmap &pix){p->setPixmap(pix);}); }
	void bindIcon(QAbstractButton *p, const IMIconInfo &iconInfo) { bindIcon(p, iconInfo, [p](const QPixmap &pix){p->setIcon(pix); }); }
	void bindIcon(ListWidgetItemWithIMIcon *p, const IMIconInfo &iconInfo);
	void unbindIcon(ListWidgetItemWithIMIcon *p);

public:
	struct UrlCookie
	{
		QString		_userId;
		QString		_groupId;

		UrlCookie(){}
		UrlCookie(const QString &uid, const QString &gid) : _userId(uid), _groupId(gid) {}
		bool operator==(const UrlCookie& other) const {
			return _userId == other._userId && _groupId == other._groupId;
		}
	};
protected:
	struct DownloadDat
	{
		QString		_httpID;
		QString		_locFileName;
		QSet<UrlCookie> _cookies;
	};

	QPixmap getIconFor(const IMIconInfo &info);
	QPixmap getUserIcon_inner(const QString &userID, const QString &url = QString(), const QString &name = QString(), const QSize& sz = DEFUALT_ICONSZ, const QColor &bkColor = g_IconBkColor);
	QPixmap getGrpIcon_inner(const QString &grpID, const QString &url = QString(), const QSize& sz = DEFUALT_ICONSZ, const QColor &bkColor = g_IconBkColor);
	void downloadNext();
	int  QSizeHashKey(const QSize &sz) { return sz.width() << 16 | (sz.height() & 0xFFFF); }

	void makeIconRoundBound(QPixmap &pix, const QSize &contentSz = QSize());
	QPixmap getIconFromUrl(const QString &url, const QSize &sz, const UrlCookie &cookie);
	QPixmap makeIconFor(const QString &name, const QSize& sz, const QColor &bkColor = g_IconBkColor);
	QPixmap makeIconFor(const QString &url, const QString &name, const QSize& sz, const UrlCookie &cookie, const QColor &bkColor = g_IconBkColor);
	QPixmap makeIconFor(const QList<CRIMGrpMemberInfo*> &lst, const QSize& sz, const QColor &bkColor = g_IconBkColor);
	QPixmap getIMUserIcon(const CRIMPublicUserInfo *pInfo, const QSize& sz, const QColor &bkColor = g_IconBkColor);
	QPixmap getIconFromCach(const QString& id, const QSize &sz);

protected:
	enum BindObjType { BindNULL=0, BindQObj, BindListItem };
	struct BindInfo
	{
		BindObjType _type;
		IMIconInfo	_iconInfo;
		std::function<void(const QPixmap &pix)> _func;
		BindInfo(){ _type = BindNULL; }
	};

	//更新缓存并且更新配置
	void updateIconInfo(const QString& id, const QString &url, const QString &name);
	//只更新缓存，不更新配置
	void updateIconCache(const QString& id);

protected slots:
	void slot_selfInfoUpdated(const CRIMUserInfo& oldInfo, const CRIMUserInfo &newInfo);
	void slot_friendInfoChanged(const CRIMFriendInfo &oldInfo, const CRIMFriendInfo &newInfo);
	void slot_grpInfoChanged(const CRIMGrpInfo &oldInfo, const CRIMGrpInfo &newInfo);
	void slot_grpMemberInfoChanged(const CRIMGrpMemberInfo &oldInfo, const CRIMGrpMemberInfo &newInfo);
	void slot_grpMemberDeleted(const CRIMGrpMemberInfo &info);
	void slot_grpMemberAdded(const CRIMGrpMemberInfo &info);
	void slot_syncServerFinish();
	void slot_qobjDestroyed(QObject *pObj);

	void slot_networkRequestFinished(QNetworkReply *reply);

private:
	QNetworkAccessManager		_qNetworkMgr;
	QString						_faceImgPath;

	QMap<QString, DownloadDat>	_downloadUrls;
	QSet<QString>				_downloadings;
	QHash<QString, QMap<void*, BindInfo>> _bindInfs;	//key 1: usrid/grpid/url;  key 2: QObject/ListWidgetItemWithIMIcon
	QHash<QString, QMap<int, QPixmap>> _pixCach;		//key 1: usrid/grpid/url;  key 2: w<<16|h;
};


inline  uint qHash(const ImIconMgr::UrlCookie &key, size_t seed = 0) {
	return qHash(key._userId, seed) ^ qHash(key._groupId, seed);
}

#endif // IMICONMGR_H
