/* 鼠标按钮 */
#ifndef _KGC_DESKTOP_H
#define _KGC_DESKTOP_H

#include <defs.h>
#include <container/container.h>
#include <kgceven.h>

typedef struct KGC_Desktop {
    KGC_Container_t *container;         /* 容器 */

} KGC_Desktop_t;

extern KGC_Desktop_t kgcDesktop;
#if 1
int KGC_DesktopKeyUp(KGC_KeyboardEven_t *even);
int KGC_DesktopKeyDown(KGC_KeyboardEven_t *even);
void KGC_DesktopMouseMotion(int mx, int my);
void KGC_DesktopMouseUp(int button, int mx, int my);
void KGC_DesktopMouseDown(int button, int mx, int my);
void KGC_DesktopDoTimer(int ticks);
#endif

int KGC_InitDesktopContainer();

#endif   /* _KGC_DESKTOP_H */
