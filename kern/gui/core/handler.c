/*
 * file:		kernel/kgc/core/handler.c
 * auther:		Jason Hu
 * time:		2020/2/6
 * copyright:	(C) 2018-2020 by Book OS developers. All rights reserved.
 */
/* KGC-kernel graph core 内核图形核心 */

/* 图形核心事件处理 */

/* 图形系统 */
#include <core/draw.h>
#include <kgceven.h>
#include <button.h>

#include <input/mouse.h>
#include <input/key.h>
#include <window/window.h>
#include <window/even.h>
#include <bar/bar.h>

/**
 * KGC_DoTimer- 时间处理
 * 
 */
static void KGC_DoTimer(KGC_TimerEven_t *even)
{
    KGC_BarDoTimer(even->ticks);
    KGC_WindowDoTimer(even->ticks);
}
        
/**
 * KGC_EvenHandler - 图形事件处理
 * @even: 鼠标按钮事件
 */
void KGC_EvenHandler(KGC_Even_t *even)
{   
    /* 通用处理 */
    switch (even->type)
    {
    case KGC_KEY_DOWN:
        /* 处理具体的按键 */
        KGC_KeyDown(&even->key);
        break;
    case KGC_KEY_UP:
        /* 处理具体的按键 */
        KGC_KeyUp(&even->key);
        break;
    case KGC_MOUSE_BUTTON_DOWN:
        if (even->button.button & KGC_MOUSE_LEFT) {
            /* 按钮只有鼠标左 */
            even->button.button = KGC_MOUSE_LEFT;
            KGC_MouseDown(&even->button);
            //printk("mouse left down\n");
        }
        if (even->button.button & KGC_MOUSE_RIGHT) {
            /* 按钮只有鼠标右 */
            even->button.button = KGC_MOUSE_RIGHT;
            
            //printk("mouse right down\n");
            KGC_MouseDown(&even->button);
        }
        if (even->button.button & KGC_MOUSE_MIDDLE) {
            even->button.button = KGC_MOUSE_MIDDLE;
            KGC_MouseDown(&even->button);
            //printk("mouse middle down\n");
        }
        break;
    case KGC_MOUSE_BUTTON_UP:
        if (even->button.button == KGC_MOUSE_LEFT) {
            //printk("mouse left up\n");
            KGC_MouseUp(&even->button);
        }
        if (even->button.button == KGC_MOUSE_RIGHT) {
            KGC_MouseUp(&even->button);
            //printk("mouse right up\n");
        }
        if (even->button.button == KGC_MOUSE_MIDDLE) {
            KGC_MouseUp(&even->button);
            //printk("mouse middle up\n");
        }
        break;
    case KGC_MOUSE_MOTION:
        KGC_MouseMotion(&even->motion);
        break;
    case KGC_TIMER_EVEN:
        KGC_DoTimer(&even->timer);
        break;
        
    default:
        break;
    }

}
