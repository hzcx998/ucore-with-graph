
#ifndef _KGC_BAR_H
#define _KGC_BAR_H

#include <defs.h>

#include <color.h>
#include <bar/taskbar.h>
#include <bar/menubar.h>
#include <container/container.h>
#include <widget/button.h>

#define KGC_MENU_BAR_COLOR KGCC_ARGB(255, 50, 50, 50) 

#define KGC_TASK_BAR_COLOR KGCC_ARGB(255, 80, 80, 80) 

/* bar的结构 */
typedef struct KGC_Bar {
    KGC_Container_t *container;
    KGC_Button_t *time;             /* 时间控件 */
} KGC_Bar_t;

extern KGC_Bar_t kgcbar;

/* 菜单栏和任务栏的高度 */
#define KGC_BAR_HEIGHT      (KGC_MENU_BAR_HEIGHT + KGC_TASK_BAR_HEIGHT)

int KGC_BarPollEven();

int KGC_InitBarContainer();

void KGC_BarMouseDown(int button, int mx, int my);
void KGC_BarMouseUp(int button, int mx, int my);
void KGC_BarMouseMotion(int mx, int my);
int KGC_BarKeyDown(KGC_KeyboardEven_t *even);
int KGC_BarKeyUp(KGC_KeyboardEven_t *even);
void KGC_BarDoTimer(int ticks);

#endif   /* _KGC_BAR_H */
