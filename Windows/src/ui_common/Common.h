#ifndef _COMMON_H__INCLUDED_
#define _COMMON_H__INCLUDED_

bool isChinaMobile(const QString &phoneNum);
QString getGenderStr(int imSDKGender);

QDateTime CoverTime(int time);
//utc -> YYYY-MM-DD HH:MM:SS
QString CoverTime2LocalTime(const QString &time);
//[HH:]MM:SS
QString CoverDuration2Str(int seconds, bool bKeepHour = false);
//time_t -> HH:MM:SS (YYYY-MM-DD)
QString CoverTime2Str(int time);
//time_t -> HH:MM (YYYY-MM-DD)
QString CoverTime2MinStr(int time);

QString AddBackslashAtBegin(const QString &path);
QString RmBackslashAtBegin(const QString &path);
QString AddBackslashAtend(const QString &path);
QString RmBackslashAtend(const QString &path);
QString CorrectPathFormat(const QString &pathName);
QString FileNameIllegalCharDeal(const QString &NoPathfileName);
QString GetSuffixWithDot(const QFileInfo &inf);

QByteArray ReadDataFromFile(const QString &fileName);
bool WriteDataToFile(const QByteArray &dat, const QString &fileName, bool bAppend=false);
QString GetDriveNameFromFile(const QString &fileName);
int GetFileMd5(const QString &fileName, QString &md5);
QString MakeMd5(const QByteArray &dat);
QString makeUUID();
QString replaceDomainOfURL(const QString &url, const QString &newDomain);


bool IsLocFile(const QString &str);

inline QString stdStrToQStr(const std::string &a)
{
    return QString::fromUtf8(a.c_str(), int(a.length()));
}
inline std::string qStrToStdStr(const QString &a)
{
	QByteArray utf8 = a.toUtf8();
	return std::string(utf8.constData(), utf8.length());
}

inline CRBase::CRString qStrToCRStr(const QString &a)
{
	QByteArray utf8 = a.toUtf8();
	return CRByteArray(utf8.constData(), utf8.length());
}
inline QString crStrToQStr(const CRString &a)
{
	return QString::fromUtf8(a.constData(), a.length());
}
QStringList StdStringListToQStringList(const std::list<std::string> &v);
std::list<std::string> QStringListToStdStringList(const QStringList &v);

QVariantList QStringListToVariantList(const QStringList &list);

QMap<QString, QString> StdstringMap2QStringMap(const std::map<std::string, std::string> &);
void QStringMap2StdStringMap(const QMap<QString, QString> &, std::map<std::string, std::string> &);
std::string getMapValue(const std::map<std::string, std::string> &v, const std::string &key);

//value1,value2, ...
QList<int> strToIntList(const QString &str, const QString &sep = ",");

QString getVideoSDKErrDesc(CRVSDK_ERR_DEF err);

#endif //_COMMON_H__INCLUDED_
