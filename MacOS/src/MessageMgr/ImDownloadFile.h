#ifndef ImDownloadFile_H
#define ImDownloadFile_H

#include "DownloadHttpFile.h"
#define  DOWNLOAD_FILE_SUFFIX ".download"
class ImDownloadFile : public QObject
{
	Q_OBJECT

public:
	static ImDownloadFile* instance();

	void download(const QString &url, const QString &locPathFileName, const QVariant &cookie);
	void abort(const QString &url);
	void abortAll();

signals:
	void s_Err(const QString &url, const QString &errMsg, const QVariant &cookie);
	void s_Progress(const QString &url, qint64 pos, qint64 size, const QVariant &cookie);
	void s_finished(const QString &url, const QString &locPathFileName, const QVariant &cookie);

protected slots:
	void slot_httpErr(const QString &url, const QString &errMsg, const QVariant &cookie);
	void slot_httpProgress(const QString &url, qint64 pos, qint64 size, const QVariant &cookie);
	void slot_httpfinished(const QString &url, const QString &locPathFileName, const QVariant &cookie);

protected:
	ImDownloadFile(QWidget *parent = 0);
	~ImDownloadFile();
	void removeOne(const QString &url);

private:
	static ImDownloadFile* m_instance;
	QVariantMap m_params;
	QMap<QString, DownloadHttpFile*> m_downloadFiles;
};

#endif // ImDownloadFile_H
