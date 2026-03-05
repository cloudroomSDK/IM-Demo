#include "stdafx.h"
#include "QWidgetInstallKeyHandler.h"


QWidgetInstallKeyHandler::QWidgetInstallKeyHandler(QObject *parent) : QObject(parent)
{
}

QWidgetInstallKeyHandler::QWidgetInstallKeyHandler(QWidget *pWidget, int key, QObject *handle, const char *member) : QWidgetInstallKeyHandler(pWidget)
{
	installKey(pWidget, key, handle, member);
}

void QWidgetInstallKeyHandler::installKey(QWidget *pWidget, int key, QObject *handle, const char *member)
{
	const char* bracketPosition = strchr(member, '(');
	if (!bracketPosition || !(member[0] >= '0' && member[0] <= '2'))
	{
		//不是信号、也不是槽
		return;
	}
	QByteArray methodName(member + 1, bracketPosition - 1 - member); // extract method name

	//第一次安装filter
	if (_keyHandlers.find(pWidget) == _keyHandlers.end())
	{
		pWidget->installEventFilter(this);
	}
	Handler &dat = _keyHandlers[pWidget][key];
	dat._handle = handle;
	dat._member = methodName;
}

void QWidgetInstallKeyHandler::uninstallKey(QWidget *pWidget, int key)
{
	auto f = _keyHandlers.find(pWidget);
	if (f == _keyHandlers.end())
		return;

	f->remove(key);
	if (f->size() <= 0)
	{
		_keyHandlers.erase(f);
		pWidget->removeEventFilter(this);
	}
}

QWidgetInstallKeyHandler::~QWidgetInstallKeyHandler()
{
}


bool QWidgetInstallKeyHandler::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		auto fItem = _keyHandlers.find(obj);
		if (fItem != _keyHandlers.end())
		{
			int key = static_cast<QKeyEvent*>(event)->key();
			if (key == Qt::Key_Return)
				key = Qt::Key_Enter;

			auto fKey = fItem->find(key);
			if (fKey != fItem->end())
			{
				bool rslt = QMetaObject::invokeMethod(fKey->_handle, fKey->_member.constData());
				return true;
			}
		}
	}

	return QObject::eventFilter(obj, event);
}
