#include "stdafx.h"
#include "Common.h"


bool isChinaMobile(const QString &phoneNum)
{
	//11位
	if (phoneNum.length() != 11)
		return false;

	//纯数字
	for (QChar c : phoneNum)
	{
		if (!c.isDigit())
			return false;
	}

	if (phoneNum[0] != '1')
		return false;

	int c2 = phoneNum[1].digitValue();
	if (c2 < 3)
		return false;

	return true;
}

QString getGenderStr(int imSDKGender)
{
	if (imSDKGender == 1)
	{
		return QObject::tr("男");
	}
	if (imSDKGender == 2)
	{
		return QObject::tr("女");
	}
	return QString();
}

QDateTime CoverTime(int time)
{
	QDateTime dtRec;
	dtRec.setTime_t(time);
	dtRec = dtRec.toLocalTime();
	return dtRec;
}

QString CoverTime2LocalTime(const QString &str)
{
	const static QString timeFormat("yyyy-MM-dd hh:mm:ss");
	QDateTime utcTime = QDateTime::fromString(str, timeFormat);
	utcTime.setTimeZone(QTimeZone::utc());

	QDateTime locTime = utcTime.toLocalTime();
	return locTime.toString(timeFormat);
}

QString CoverTime2Str(int time)
{
	QDateTime dtRec;
	dtRec.setTime_t(time);
	dtRec = dtRec.toLocalTime();

	QString strTime;
	if ( dtRec.date() == QDate::currentDate() )
	{
		strTime =  dtRec.toString("hh:mm:ss");
	}
	else
	{
		strTime =  dtRec.toString("hh:mm:ss (yyyy-MM-dd)");
	}

	return strTime;
}

QString CoverTime2MinStr(int time)
{
	QDateTime dtRec;
	dtRec.setTime_t(time);
	dtRec = dtRec.toLocalTime();

	QString strTime;
	if ( dtRec.date() == QDate::currentDate() )
	{
		strTime =  dtRec.toString("hh:mm");
	}
	else
	{
		strTime =  dtRec.toString("hh:mm (yyyy-MM-dd)");
	}

	return strTime;
}

QString CoverDuration2Str(int seconds, bool bKeepHour)
{
    int nHour = seconds / 3600;
    int nMin = (seconds % 3600) / 60;
    int nSec = seconds % 60;

    QString rslt;
    if (nHour > 0 || bKeepHour)
    {
        rslt += QString::asprintf("%02d:", nHour);
    }
    rslt += QString::asprintf("%02d:%02d", nMin, nSec);
    return rslt;
}

char GetBackslash_reverse()
{
#ifdef WIN32
	return '/';
#else
	return '\\';
#endif
};

QString AddBackslashAtBegin(const QString &path)
{
	QString rslt = CorrectPathFormat(path);
	if (!rslt.startsWith(QDir::separator()))
	{
		rslt = QDir::separator() + rslt;
	}
	return rslt;
}

QString RmBackslashAtBegin(const QString &path)
{
	QString rslt = CorrectPathFormat(path);
	if ( rslt.startsWith(QDir::separator()) )
	{
		rslt.remove(0, 1);
	}
	return rslt;
}


QString AddBackslashAtend(const QString &path)
{
	QString rslt = CorrectPathFormat(path);
	if ( !rslt.endsWith(QDir::separator()) )
	{
		rslt += QDir::separator();
	}
	return rslt;
}

QString RmBackslashAtend(const QString &path)
{
	QString rslt = CorrectPathFormat(path);
	if ( rslt.endsWith(QDir::separator()) )
	{
		rslt.remove(rslt.length()-1, 1);
	}
	return rslt;
}

QString CorrectPathFormat(const QString &pathName)
{
	QString rslt(pathName);
	rslt.replace(GetBackslash_reverse(), QDir::separator());
	return rslt;
}

