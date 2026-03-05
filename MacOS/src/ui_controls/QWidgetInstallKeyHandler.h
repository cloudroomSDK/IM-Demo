#ifndef QWidgetInstallKeyHandler_H
#define QWidgetInstallKeyHandler_H


class QWidgetInstallKeyHandler : public QObject
{
	Q_OBJECT

public:
	QWidgetInstallKeyHandler(QObject *parent=NULL);
	QWidgetInstallKeyHandler(QWidget *pWidget, int key, QObject *handle, const char *member);
	~QWidgetInstallKeyHandler();

	void installKey(QWidget *pWidget, int key, QObject *handle, const char *member);
	void uninstallKey(QWidget *pWidget, int key);

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	struct Handler
	{
		QObject*	_handle;
		QByteArray	_member;
	};
	QMap<QObject*, QMap<int, Handler>>	_keyHandlers;
};

#endif // _QWidgetInstallKeyHandler_H_
