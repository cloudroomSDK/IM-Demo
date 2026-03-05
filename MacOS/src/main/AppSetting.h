#ifndef APPSETTING_H
#define APPSETTING_H

#include <QWidget>
#include "ui_AppSetting.h"

class AppSetting : public QWidget
{
	Q_OBJECT

public:
	AppSetting(QWidget *parent = 0);
	~AppSetting();

private:
	Ui::AppSetting ui;
};

#endif // APPSETTING_H
