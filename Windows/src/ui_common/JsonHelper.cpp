#include "stdafx.h"
#include "JsonHelper.h"
#include <QJsonDocument> 

QByteArray QVariantToJosnStr(const QVariant &JsonData)
{
	return QJsonDocument::fromVariant(JsonData).toJson(QJsonDocument::Compact);
}

QVariant JsonStrToQVaraint(const QByteArray &JsonData)
{
    return QJsonDocument::fromJson(JsonData).toVariant();
}
