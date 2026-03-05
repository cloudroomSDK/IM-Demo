#ifndef WIDGETQSSLAYABLE_H
#define WIDGETQSSLAYABLE_H

#include <QWidget>

class WidgetQssLayAble : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(int layoutMargin READ layoutMargin WRITE setlayoutMargin)
	Q_PROPERTY(int layoutMarginLeft READ layoutMarginLeft WRITE setlayoutMarginLeft)
	Q_PROPERTY(int layoutMarginTop READ layoutMarginTop WRITE setlayoutMarginTop)
	Q_PROPERTY(int layoutMarginRight READ layoutMarginRight WRITE setlayoutMarginRight)
	Q_PROPERTY(int layoutMarginBottom READ layoutMarginBottom WRITE setlayoutMarginBottom)
	Q_PROPERTY(int layoutSpacing READ layoutSpacing WRITE setlayoutSpacing)
	Q_PROPERTY(int layoutVSpacing READ layoutVSpacing WRITE setlayoutVSpacing)
	Q_PROPERTY(int layoutHSpacing READ layoutHSpacing WRITE setlayoutHSpacing)

public:
	WidgetQssLayAble(QWidget *parent = NULL);
	~WidgetQssLayAble();

protected:
	void paintEvent(QPaintEvent *event) override;

protected:
	int layoutMargin() const;
	int layoutMarginLeft() const;
	int layoutMarginTop() const;
	int layoutMarginRight() const;
	int layoutMarginBottom() const;
	int layoutSpacing() const;
	int layoutVSpacing() const;
	int layoutHSpacing() const;

	void setlayoutMargin(int);
	void setlayoutMarginLeft(int);
	void setlayoutMarginTop(int);
	void setlayoutMarginRight(int);
	void setlayoutMarginBottom(int);
	void setlayoutSpacing(int);
	void setlayoutVSpacing(int);
	void setlayoutHSpacing(int);


private:
	
};

#endif // WIDGETQSSLAYABLE_H
