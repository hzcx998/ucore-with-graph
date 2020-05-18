#include <kmalloc.h>
#include <list.h>
#include <container/container.h>
#include <font/font.h>
#include <kgceven.h>
#include <input/input.h>
#include <clock.h>
#include <string.h>
#include <proc.h>
#include <console.h>
#include <stdio.h>
#include <core/handler.h>
#include <core/kgc.h>
/**
 * KGC_AllocBuffer - 分配缓冲区
 * @size: 缓冲区大小
 * 
 * 根据大小选择不同的分配方式
 * 
 * 成功返回缓冲区地址，失败返回NULL
 */
void *KGC_AllocBuffer(size_t size)
{
    return kmalloc(size);
}
#if 1
/**
 * KGC_KeyboardInput - 图形核心键盘输入
 */
void KGC_KeyboardInput(uint32_t key)
{
    /* 指向输入指针 */
    KGC_Even_t even;

    memset(&even, 0, sizeof(KGC_Even_t));
    /* 对原始数据进行处理，生成通用按键。按键按下，按键弹起，组合按键 */
    KGC_EvenKeyboardKey(key, &even.key);
    KGC_EvenHandler(&even);
}

/*c =  cons_getc()
 */

/**
 * KGC_TimerOccur - 时钟产生
 * 
 */
void KGC_TimerOccur()
{
    /* 指向输入指针 */
    KGC_Even_t even;

    memset(&even, 0, sizeof(KGC_Even_t));
        
    /* 时钟处理 */
    KGC_EvenTimer(ticks, &even.timer);
    KGC_EvenHandler(&even);
            
}

/**
 * KGC_MouseInput - 处理鼠标输入
 * 
 */
void KGC_MouseInput(InputBufferMouse_t *packet)
{
    /* 指向输入指针 */
    KGC_Even_t even;
    /* 鼠标移动事件及其处理 */
    memset(&even, 0, sizeof(KGC_Even_t));
    KGC_EvenMouseMotion(packet->xIncrease, packet->yIncrease, packet->zIncrease, &even.motion);
    KGC_EvenHandler(&even);

    memset(&even, 0, sizeof(KGC_Even_t));
    /* 鼠标按钮事件及其处理 */
    KGC_EvenMouseButton(packet->button, &even.button);
    KGC_EvenHandler(&even);
}
#endif
/**
 * InitKGC() - 初始化内核图形核心
 * 
 */
int InitKGC()
{
    /* 初始化字体 */
    KGC_InitFont();    
    /* 初始化容器 */
    KGC_InitContainer();

    return 0;
}
