#include "stdafx.h"
#include "ImDownloadFile.h"

ImDownloadFile* ImDownloadFile::m_instance = NULL;

ImDownloadFile* ImDownloadFile::instance()
{
	if (m_instance == NULL)
	{
		m_instance = new ImDownloadFile();
	}
	return m_instance;
}

ImDownloadFile::ImDownloadFile(QWidget *parent) : QObject(parent)
{

}

ImDownloadFile::~ImDownloadFile()
{
	m_instance = NULL;
}

void ImDownloadFile::download(const QString &url, const QString &locPathFileName, const QVariant &cookie)
{
	if (m_downloadFiles.contains(url))
		return;

	QString downloadFileName = locPathFileName + DOWNLOAD_FILE_SUFFIX;
	if (QFile::exists(downloadFileName))
	{
		emit s_Err(url, tr("文件已存在"), cookie);
		return;
	}
	DownloadHttpFile *downloadFileObj = new DownloadHttpFile();
	downloadFileObj->download(url, downloadFileName, cookie);
	connect(downloadFileObj, &DownloadHttpFile::s_err, this, &ImDownloadFile::slot_httpErr);
	connect(downloadFileObj, &DownloadHttpFile::s_progress, this, &ImDownloadFile::slot_httpProgress);
	connect(downloadFileObj, &DownloadHttpFile::s_finished, this, &ImDownloadFile::slot_httpfinished);
	m_downloadFiles[url] = downloadFileObj;
}

void ImDownloadFile::abort(const QString &url)
{
	if (m_downloadFiles.contains(url))
	{
		m_downloadFiles[url]->abort();
		removeOne(url);
	}
}

void ImDownloadFile::abortAll()
{
	for (auto it = m_downloadFiles.begin(); it != m_downloadFiles.end(); it++)
	{
		DownloadHttpFile *downloadFileObj = it.value();
		downloadFileObj->abort();
		downloadFileObj->deleteLater();
	}
	m_downloadFiles.clear();
}

void ImDownloadFile::removeOne(const QString &url)
{
	auto it = m_downloadFiles.find(url);
	if (it == m_downloadFiles.end())
		return;

	it.value()->deleteLater();
	m_downloadFiles.erase(it);
}

void ImDownloadFile::slot_httpErr(const QString &url, const QString &errMsg, const QVariant &cookie)
{
	removeOne(url);
	emit s_Err(url, errMsg, cookie);
}

void ImDownloadFile::slot_httpProgress(const QString &url, qint64 pos, qint64 size, const QVariant &cookie)
{
	emit s_Progress(url, pos, size, cookie);
}

void ImDownloadFile::slot_httpfinished(const QString &url, const QString &downloadFileName, const QVariant &cookie)
{
	removeOne(url);

	QString locPathFileName = downloadFileName;
	QString tmpSuffix = DOWNLOAD_FILE_SUFFIX;
	if (locPathFileName.endsWith(tmpSuffix))
	{
		int index = locPathFileName.lastIndexOf(tmpSuffix);
		locPathFileName = locPathFileName.remove(index, tmpSuffix.length());
		if (!QFile::rename(downloadFileName, locPathFileName))
		{
			emit s_Err(url, tr("无法生成目标文件"), cookie);
		}
	}
	
	emit s_finished(url, locPathFileName, cookie);
}
