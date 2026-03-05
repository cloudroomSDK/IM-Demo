#include "stdafx.h"
#include "UICommon_process.h"


int OpenAndSelectFile(const QString &file)
{
    QString selectName = CorrectPathFormat(file);

    HINSTANCE hi = 0;
    bool bFileExist = QFile::exists(selectName);
    if ( !bFileExist )
    {
        QFileInfo fInfo(selectName);
        QString path = fInfo.absolutePath();
        hi = ShellExecute(NULL, L"open", path.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
    else
    {
        QString param =	"/n,/e,/select," + selectName;
        hi = ShellExecute(NULL, L"open", L"explorer.exe", param.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
    }
    return 1;
}

int OpenLocFile(const QString &file)
{
    HINSTANCE result = ShellExecute(NULL, L"open", file.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
    return 1;
}

void GetShellTrayWndInfo(ShellTrayWndInfo &info)
{
	//得到任务栏是否自动隐藏
	APPBARDATA taskbar;
	memset(&taskbar, 0, sizeof(APPBARDATA));
	taskbar.cbSize = sizeof(APPBARDATA);
	DWORD state = SHAppBarMessage(ABM_GETSTATE, &taskbar);
	info.bAutoHide = (state & ABS_AUTOHIDE);

	//得到位置
	memset(&taskbar, 0, sizeof(APPBARDATA));
	taskbar.cbSize = sizeof(APPBARDATA);
	SHAppBarMessage(ABM_GETTASKBARPOS, &taskbar);
	info.cfgRt.setRect(taskbar.rc.left, taskbar.rc.top, taskbar.rc.right - taskbar.rc.left, taskbar.rc.bottom - taskbar.rc.top);
	info.type = (DIRECTION_TYPE)taskbar.uEdge;
}
