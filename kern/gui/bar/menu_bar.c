
/* 系统内核 */

#include <window/window.h>
#include <window/message.h>
#include <bar/menubar.h>
#include <bar/bar.h>
//#include <input/mouse.h>
#include <container/draw.h>
#include <string.h>
#include <stdio.h>

extern KGC_Bar_t kgcbar;

void KGC_MenuBarHandler(KGC_Button_t *button)
{
    
}

/** 
 * KGC_MenuBarUpdateTime - 更新时间显示
 * 
 */
void KGC_MenuBarUpdateTime()
{
#if 0
    /* 先将原来的位置清空 */
    KGC_ContainerDrawRectangle(kgcbar.container, kgcbar.time->label.x,
        kgcbar.time->label.y, kgcbar.time->label.width, 
        kgcbar.time->label.height, KGC_MENU_BAR_COLOR);

    /* 解析时间 */
    char buf[6];
    memset(buf, 0, 6);
    sprintf(buf, "%d:%d", systemDate.hour, systemDate.minute);
    /* 设置文本，然后显示 */
    KGC_ButtonSetText(kgcbar.time, buf);
    KGC_ButtonShow((KGC_Widget_t *)kgcbar.time);
#endif
}

int KGC_InitMenuBar()
{
     
    kgcbar.time = KGC_CreateButton();
    if (kgcbar.time == NULL) {
        cprintf("create button failed!\n");
    } 
#if 0  
    KGC_ButtonSetLocation(kgcbar.time, kgcbar.container->width - 6*8, 0);
    KGC_ButtonSetSize(kgcbar.time, 6*8, 24);
  
    KGC_ButtonSetText(kgcbar.time, "0: 0");

    KGC_ButtonSetName(kgcbar.time, "time");
    KGC_ButtonAdd(kgcbar.time, kgcbar.container);
    KGC_ButtonSetHandler(kgcbar.time, &KGC_MenuBarHandler);
    KGC_ButtonShow(&kgcbar.time->label.widget);
#endif
    return 0;
}
    