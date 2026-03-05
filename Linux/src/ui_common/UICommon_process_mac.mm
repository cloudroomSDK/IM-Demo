#include "stdafx.h"
#include "UICommon_process.h"
#ifdef MAC
#import <AppKit/AppKit.h>
#import <IOKit/pwr_mgt/IOPMLib.h>
#else
#import <UIKit/UIApplication.h>
#endif

int OpenAndSelectFile(const QString &file)
{
#ifdef MAC
    QFileInfo fileInfo(file);
    QString fileDir = fileInfo.absoluteDir().absolutePath();
    bool success = [[NSWorkspace sharedWorkspace] selectFile:file.toNSString() inFileViewerRootedAtPath:fileDir.toNSString()];
    return success?40:0;
#else
    return 0;
#endif
}

int OpenLocFile(const QString &file)
{
#ifdef MAC
    bool success = [[NSWorkspace sharedWorkspace] openFile:file.toNSString()];
    return success?40:0;
#else
    return 0;
#endif
}

void GetShellTrayWndInfo(ShellTrayWndInfo &info)
{
}

#ifdef MAC
void preventSystemSleep() {
    // kIOPMAssertionTypeNoDisplaySleep prevents display sleep,
    // kIOPMAssertionTypeNoIdleSleep prevents idle sleep
    
    // reasonForActivity is a descriptive string used by the system whenever it needs
    // to tell the user why the system is not sleeping. For example,
    // "Mail Compacting Mailboxes" would be a useful string.
    
    //  NOTE: IOPMAssertionCreateWithName limits the string to 128 characters.
    CFStringRef reasonForActivity= CFSTR("Describe Activity Type");
    
    IOPMAssertionID assertionID;
    IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                                   kIOPMAssertionLevelOn, reasonForActivity, &assertionID);
    if (success == kIOReturnSuccess)
    {
        
        // Add the work you need to do without
        // the system sleeping here.
        
//        success = IOPMAssertionRelease(assertionID);
        // The system will be able to sleep again.
    }
 
}
#endif
