#ifndef __UICOMMON_H
#define __UICOMMON_H

QListWidgetItem* QListWidgetFindData(QListWidget *pList, const QVariant &dat, int role=Qt::UserRole); 
void QListWidgetShowAllItems(QListWidget *pList);

void WidgetStyleUpdate(QWidget *pWnd);

//实现某对话框自动变大变小；
//1. 必须都在布局里； 
//2. 对话框将变到最小合适尺寸,如果想保持某个方向尺寸不变，需要setFixedWidth/setFixedHeight)
void adjustDlgSize(QWidget *pDlg);

void ShowAndActiveWindow(QWidget *pWindow);
void CloseWindowAndAllSubs(QWidget *pWindow);
void HideWindowAnddAllSubs(QWidget *pWindow, QList<QWidget*> &hideWnds);
void ShowAllWindows(const QList<QWidget*> &hideWnds);

//为按钮增加一个右下三角图标
void addDropDownIconFor(QAbstractButton *button);
QWidget* addSubWidgetFor(QWidget *w, const QString& objName, Qt::Alignment alignment = Qt::AlignRight|Qt::AlignBottom);
void addSubWidgetFor(QWidget *w, QWidget *pSubWidget, Qt::Alignment alignment = Qt::AlignRight | Qt::AlignBottom);


QPoint centerOfWidget(QWidget *p);
void moveWindowToCenter(QWidget* window);
void moveWindowCenterTo(QWidget* window, const QPoint &pt);

QScreen* getScreenByWidget(QWidget *p);
QScreen* getScreenByPos(const QPoint &pt);

//检查是否为子孙窗口
bool isDescendantsWidget(QWidget *w, QWidget *childWidget);

bool GotoURL(const QString &url);

void cfgQNetworkRequestForHttps(QNetworkRequest *req, bool skipVerifyCert);


#endif	//__UICOMMON_H
