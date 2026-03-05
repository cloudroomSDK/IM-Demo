#ifndef QBUTTON_WITH_GIF_H
#define QBUTTON_WITH_GIF_H

class QGifLabel;
class QButtonWithGif : public QPushButton
{
	Q_OBJECT

public:
	QButtonWithGif(QWidget *parent = 0);
	void setEnabled(bool);
private:
	QGifLabel *m_lbGif;
};


#endif // QBUTTON_WITH_GIF_H
