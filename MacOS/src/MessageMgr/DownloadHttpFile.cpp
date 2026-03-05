#include "stdafx.h"
#include "DownloadHttpFile.h"


DownloadHttpFile::DownloadHttpFile(QWidget *parent) : QObject(parent)
{
	QObject::connect(&m_timeoutCheck, &QTimer::timeout, this, &DownloadHttpFile::slot_timeoutCheck);

	m_lastRecvTime = 0;
}

DownloadHttpFile::~DownloadHttpFile()
{
}

void DownloadHttpFile::download(const QString &url, const QString &locPathFileName, const QVariant &cookie)
{
	MainLogDebug("download: %s -> %s", url.toUtf8().constData(), locPathFileName.toUtf8().constData());
	m_url = url;
	m_cookie = cookie;
	m_locFile.setFileName(locPathFileName);

	QFileInfo fInfo(locPathFileName);
	QDir fDir;
	fDir.mkpath(fInfo.absolutePath());
	
	if (!m_locFile.open(QIODevice::WriteOnly))
	{
		MainLogWarn("Open loc file failed! file:%s", locPathFileName.toUtf8().constData());
		onErr(tr("下载文件失败！（打开本地文件失败)"));
		return;
	}

	m_lastRecvTime = QDateTime::currentMSecsSinceEpoch();
	m_timeoutCheck.start(1000);

	QUrl qurl(m_url);
	QNetworkRequest request(qurl);
	request.setRawHeader("Content-Type", "application/json; charset=utf-8");
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	cfgQNetworkRequestForHttps(&request, App::instance()->getCfgInfo().skipVerifyCert);

	m_netReply = m_netMgr.get(request);
	connect(m_netReply, &QIODevice::readyRead, this, &DownloadHttpFile::slot_readyRead);
	connect(m_netReply, &QNetworkReply::downloadProgress, this, &DownloadHttpFile::slot_downloadProgress);
	connect(m_netReply, &QNetworkReply::finished, this, &DownloadHttpFile::slot_httpFinished);
}

void DownloadHttpFile::slot_timeoutCheck()
{
	qint64 curTime = QDateTime::currentMSecsSinceEpoch();
	if (curTime - m_lastRecvTime > 30 * 1000)
	{
		MainLogWarn("Download timeout! url:%s", m_url.toUtf8().constData());
		onErr(tr("下载超时失败！"));
		return;
	}
}

void DownloadHttpFile::slot_readyRead()
{
	if (m_url.isEmpty())
		return;

	m_lastRecvTime = QDateTime::currentMSecsSinceEpoch();
	QByteArray data = m_netReply->readAll();
	int nWrted = int(m_locFile.write(data.constData(), data.length()));
	if (nWrted != data.length())
	{
		MainLogWarn("Download failed, write file err! url:%s", m_url.toUtf8().constData());
		onErr(tr("下载文件失败！（网络响应数据不正确)"));
		return;
	}
}

void DownloadHttpFile::slot_downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (m_url.isEmpty())
		return;

	emit s_progress(m_url, bytesReceived, bytesTotal, m_cookie);
}

void DownloadHttpFile::slot_httpFinished()
{
	if (m_url.isEmpty())
		return;

	QNetworkReply::NetworkError err = m_netReply->error();
	int httpCode = m_netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (err || httpCode != 200)
	{
		onErr(tr("下载文件失败！（网络响应数据不正确)"));
		return;
	}
	MainLogWarn("Download success! fileSize:%d, url:%s", int(m_locFile.size()), m_url.toUtf8().constData());
	
	QString url = m_url;
	QString fileName = m_locFile.fileName();
	abort(false);

	emit s_finished(url, fileName, m_cookie);
}

void DownloadHttpFile::abort(bool bRemoveFile)
{
	m_timeoutCheck.stop();
	m_locFile.close();
	if (bRemoveFile)
	{
		m_locFile.remove();
	}
	m_url.clear();

	m_netReply->disconnect(this);
	m_netReply->abort();
	m_netReply->deleteLater();
	m_netReply = Q_NULLPTR;
}

void DownloadHttpFile::onErr(const QString &msg)
{
	if (m_url.isEmpty())
		return;

	abort();
	emit s_err(m_url, msg, m_cookie);
}