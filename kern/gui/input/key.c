/* 系统内核 */
#include <window/window.h>
#include <window/even.h>
#include <bar/bar.h>
#include <desktop/desktop.h>

void KGC_KeyDown(KGC_KeyboardEven_t *even)
{
    /* 如果已经执行了系统快捷键，就直接返回，不往后面继续监测 */
    if (KGC_BarKeyDown(even)) 
        return;
    /* 发送到窗口 */
    KGC_WindowKeyDown(even);
    /* 发送到桌面 */
    KGC_DesktopKeyDown(even);
}

void KGC_KeyUp(KGC_KeyboardEven_t *even)
{
    /* 如果已经执行了系统快捷键，就直接返回，不往后面继续监测 */
    if (KGC_BarKeyUp(even)) 
        return;
    /* 发送到窗口 */
    
    KGC_WindowKeyUp(even);
    /* 发送到桌面 */
    KGC_DesktopKeyUp(even);
}
 