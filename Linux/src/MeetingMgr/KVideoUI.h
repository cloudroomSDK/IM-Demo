#ifndef KVIDEOUI_H
#define KVIDEOUI_H

#include "CustomRenderWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class KVideoUI; }
QT_END_NAMESPACE

#define CustomRenderBase CustomVideoView

class KVideoUI : public CustomRenderBase
{
	Q_OBJECT

public:
	KVideoUI(QWidget* pParent = 0);
	virtual ~KVideoUI();

	//恢复初始值
	void clean();

	void setVideoInfo(const char* userID);
	void setVideoInfo(const CRVSDK::CRUserVideoID &cam);

	const CRVSDK::CRUserVideoID &getUsrCamID() const { return m_vId; }
	const CRBase::CRString &getUserId() const {	return m_vId._userID; }
	int getCameraId() const { return m_vId._videoID; }

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	Ui::KVideoUI	*ui{ nullptr };
	CRUserVideoID	m_vId;
	CRVSDK_ASTATUS	m_aST{ CRVSDK_AST_UNKNOWN };
	CRVSDK_VSTATUS	m_vST{ CRVSDK_VST_UNKNOWN };
};


#endif // KVIDEOUI_H
