#ifndef _QJSON_HELPER_H_
#define _QJSON_HELPER_H_

//QVariant <-> QByteArray
QByteArray QVariantToJosnStr(const QVariant &JsonData);
QVariant JsonStrToQVaraint(const QByteArray &JsonData);
inline QByteArray VarMapToJson(const QVariantMap &varmap) { return QVariantToJosnStr(varmap); }
inline QVariantMap JsonToVarMap(const QString &str) { return JsonStrToQVaraint(str.toUtf8()).toMap(); }

//QVariant <-> QString
inline QString QVariantToJosnQStr(const QVariant &JsonData){
	return QString::fromUtf8(QVariantToJosnStr(JsonData));
}

inline QVariant JsonQStrToQVaraint(const QString &JsonData){
	return JsonStrToQVaraint(JsonData.toUtf8());
}

//QVariant -> obj
template <typename T> T QVariantToStruct(const QVariant& value);
template <> inline int QVariantToStruct<int>(const QVariant& value) { return value.toInt(); }
template <> inline QString QVariantToStruct<QString>(const QVariant& value) { return value.toString(); }
template <typename T> std::list<T> QVariantToStructList(const QVariantList& value);
template <typename T> QList<T> QVariantToStructQList(const QVariantList& value);

//obj -> QVariant
template <typename T> QVariant StructToQVariant(const T& obj);
template <> inline QVariant StructToQVariant(const int& obj) { return QVariant(obj); }
template <> inline QVariant StructToQVariant(const QString& obj) { return QVariant(obj); }
template <typename T> QVariantList StructListToQVariant(const std::list<T>& value);
template <typename T> QVariantList StructListToQVariant(const QList<T>& value);

// obj -> string
template<typename T> T JsonStrToStruct(const QByteArray &json);
template<typename T> QByteArray StructToJsonStr(const T &json);

//////////////////////////////////////////////////////////////////////////
template <typename T> std::list<T> QVariantToStructList(const QVariant& value)
{
	std::list<T> lst;
	QVariantList qvarLst = value.toList();
	for (auto &item : qvarLst)
	{
		T v = QVariantToStruct<T>(item);
		lst.push_back(v);
	}
	return lst;
}

template <typename T> QList<T> QVariantToStructQList(const QVariantList& value)
{
	QList<T> lst;
	for (auto &item : value)
	{
		T v = QVariantToStruct<T>(item);
		lst.push_back(v);
	}
	return lst;
}

template <typename T> QVariantList StructListToQVariant(const std::list<T>& value)
{
	QVariantList lst;
	for (auto &item : value)
	{
		QVariant v = StructToQVariant(item);
		lst.push_back(v);
	}
	return lst;
}
template <typename T> QVariantList StructListToQVariant(const QList<T>& value)
{
	QVariantList lst;
	for (auto &item : value)
	{
		QVariant v = StructToQVariant(item);
		lst.push_back(v);
	}
	return lst;
}

template<typename T> T JsonStrToStruct(const QByteArray &value)
{
	QVariant var = JsonStrToQVaraint(value);
	return QVariantToStruct<T>(var.toMap());
}

template<typename T> QByteArray StructToJsonStr(const T &value)
{
	QVariantMap var = StructToQVariant(value);
	return QVariantToJosnStr(var);
}

#endif