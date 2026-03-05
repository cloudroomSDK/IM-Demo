#include "stdafx.h"

void OutPutLog(E_LOG_LEVEL_DEF level, const char *modlue, const char* strFormat, ...)
{
	va_list argptr;
	va_start(argptr, strFormat);
	QString str = QString::vasprintf(strFormat, argptr);
	va_end(argptr);

	QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
	qDebug("%s | %s | %lld | %s", strTime.toUtf8().constData(), modlue, int64_t(QThread::currentThreadId()), str.toUtf8().constData());
}