QString FileNameIllegalCharDeal(const QString &NoPathfileName)
{
	QString rslt(NoPathfileName);
	rslt.remove('\r');
	rslt.replace('\\', '_');
	rslt.replace('/', '_');
	rslt.replace(':', '_');
	rslt.replace('*', '_');
	rslt.replace('"', '_');
	rslt.replace('\'', '_');
	rslt.replace('<', '_');
	rslt.replace('>', '_');
	rslt.replace('|', '_');
	rslt.replace('?', '_');
	rslt.replace('\n', '_');
	return rslt;
}

QString GetSuffixWithDot(const QFileInfo &inf)
{
	QString rslt = "." + inf.suffix();
	if ( rslt.length()<=1 )
	{
		rslt.clear();
	}
	return rslt;
}


QByteArray ReadDataFromFile(const QString &fileName)
{
	//打开文件
	QFile file(fileName);
	if ( !file.open(QIODevice::ReadOnly) )
		return QByteArray();

	return file.readAll();
}

bool WriteDataToFile(const QByteArray &dat, const QString &fileName, bool bAppend)
{
	//打开文件
	QFile file(fileName);
	QFile::OpenMode openMd = bAppend ? (QIODevice::WriteOnly | QIODevice::Append) : (QIODevice::WriteOnly | QIODevice::Truncate);
	if (!file.open(openMd))
		return false;

	int nWrt = file.write(dat);
	if (nWrt != dat.size())
		return false;

	return true;
}

int GetFileMd5(const QString &fileName, QString &md5)
{
	QFile file(fileName);
	if ( !file.open(QIODevice::ReadOnly) )
		return file.error();

	QCryptographicHash hash(QCryptographicHash::Md5);
	while(!file.atEnd()){
		QByteArray data_md5_file=file.read(1024*1024);
		hash.addData(data_md5_file);
	}

	md5 = QString::fromLocal8Bit(hash.result().toHex());
	return 0;
}


QString MakeMd5(const QByteArray &dat)
{
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(dat);
	return QString::fromLocal8Bit(hash.result().toHex());
}

QString makeUUID()
{
	QUuid guid = QUuid::createUuid();
	QString str = guid.toString();
	str.remove('{');
	str.remove('}');
	return str;
}

QMap<QString, QString> StdstringMap2QStringMap(const std::map<std::string, std::string> &varMap)
{
	QMap<QString, QString> qMap;
	for (std::map<std::string, std::string>::const_iterator it=varMap.begin(); it!=varMap.end(); it++)
	{
		qMap[it->first.c_str()] = it->second.c_str();
	}
	return qMap;
}

void QStringMap2StdStringMap(const QMap<QString, QString> &qMap, std::map<std::string, std::string> &stdMap)
{
	stdMap.clear();
	for(QMap<QString, QString>::const_iterator pos=qMap.begin(); pos!=qMap.end(); pos++)
	{
		stdMap[pos.key().toUtf8().constData()] = pos.value().toUtf8().constData();
	}
}

QString getMapValue(const QMap<QString, QString> &v, const QString &key)
{
	QMap<QString, QString>::const_iterator pos = v.find(key);
	if ( pos==v.end() )
		return "";
	return pos.value();
}

std::string getMapValue(const std::map<std::string, std::string> &v, const std::string &key)
{
	std::map<std::string, std::string>::const_iterator pos = v.find(key);
	if ( pos==v.end() )
		return "";
	return pos->second;
}


bool IsLocFile(const QString &str)
{
	//相对文件路径
	if (str.startsWith('.'))
		return true;
	//linux全路径
	if (str.startsWith('/'))
		return true;
	//windows path c:\xxx
	if (str.at(1) == ':')
		return true;
	//无':', 单纯的文件名
	if (str.indexOf(':') < 0)
		return true;
	return false;
}

QStringList StdStringListToQStringList(const std::list<std::string> &v)
{
	QStringList l;
    l.reserve(int(v.size()));
	for (auto &pos : v)
	{
		l.push_back(stdStrToQStr(pos));
	}
	return l;
}

