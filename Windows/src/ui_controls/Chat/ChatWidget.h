#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include "ChatModel.h"
#include "ChatStyle.h"
class WidgetLoadMore_Mgr;
class ChatWidget : public QListView
{
	Q_OBJECT

public:
	explicit ChatWidget(QWidget *parent = nullptr);
	virtual ~ChatWidget();

	//设置视图模式
	void setChatViewMode(ChatStyle::ChatViewMode mode);
	//写入数据
	int insertMsg(const UIMsgInfo &msg, bool insertFirst = false);
	int insertMsg(const QList<UIMsgInfo> &msgs, bool insertFirst = false);
	//删除数据
	void removeMsg(const QString &msgId);
	//更新数据
	void updateMsg(const UIMsgInfo &msg);
	//获取消息信息
	bool containsMsg(const QString &msgId) const;
	const UIMsgInfo& getMsgInfo(const QString &msgId) const;
	const UIMsgInfo& getFirstMsg() const;
	const UIMsgInfo& getLastMsg() const;
	//设置最大数量
	void setMaxCount(int maxCount = 5000){m_maxCount = maxCount;}
	int getMaxCount(){ return m_maxCount; }
	//获取当前数量
	int getCount(){ return m_model->rowCount() - 1; }
	//清空
	void clear();
	//图片资源替换
	void updateResource(const QString &msgId, const QUrl &resUrl);
	//跳转到对应位置
	void scrollToMsg(const QString &msgId);
	//消息在当前显示区域的后面
	bool isMsgAfterCurrentShow(const QString &msgId);
	//获取当前显示的消息
	QList<UIMsgInfo> getCurrentShowMsgs();

	//是否菜单自定义action
	enum MenuType
	{
		MENU_FACE,		//头像区域右键菜单
		MENU_CONTENT,	//内容区域右键菜单
		MENU_OTHERAREA,	//空白区域右键菜单
	};

	enum DATA_STATE
	{
		DS_NO_MOREDATA,	//没有更多数据
		DS_LOADING,		//加载中
		DS_CANLOAD,		//可加载
		DS_FULL,		//数据满了
	};
	//设置数据状态
	void setDataState(DATA_STATE state);

	const ChatStyle& getStyle() const { return m_style; }

signals:
	//头像左键单击
	void s_faceClicked(const UIMsgInfo &msg);
	//内容单击
	void s_contentClicked(CursorPosType type, const QString &cursorText, const UIMsgInfo &msg);
	//右键单击
	void s_rightClicked(ChatWidget::MenuType menuType, const UIMsgInfo &msg = UIMsgInfo(), const QVariantMap &exParams = QVariantMap());

	//回复某条消息
	void s_replyMsg(const QString &msgID);
	//删除了某条消息
	void s_onMsgDelelted(const QString &msgID);
	//清空了消息
	void s_onMsgCleared();
	//加载更多数据
	void s_loadMoreClicked();
	//打开历史记录
	void s_openHistoryClicked();
	
	//重新发送
	void s_resendClicked(const UIMsgInfo &msg);

protected:
	int getCurSelCount(){ return m_curSelItem.size(); }
	void copy(const QStringList &msgIDs);
	void copyCurSelect();
	const UIMsgInfo& getMsgInfo(const QModelIndex &index) const;

protected:
	bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent* ev) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
	void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *e) Q_DECL_OVERRIDE;
	void updateGeometries() Q_DECL_OVERRIDE;

	void clearSelection();
	void onLeftButtonClicked(QMouseEvent *event, bool bDoubleClick);
	int getRowForPos(const QPoint &pos);

protected slots:
	void slot_faceLeftClicked(const UIMsgInfo &msg);
	void slot_faceRightClicked(const UIMsgInfo &msg);
	void slot_contentLeftClicked(int row, const UIMsgInfo &info, const QVariantMap &exParams);
	void slot_contentRightClicked(int row, const UIMsgInfo &info, const QVariantMap &exParams);
	void slot_areaRightClicked();
	void updateUI();
	void updateShowItems();
	
private:
	int m_selRowStart;
	QPoint m_selStartPos;
	QList<int> m_curSelItem;
	ChatModel *m_model;

	UIMsgInfo m_infoForShowMenu;
	QTimer m_timer;
	int m_pageCount;
	int m_maxCount;
	WidgetLoadMore_Mgr *m_loadMoreWidget;

	ChatStyle m_style;
};

class WidgetLoadMore_Mgr : public QWidget
{
	Q_OBJECT
public:
	WidgetLoadMore_Mgr(QWidget *parent = 0);
	~WidgetLoadMore_Mgr();

	void setState(ChatWidget::DATA_STATE state);
	ChatWidget::DATA_STATE getState(){ return m_state; }
	bool canLoading(){ return m_state == ChatWidget::DS_CANLOAD; }
public:
	QPushButton *btnLoad;
	QPushButton *btnOpenHistory;
	QLabel *lbWaiting;
	QLabel *lbStateDesc;

private:
	QMovie *m_movie;
	ChatWidget::DATA_STATE m_state;
};
#endif // MYLISTWIDGET_H
