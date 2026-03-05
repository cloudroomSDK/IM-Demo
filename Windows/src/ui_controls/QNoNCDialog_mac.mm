#include "QNoNCDialog.h"
#include <Cocoa/Cocoa.h>

//此类用于支持重载系统按钮的行为
//this Objective-c class is used to override the action of sysytem close button and zoom button
//https://stackoverflow.com/questions/27643659/setting-c-function-as-selector-for-nsbutton-produces-no-results
@interface ButtonPasser : NSObject{
}
@property(readwrite) QNoNCDialog* window;
+ (void)closeButtonAction:(id)sender;
- (void)zoomButtonAction:(id)sender;
@end

@implementation ButtonPasser{   
}
+ (void)closeButtonAction:(id)sender
{
    Q_UNUSED(sender);
//    ProcessSerialNumber pn;
//    GetFrontProcess (&pn);
//    ShowHideProcess(&pn,false);
}
- (void)zoomButtonAction:(id)sender
{
    Q_UNUSED(sender);
    if (0 == self.window) return;
    if (self.window->isMaximized()) self.window->showNormal();
    else self.window->showMaximized();
}
@end

void QNoNCDialog::unifiedNativeTitle(bool bUnified)
{
    m_unifiedTitle = bUnified;
    if(this->windowFlags()&Qt::FramelessWindowHint)
    {
        return;
    }
    if(!m_initTitle)
    {
        return;
    }
    NSView* view = (NSView*)internalWinId();
    NSWindow *window = view.window;

    if(bUnified)
    {
        //设置标题文字和图标为不可见
        window.titleVisibility = NSWindowTitleHidden;   //MAC_10_10及以上版本支持
        //设置标题栏为透明
        window.titlebarAppearsTransparent = YES;        //MAC_10_10及以上版本支持
        //设置不可由标题栏拖动,避免与自定义拖动冲突
        [window setMovable:NO];                         //MAC_10_6及以上版本支持

        //window.movableByWindowBackground = YES;
        //设置view扩展到标题栏
        window.styleMask |=  NSWindowStyleMaskFullSizeContentView; //MAC_10_10及以上版本支持
        this->move(this->x(), this->y() - 22);
    }
    else
    {
        window.titleVisibility = NSWindowTitleVisible;
        window.titlebarAppearsTransparent = NO;
        [window setMovable:YES];
        window.styleMask &= ~NSWindowStyleMaskFullSizeContentView;
        this->move(this->x(), this->y() + 22);
    }
//    ButtonPasser * passer = [[ButtonPasser alloc] init];
//    passer.window = this;
//    //重载全屏按钮的行为
//    //override the action of fullscreen button
//    NSButton *zoomButton = [window standardWindowButton:NSWindowZoomButton];
//    [zoomButton setTarget:passer];
//    [zoomButton setAction:@selector(zoomButtonAction:)];
}

