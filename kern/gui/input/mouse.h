#ifndef _KGC_INPUT_MOUSE_H
#define _KGC_INPUT_MOUSE_H

#include <defs.h>
#include <color.h>
#include <kgceven.h>
#include <container/container.h>

/* 鼠标填充色 */
#define KGC_MOUSE_FILL_COLOR    KGCC_BLACK
/* 鼠标边框色 */
#define KGC_MOUSE_BORDER_COLOR  KGCC_WHITE

/* 鼠标唯一色，对于唯一色，不会进行写入视图 */
#define KGC_MOUSE_UNIQUE_COLOR  KGCC_ARGB(0,0,0,0)

/* 鼠标视图大小是16*16 */
#define KGC_MOUSE_CONTAINER_SIZE     16

/* 鼠标 */
struct KGC_Mouse {
    int x, y;               /* 鼠标位置 */
    int button;             /* 按钮 */
    KGC_Container_t *container;       /* 视图 */
    uint32_t *bmp;          /* 位图数据 */   
};

extern struct KGC_Mouse mouse;

void KGC_MouseMotion(KGC_MouseMotionEven_t *motion);
void KGC_MouseDown(KGC_MouseButtonEven_t *button);
void KGC_MouseUp(KGC_MouseButtonEven_t *button);
int KGC_InitMouseContainer();

#endif   /* _KGC_INPUT_MOUSE_H */
