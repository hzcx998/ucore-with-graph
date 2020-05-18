#include <desktop/desktop.h>
#include <container/draw.h>
#include <vbe.h>
#include <color.h>
#include <window/window.h>

#define KGC_DESKTOP_COLOR KGCC_ARGB(255, 60, 60, 60)

KGC_Desktop_t kgcDesktop;

/* 将输入转换成特殊的消息转发给当前窗口 */

void KGC_DesktopDoTimer(int ticks)
{
    /* 当前窗口为空才发送到桌面 */
    if (GET_CURRENT_WINDOW() == NULL) {
        //printk("desktop timer\n");    
    }
}

void KGC_DesktopMouseDown(int button, int mx, int my)
{
    /* 当前窗口为空才发送到桌面 */
    if (GET_CURRENT_WINDOW() == NULL) {
        //printk("desktop mouse down\n");    
    }
}

void KGC_DesktopMouseUp(int button, int mx, int my)
{
    /* 当前窗口为空才发送到桌面 */
    if (GET_CURRENT_WINDOW() == NULL) {
        //printk("desktop mouse up\n");    
    }
}

void KGC_DesktopMouseMotion(int mx, int my)
{
    /* 当前窗口为空才发送到桌面 */
    if (GET_CURRENT_WINDOW() == NULL) {
        //printk("desktop mouse motion\n");    
    }
}

/**
 * KGC_DesktopKeyDown - 桌面按键按下
 * @even: 按键事件
 * 
 */
int KGC_DesktopKeyDown(KGC_KeyboardEven_t *even)
{
    /* 当前窗口为空才发送到桌面 */
    if (GET_CURRENT_WINDOW() == NULL) {
        //printk("desktop key down\n");    
    }
    return 0;
}

int KGC_DesktopKeyUp(KGC_KeyboardEven_t *even)
{
    /* 当前窗口为空才发送到桌面 */
    if (GET_CURRENT_WINDOW() == NULL) {
        //printk("desktop key up\n");    
    }
    return 0;
}

int KGC_InitDesktopContainer()
{
    /* 桌面容器是第一个容器 */
    kgcDesktop.container = KGC_ContainerAlloc();
    if (kgcDesktop.container == NULL)
        panic("alloc memory for container failed!\n");
    KGC_ContainerInit(kgcDesktop.container, "desktop", 0, 0,
        videoInfoPtr->xResolution, videoInfoPtr->yResolution, NULL);    
    KGC_ContainerDrawRectangle(kgcDesktop.container, 0, 0, 
        videoInfoPtr->xResolution, videoInfoPtr->yResolution, KGC_DESKTOP_COLOR);
    KGC_ContainerZ(kgcDesktop.container, 0);
    cprintf("init desktop done.");

    KGC_Label_t label;
    KGC_LabelInit(&label);
    KGC_LabelSetLocation(&label, 100,100);
    KGC_LabelSetSize(&label, 100, 20);
    KGC_LabelSetText(&label, "hello, ucore");
    KGC_LabelAdd(&label, kgcDesktop.container);
    KGC_LabelShow(&label.widget);

    return 0;
}