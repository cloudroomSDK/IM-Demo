#ifndef CRDialogBtns_h
#define CRDialogBtns_h

#include <QNoNCDialog.h>

namespace Ui { class CRDialogBtns; }
class CRDialogBtns : public TitleBar
{
	Q_OBJECT

public:
	CRDialogBtns(QWidget *parent);
	virtual ~CRDialogBtns();

protected:
	void showEvent(QShowEvent *event) override;

private:
	Ui::CRDialogBtns *ui;
};


#endif // CRDialogBtns_h
