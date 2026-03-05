#ifndef __UICOMMON_PROCESS_H
#define __UICOMMON_PROCESS_H

int OpenAndSelectFile(const QString &file);
int OpenLocFile(const QString &file);

enum DIRECTION_TYPE { DIRECTION_LEFT = 0, DIRECTION_TOP, DIRECTION_RIGHT, DIRECTION_BOTTOM };
struct ShellTrayWndInfo
{
	QRect			cfgRt;	//配置的显示位置
	DIRECTION_TYPE	type;
	bool			bAutoHide;
};
void GetShellTrayWndInfo(ShellTrayWndInfo &info);
#ifdef MAC
void preventSystemSleep();
#endif
#ifdef LINUX
QString GetOSName();
#endif
#endif
