#ifndef DOWNLOADHTTPFILE_H
#define DOWNLOADHTTPFILE_H


class DownloadHttpFile : public QObject
{
	Q_OBJECT

public:
	DownloadHttpFile(QWidget *parent = 0);
	~DownloadHttpFile();

	void download(const QString &url, const QString &locPathFileName, const QVariant &cookie = QVariant());
	void abort(bool bRemoveFile = true);

signals:
	void s_err(const QString &url, const QString &errMsg, const QVariant &cookie);
	void s_progress(const QString &url, qint64 pos, qint64 size, const QVariant &cookie);
	void s_finished(const QString &url, const QString &locPathFileName, const QVariant &cookie);

protected slots:
	void slot_readyRead();
	void slot_downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void slot_httpFinished();
	void slot_timeoutCheck();

protected:
	void onErr(const QString &msg);

private:
	QNetworkAccessManager m_netMgr;
	QNetworkReply *m_netReply;
	QString			m_url;
	QFile			m_locFile;
	QVariant		m_cookie;
	qint64			m_lastRecvTime;
	QTimer			m_timeoutCheck;

};

#endif // DownloadHttpFile_H