std::list<std::string> QStringListToStdStringList(const QStringList &v)
{
	std::list<std::string> l;
	for (auto &pos : v)
	{
		l.push_back(qStrToStdStr(pos));
	}
	return l;
}

QVariantList QStringListToVariantList(const QStringList &v)
{
	QVariantList l;
	for (auto &pos : v)
	{
		l.push_back(QVariant(pos));
	}
	return l;
}


QList<int> strToIntList(const QString &str, const QString &sep)
{
	QStringList strs = str.split(sep, QString::SkipEmptyParts);
	QList<int> rslt;
	for (auto &s : strs)
	{
		rslt.push_back(s.toInt());
	}
	return rslt;
}

QString getVideoSDKErrDesc(CRVSDK_ERR_DEF err)
{
	static QMap<int, QString> errs = {
		{ CRVSDKERR_VCAM_URLERR, "ipcam url不正确" },
		{ CRVSDKERR_VCAM_ALREADYEXIST, "已存在" },
		{ CRVSDKERR_VCAM_TOOMANY, "添加太多" },
		{ CRVSDKERR_VCAM_INVALIDFMT, "不支持的格式" },
		{ CRVSDKERR_VCAM_INVALIDMONITOR, "无效的屏幕id" },

		{ CRVSDKERR_NOERR, "没有错误" },
		{ CRVSDKERR_UNKNOWERR, "未知错误" },
		{ CRVSDKERR_OUTOF_MEM, "内存不足" },
		{ CRVSDKERR_INNER_ERR, "sdk内部错误" },
		{ CRVSDKERR_MISMATCHCLIENTVER, "不支持的sdk版本" },
		{ CRVSDKERR_PARAM_ERR, "参数错误" },
		{ CRVSDKERR_ERR_DATA, "无效数据" },
		{ CRVSDKERR_ANCTPSWD_ERR, "AppID或AppSecret不正确" },
		{ CRVSDKERR_SERVER_EXCEPTION, "服务异常" },
		{ CRVSDKERR_LOGINSTATE_ERROR, "登录状态错误" },
		{ CRVSDKERR_KICKOUT_BY_RELOGIN, "帐号在别处被使用" },
		{ CRVSDKERR_NOT_INIT, "sdk未初始化" },
		{ CRVSDKERR_NOT_LOGIN, "还没有登录" },
		{ CRVSDKERR_BASE64_COV_ERR, "base64转换失败" },
		{ CRVSDKERR_CUSTOMAUTH_NOINFO, "启用了第三方鉴权，但没有携带鉴权信息" },
		{ CRVSDKERR_CUSTOMAUTH_NOTSUPPORT, "没有启用第三方鉴权，但携带了鉴权信息" },
		{ CRVSDKERR_CUSTOMAUTH_EXCEPTION, "访问第三方鉴权服务异常" },
		{ CRVSDKERR_CUSTOMAUTH_FAILED, "第三方鉴权不通过" },
		{ CRVSDKERR_TOKEN_TIMEOUT, "token已过期" },
		{ CRVSDKERR_TOKEN_AUTHINFOERR, "鉴权信息错误" },
		{ CRVSDKERR_TOKEN_APPIDNOTEXIST, "appid不存在" },
		{ CRVSDKERR_TOKEN_AUTH_FAILED, "鉴权失败" },
		{ CRVSDKERR_TOKEN_NOTTOKENTYPE, "非token鉴权方式" },
		{ CRVSDKERR_API_NO_PERMISSION, "没有api访问权限" },

		{ CRVSDKERR_ACCOUNT_EXPIRED, "账号已过期" },
		{ CRVSDKERR_CLIENT_NO_PERMISSION, "所有终端未授权" },
		{ CRVSDKERR_CLIENT_SIP_NO_PERMISSION, "sip/h323终端未授权" },
		{ CRVSDKERR_CLIENT_IPC_NO_PERMISSION, "IPC终端未授权" },
		{ CRVSDKERR_CLIENT_PLATFORM_NO_PERMISSION, "当前使用的终端平台未授权" },
		{ CRVSDKERR_CLIENT_PLATFORM_UNSPPORT, "不支持当前使用的终端平台" },
		{ CRVSDKERR_FUNC_UNSPPORT, "不支持的功能或操作" },

		{ CRVSDKERR_NETWORK_INITFAILED, "网络初始化失败" },
		{ CRVSDKERR_NO_SERVERINFO, "没有服务器信息" },
		{ CRVSDKERR_NOSERVER_RSP, "服务器没有响应" },
		{ CRVSDKERR_CREATE_CONN_FAILED, "创建连接失败" },
		{ CRVSDKERR_SOCKETEXCEPTION, "socket异常" },
		{ CRVSDKERR_SOCKETTIMEOUT, "网络超时" },
		{ CRVSDKERR_FORCEDCLOSECONNECTION, "连接被关闭" },
		{ CRVSDKERR_CONNECTIONLOST, "连接丢失" },
		{ CRVSDKERR_VOICEENG_INITFAILED, "语音引擎初始化失败" },
		{ CRVSDKERR_SSL_ERR, "ssl通信错误" },
		{ CRVSDKERR_RSPDAT_ERR, "响应数据不正确" },
		{ CRVSDKERR_DATAENCRYPT_ERR, "数据加密失败" },
		{ CRVSDKERR_DATADECRYPT_ERR, "数据加密失败" },

		{ CRVSDKERR_QUE_ID_INVALID, "队列ID错误" },
		{ CRVSDKERR_QUE_NOUSER, "没有用户在排队" },
		{ CRVSDKERR_QUE_USER_CANCELLED, "排队用户已取消" },
		{ CRVSDKERR_QUE_SERVICE_NOT_START, "没有开启队列服务" },
		{ CRVSDKERR_ALREADY_OTHERQUE, "已在其它队列排队" },

		{ CRVSDKERR_INVALID_CALLID, "无效的呼叫ID" },
		{ CRVSDKERR_CALL_EXIST, "已在呼叫中" },
		{ CRVSDKERR_PEER_BUSY, "对方忙" },
		{ CRVSDKERR_PEER_OFFLINE, "对方不在线" },
		{ CRVSDKERR_PEER_NOANSWER, "对方无应答" },
		{ CRVSDKERR_PEER_NOT_FOUND, "用户不存在" },
		{ CRVSDKERR_PEER_REFUSE, "对方拒接" },

		{ CRVSDKERR_MEETNOTEXIST, "会议不存在或已结束" },
		{ CRVSDKERR_AUTHERROR, "会议密码不正确" },
		{ CRVSDKERR_MEMBEROVERFLOW, "会议终端数量已满" },
		{ CRVSDKERR_RESOURCEALLOCATEERROR, "分配会议资源失败" },
		{ CRVSDKERR_MEETROOMLOCKED, "会议已加锁" },
		{ CRVSDKERR_BALANCELESS, "余额不足" },
		{ CRVSDKERR_SEVICE_NOTENABLED, "业务权限未开启" },
		{ CRVSDKERR_ALREADYINMEETING, "不能再次入会" },
		{ CRVSDKERR_MIC_NORIGHT, "没有mic权限" },
		{ CRVSDKERR_MIC_BEING_USED, "mic已被使用" },
		{ CRVSDKERR_MIC_UNKNOWERR, "mic未知错误" },
		{ CRVSDKERR_SPK_NORIGHT, "没有扬声器权限" },
		{ CRVSDKERR_SPK_BEING_USED, "扬声器已被使用" },
		{ CRVSDKERR_SPK_UNKNOWERR, "扬声器未知错误" },
		{ CRVSDKERR_PIC_ISNULL, "图像为空" },
		{ CRVSDKERR_DEV_NOTEXIST, "设备不存在" },
		{ CRVSDKERR_MIC_OPENTOOMUCH, "开麦达到上限" },
		{ CRVSDKERR_NOT_INMEETING, "还没有入会" },
		{ CRVSDKERR_REPEAT_FAIL, "数据重复或功能重复开启" },

		{ CRVSDKERR_CATCH_SCREEN_ERR, "抓屏失败" },
		{ CRVSDKERR_RECORD_MAX, "单次录制达到最大时长" },
		{ CRVSDKERR_RECORD_NO_DISK, "磁盘空间不够" },
		{ CRVSDKERR_RECORD_SIZE_ERR, "录制尺寸超出了允许值" },
		{ CRVSDKERR_CFG_RESTRICTED, "录制超出限制" },
		{ CRVSDKERR_FILE_ERR, "录制写文件或推流失败" },
		{ CRVSDKERR_RECORDSTARTED, "录制已开启" },
		{ CRVSDKERR_NOMORE_MCU, "录制服务器资源不足" },
		{ CRVSDKERR_SVRRECORD_SPACE_FULL, "云端录像空间已满" },

		{ CRVSDKERR_SENDFAIL, "发送失败" },
		{ CRVSDKERR_CONTAIN_SENSITIVEWORDS, "有敏感词语" },

		{ CRVSDKERR_SENDCMD_LARGE, "发送信令数据过大" },
		{ CRVSDKERR_SENDBUFFER_LARGE, "发送数据过大" },
		{ CRVSDKERR_SENDDATA_TARGETINVALID, "目标用户不存在" },
		{ CRVSDKERR_SENDFILE_FILEINERROR, "文件错误" },
		{ CRVSDKERR_TRANSID_INVALID, "无效的发送id" },

		{ CRVSDKERR_RECORDFILE_STATE_ERR, "状态错误不可上传/取消上传" },
		{ CRVSDKERR_RECORDFILE_NOT_EXIST, "录制文件不存在" },
		{ CRVSDKERR_RECORDFILE_UPLOAD_FAILED, "上传失败，失败原因参考日志" },
		{ CRVSDKERR_RECORDFILE_DEL_FAILED, "移除本地文件失败" },

		{ CRVSDKERR_FILE_NOT_EXIST, "文件不存在" },
		{ CRVSDKERR_FILE_READ_ERR, "文件读失败" },
		{ CRVSDKERR_FILE_WRITE_ERR, "文件写失败" },
		{ CRVSDKERR_FILE_ALREADY_EXIST, "目标文件已存在" },
		{ CRVSDKERR_FILE_OPERATOR_ERR, "文件操作失败" },
		{ CRVSDKERR_FILE_SIZE_UNSUPPORT, "不支持的文件尺寸" },

		{ CRVSDKERR_NETDISK_NOT_EXIST, "网盘不存在" },
		{ CRVSDKERR_NETDISK_PERMISSIONDENIED, "没有网盘权限" },
		{ CRVSDKERR_NETDISK_INVALIDFILENAME, "不合法文件名" },
		{ CRVSDKERR_NETDISK_FILEALREADYEXISTS, "文件已存在" },
		{ CRVSDKERR_NETDISK_FILEORDIRECTORYNOTEXISTS, "文件或目录不存在" },
		{ CRVSDKERR_NETDISK_FILENOTTRANSFORM, "文件没有转换" },
		{ CRVSDKERR_NETDISK_TRANSFORMFAILED, "文件转换失败" },
		{ CRVSDKERR_NETDISK_NOSPACE, "空间不足" },

		{ CRVSDKERR_PLUGIN_INITFAILED, "插件加载失败" },
		{ CRVSDKERR_PLUGIN_NOTINIT, "插件还未初始化" },
		{ CRVSDKERR_PLUGIN_CREATEFAILED, "创建插件实例失败" }
	};

	auto pos = errs.find(err);
	if (pos == errs.end())
	{
		return QString("未知错误(%1)").arg(err);
	}
	return pos.value();
}
