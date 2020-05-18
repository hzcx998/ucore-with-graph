/* 图形事件 */
#ifndef _KGC_EVEN_H
#define _KGC_EVEN_H
/* KGC-kernel graph core 内核图形核心 */

#include <defs.h>
#include <kcode.h>

/* 图形事件状态 */
enum KGC_EvenStates  {
    KGC_PRESSED   = 1,    /* 按下状态 */
    KGC_RELEASED,         /* 释放状态 */
};

/* 图形键盘输入 */
typedef struct KGC_KeyboardEven {
    uint8_t type;                /* 键盘事件类型：KGC_KEY_DOWN, KGC_KEY_UP */
    uint8_t state;               /* 按钮状态 */
    KGC_KeyInfo_t keycode;    /* 按键信息 */
} KGC_KeyboardEven_t;

/* 图形鼠标移动 */
typedef struct KGC_MouseMotionEven {
    uint8_t type;                /* 鼠标移动事件类型：KGC_MOUSE_MOTION */
    int32_t x;              /* x偏移 */
    int32_t y;              /* y偏移 */
} KGC_MouseMotionEven_t;

/* 图形鼠标按扭 */
typedef struct KGC_MouseButtonEven {
    uint8_t type;                /* 鼠标按钮事件类型：KGC_MOUSE_DOWN, KGC_MOUSE_UP */
    uint8_t state;               /* 按钮状态 */
    uint8_t button;              /* 按钮值 */
} KGC_MouseButtonEven_t;

/* 图形鼠标按扭 */
typedef struct KGC_TimerEven {
    uint8_t type;                /* 时间事件，每秒产生一次 */
    uint32_t ticks;               /* 事件产生时的ticks */
} KGC_TimerEven_t;

/* 图形的事件类型 */
typedef enum {
    KGC_NOEVENT = 0,      /* 未使用 */
    KGC_KEY_DOWN,         /* 按键按下 */
    KGC_KEY_UP,           /* 按键弹起 */
    KGC_MOUSE_MOTION,     /* 鼠标移动 */
    KGC_MOUSE_BUTTON_DOWN,/* 鼠标按钮按下 */
    KGC_MOUSE_BUTTON_UP,  /* 鼠标按钮弹起 */
    KGC_TIMER_EVEN,       /* 时钟事件 */
    MAX_KGC_EVENT_NR,     /* 最大的事件数量 */
} KGC_EvenType_t;

/* 图形输入
一个联合结构
 */
typedef union KGC_Even {
    uint8_t type;    /* 输入类型，由于下面每一个成员的第一个成员都是type，所以次type就是该成员的type */
    KGC_KeyboardEven_t key;
    KGC_MouseMotionEven_t motion;
    KGC_MouseButtonEven_t button;   
    KGC_TimerEven_t timer; 
} KGC_Even_t;

void KGC_EvenKeyboardKey(int key, KGC_KeyboardEven_t *even);
void KGC_EvenMouseMotion(int32_t x, int32_t y, int32_t z, KGC_MouseMotionEven_t *even);
void KGC_EvenMouseButton(uint8_t button, KGC_MouseButtonEven_t *even);
void KGC_EvenTimer(int32_t ticks, KGC_TimerEven_t *even);

#endif   /* _KGC_EVEN_H */
