/* 鼠标按钮 */
#ifndef _KGC_BUTTON_H
#define _KGC_BUTTON_H

#include <defs.h>
/* KGC-kernel graph core 内核图形核心 */

/* 鼠标按钮 */
enum KGC_MouseButton {
    KGC_MOUSE_LEFT        = 0x01, /* 鼠标左键 */
    KGC_MOUSE_RIGHT       = 0x02, /* 鼠标右键 */
    KGC_MOUSE_MIDDLE      = 0x04, /* 鼠标中键 */
};

#endif   /* _KGC_BUTTON_H */
