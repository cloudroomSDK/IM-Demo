#include "stdafx.h"
#include "UICommon.h"



QListWidgetItem* QListWidgetFindData(QListWidget *pList, const QVariant &dat, int role)
{
	if ( pList==NULL )
		return NULL;
	for(int i=0; i<pList->count(); i++)
	{
		QListWidgetItem *pItem = pList->item(i);
		if ( pItem->data(role)==dat )
			return pItem;
	}

	return NULL;
}

void QListWidgetShowAllItems(QListWidget *pList)
{
	if ( pList==NULL )
		return;
	for(int i=0; i<pList->count(); i++)
	{
		QListWidgetItem *pItem = pList->item(i);
		if ( pItem->isHidden() )
			pItem->setHidden(false);
	}
}

void WidgetStyleUpdate(QWidget *pWnd)
{
	pWnd->style()->unpolish(pWnd);
	pWnd->style()->polish(pWnd);

// 	pWnd->style()->polish(pWnd);
// 	QEvent event(QEvent::StyleChange);
// 	QApplication::sendEvent(pWnd, &event);
// 	pWnd->update();
// 	pWnd->updateGeometry();
}


void activeAllSubLayout(QLayout *playout)
{
	if (playout == NULL)
		return;

	int itemCount = playout->count();
	for (int i = 0; i<itemCount; i++)
	{
		QLayoutItem *pItem = playout->itemAt(i);
		QLayout *pItemLy = pItem->layout();
		if (pItemLy != NULL)
		{
			activeAllSubLayout(pItemLy);
			continue;
		}
			
		QWidget *pItemWdg = pItem->widget();
		if (pItemWdg != NULL)
		{
			QString objName = pItemWdg->objectName();
			activeAllSubLayout(pItemWdg->layout());
			pItemWdg->updateGeometry();
			continue;
		}
	}

	playout->activate();
}

void adjustDlgSize(QWidget *p)
{
	if (p == NULL)
		return;
	activeAllSubLayout(p->layout());
	p->adjustSize();
}

void ShowAndActiveWindow(QWidget *pWindow)
{
	if (!pWindow->isVisible())
	{
		pWindow->show();
	}
	else
	{
		QNoNCDialog *pDlg = dynamic_cast<QNoNCDialog*>(pWindow);
		if (pDlg != nullptr)
		{
			if (pDlg->isMinimized())
			{
				pDlg->setWindowState(pDlg->windowState() & ~Qt::WindowMinimized);
			}
		}
		else
		{
			if (pWindow->isMinimized())
			{
				pWindow->setWindowState(pWindow->windowState() & ~Qt::WindowMinimized);
			}
		}
	}

#ifdef WIN32
	HWND hWnd = (HWND)pWindow->internalWinId();
	if (hWnd != nullptr)
	{
		BOOL b2 = SetForegroundWindow(hWnd);
		BOOL b1 = BringWindowToTop(hWnd);
		HWND oldHwnd = SetActiveWindow(hWnd);
	}
#else
#endif
}

void CloseWindowAndAllSubs(QWidget *pWindow)
{
	const QObjectList &objList = pWindow->children();
	for (auto &item : objList)
	{
		if (!item->isWidgetType())
			continue;
		QWidget *pWidget = static_cast<QWidget*>(item);
		//qDebug() << "CloseWindowAndAllSubs:" <<  pWidget->metaObject()->className() << "," << pWidget->objectName();
		CloseWindowAndAllSubs(pWidget);
	}
	if (pWindow->isWindow())
	{
// 		qDebug() << "CloseWindowAndAllSubs:" << pWindow->metaObject()->className() << "," << pWindow->objectName();
		bool bModelDlg = pWindow->isModal();
		pWindow->close();
		if (bModelDlg)
		{
			//模式对话框关闭后，要等eventLoop返回
			QApplication::processEvents();
		}
	}
}

void HideWindowAnddAllSubs(QWidget *pWindow, QList<QWidget*> &hideWnds)
{
	if (pWindow->isWindow() && pWindow->isVisible())
	{
		hideWnds.push_back(pWindow);
		pWindow->hide();
	}
	const QObjectList &objList = pWindow->children();
	for (auto &item : objList)
	{
		if (!item->isWidgetType())
			continue;
		QWidget *pWidget = static_cast<QWidget*>(item);
		HideWindowAnddAllSubs(pWidget, hideWnds);
	}
}

void ShowAllWindows(const QList<QWidget*> &hideWnds)
{
    for (auto pWnd : hideWnds)
    {
#ifdef LINUX
        //linux全屏窗口hide再show时，不是全屏状态，在此进行全屏处理
        QNoNCDialog *pDlg = dynamic_cast<QNoNCDialog*>(pWnd);
        if ( pDlg!=nullptr && pDlg->isFullScreen() )
        {
            pDlg->showFullScreen();
            continue;
        }
#endif
        pWnd->show();
    }
}

void addDropDownIconFor(QAbstractButton *button)
{
	addSubWidgetFor(button, "DragDown");
}

QWidget* addSubWidgetFor(QWidget *w, const QString& objName, Qt::Alignment alignment)
{
	QWidget *r = new QWidget(w);
	r->setObjectName(objName);
	r->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	addSubWidgetFor(w, r, alignment);
	return r;
}


void addSubWidgetFor(QWidget *w, QWidget *pSubWidget, Qt::Alignment alignment)
{
	QHBoxLayout *layOut = new QHBoxLayout(w);
	layOut->setContentsMargins(0, 0, 0, 0);
	//水平布局，只有top, bottom对齐生效， left和right采用弹簧方式实现
	if (alignment & Qt::AlignRight)
	{
		layOut->addStretch(0);
	}
	layOut->addWidget(pSubWidget, 0, alignment);
	if (alignment & Qt::AlignLeft)
	{
		layOut->addStretch(0);
	}

}


QPoint centerOfWidget(QWidget *pWidget)
{
	QRect rt;
	if (pWidget)
	{
		rt = pWidget->frameGeometry();
		rt.moveTopLeft(pWidget->mapToGlobal(QPoint()));
	}
	else
	{
		rt = QApplication::desktop()->screenGeometry(-1);
	}
	return rt.center();
}

bool isDescendantsWidget(QWidget *w, QWidget *childWidget)
{
	if (w == NULL || childWidget == NULL)
		return false;

	
	for (QWidget* parentW = childWidget->parentWidget(); parentW != NULL; parentW = parentW->parentWidget())
	{
		if (parentW == w)
		{
			return true;
		}
	}
	return false;
}

void moveWindowToCenter(QWidget* window)
{
	// 获取屏幕的宽度和高度
	QRect screenRt = QApplication::desktop()->availableGeometry(window);

	// 计算子窗口的左上角坐标
	int x = screenRt.left() + (screenRt.width() - window->width()) / 2;
	int y = screenRt.top() + (screenRt.height() - window->height()) / 2;

	// 将子窗口移动到屏幕中央
	window->move(x, y);
}

void moveWindowCenterTo(QWidget* window, const QPoint &pt)
{
	QRect rt = window->geometry();
	rt.moveCenter(pt);
	window->setGeometry(rt);
}

void cfgQNetworkRequestForHttps(QNetworkRequest *req, bool skipVerifyCert)
{
	if (req == nullptr)
		return;
	
	QSslConfiguration config;
	if (skipVerifyCert)
	{
		config.setPeerVerifyMode(QSslSocket::VerifyNone);
	}
	config.setProtocol(QSsl::TlsV1SslV3);
	req->setSslConfiguration(config);
}
