
/* 系统内核 */
#include <bar.h>
#include <vbe.h>
#include <container/container.h>
#include <container/draw.h>
#include <input/keycode.h>
#include <stdio.h>

KGC_Bar_t kgcbar;

/*
系统快捷键：
[可屏蔽] alt + tab, alt + shift + tab:
    切换任务
[不屏蔽] ctl + alt + delete:
    打开系统管理界面
*/


void KGC_BarDoTimer(int ticks)
{
    /*  */
    //cprintf("bar timer!\n");
    /* 更新时间 */
    KGC_MenuBarUpdateTime();   
}

void KGC_BarMouseDown(int button, int mx, int my)
{
    //cprintf("bar mouse down\n");
    /* 控件检测 */
    KGC_WidgetMouseButtonDown(&kgcbar.container->widgetListHead, button, mx, my);
    
}

void KGC_BarMouseUp(int button, int mx, int my)
{
    //cprintf("bar mouse up\n");
    /* 控件检测 */
    KGC_WidgetMouseButtonUp(&kgcbar.container->widgetListHead, button, mx, my);

}

void KGC_BarMouseMotion(int mx, int my)
{
    //cprintf("bar mouse motion\n");
    KGC_WidgetMouseMotion(&kgcbar.container->widgetListHead, mx, my);

}
#if 1
int KGC_BarKeyDown(KGC_KeyboardEven_t *even)
{
    //cprintf("bar key down\n");
    /* 1.如果是不可屏蔽快捷键，处理后就立即返回不再继续处理按键的条件。 */
    if (even->keycode.code == IKEY_DELETE) {
        /* CTRL + ALT + DELETE: 打开系统管理界面 */
        if ((even->keycode.modify & KGC_KMOD_ALT) && 
            (even->keycode.modify & KGC_KMOD_CTRL)) {
            /* 系统管理界面 */
            
            return -1;  
        }
    }
    
    /* 2.如果是可屏蔽快捷键，查看屏蔽标志，根据屏蔽标志来判断是否继续处理按键 */
    
    if (even->keycode.code == IKEY_TAB) {
        /* CTRL + SHIFT + TAB: 向前切换任务 */
        if ((even->keycode.modify & KGC_KMOD_ALT) && 
            (even->keycode.modify & KGC_KMOD_SHIFT)) {
            if (GET_CURRENT_WINDOW()) {
                /* 查看当前窗口的可否获取系统快捷键 */
                if (GET_CURRENT_WINDOW()->flags & KGC_WINDOW_FLAG_KEY_SHORTCUTS) 
                    return 0;
                
            } else {
                /* 没有屏蔽才能进行操作 */
                KGC_SwitchPrevWindowAuto();
                return -1;
            }
                
        /* CTRL + TAB: 向后切换任务 */
        } else if (even->keycode.modify & KGC_KMOD_ALT) {
            if (GET_CURRENT_WINDOW()) {
                /* 查看当前窗口的可否获取系统快捷键 */
                if (GET_CURRENT_WINDOW()->flags & KGC_WINDOW_FLAG_KEY_SHORTCUTS) 
                    return 0;
                
            } else {
                /* 没有屏蔽才能进行操作 */
                KGC_SwitchNextWindowAuto();
                return -1;
            }
        }
    } else if (even->keycode.code == IKEY_ESCAPE) {
        if (GET_CURRENT_WINDOW()) {
            
            /* 查看当前窗口的可否获取系统快捷键 */
            if (GET_CURRENT_WINDOW()->flags & KGC_WINDOW_FLAG_KEY_SHORTCUTS)
                return 0;
            
            /* 没有屏蔽才能进行操作 */
            KGC_SwitchNextWindowAuto();
        } else {
            /* 没有屏蔽才能进行操作 */
            KGC_SwitchNextWindowAuto();
            return -1;
        }
    }
    /* 3.非系统快捷键，返回继续处理按键 */
    return 0;
}

int KGC_BarKeyUp(KGC_KeyboardEven_t *even)
{
    //cprintf("bar key up\n");
    /* 1.如果是不可屏蔽快捷键，处理后就立即返回不再继续处理按键的条件。 */
    if (even->keycode.code == IKEY_DELETE) {
        /* CTRL + ALT + DELETE: 打开系统管理界面 */
        if ((even->keycode.modify & KGC_KMOD_ALT) && 
            (even->keycode.modify & KGC_KMOD_CTRL)) {
            /* 系统管理界面 */
            
            return -1;  
        }
    }

    /* 2.如果是可屏蔽快捷键，查看屏蔽标志，根据屏蔽标志来判断是否继续处理按键 */
    if (even->keycode.code == IKEY_TAB) {
        /* CTRL + SHIFT + TAB: 向前切换任务 */
        if ((even->keycode.modify & KGC_KMOD_ALT) && 
            (even->keycode.modify & KGC_KMOD_SHIFT)) {
            if (GET_CURRENT_WINDOW()) {   
        
                /* 查看当前窗口的可否获取系统快捷键 */
                if (GET_CURRENT_WINDOW()->flags & KGC_WINDOW_FLAG_KEY_SHORTCUTS)
                    return 0;
            } else {
                /* 没有屏蔽才能进行操作 */
                return -1;
            }
                
        /* CTRL + TAB: 向后切换任务 */
        } else if (even->keycode.modify & KGC_KMOD_ALT) {
            if (GET_CURRENT_WINDOW()) {   
        
                /* 查看当前窗口的可否获取系统快捷键 */
                if (GET_CURRENT_WINDOW()->flags & KGC_WINDOW_FLAG_KEY_SHORTCUTS)
                    return 0;
            } else {
                /* 没有屏蔽才能进行操作 */
                return -1;
            }
        }
    } else if (even->keycode.code == IKEY_SPACE) {
        if (GET_CURRENT_WINDOW()) {   
        
            /* 查看当前窗口的可否获取系统快捷键 */
            if (GET_CURRENT_WINDOW()->flags & KGC_WINDOW_FLAG_KEY_SHORTCUTS)
                return 0;
        } else {
            /* 没有屏蔽才能进行操作 */
            return -1;
        }
    } else if (even->keycode.code == IKEY_ENTER) {
        if (GET_CURRENT_WINDOW()) {   
            /* 查看当前窗口的可否获取系统快捷键 */
            if (GET_CURRENT_WINDOW()->flags & KGC_WINDOW_FLAG_KEY_SHORTCUTS)
                return 0;
        } else {
            /* 没有屏蔽才能进行操作 */
            return -1;
        }
    }
    /* 3.非系统快捷键，返回继续处理按键 */
    return 0;   
}

#endif
void KGC_DrawBar(KGC_Container_t *container)
{
    KGC_ContainerDrawRectangle(container, 0, 0, 
        container->width, KGC_MENU_BAR_HEIGHT, KGC_MENU_BAR_COLOR);

    KGC_ContainerDrawRectangle(container, 0, KGC_MENU_BAR_HEIGHT, 
        container->width, KGC_TASK_BAR_HEIGHT, KGC_TASK_BAR_COLOR);

}
int KGC_InitBarContainer()
{
    /* 菜单栏 */
    kgcbar.container = KGC_ContainerAlloc();
    if (kgcbar.container == NULL)
        return -1;
    KGC_ContainerInit(kgcbar.container, "bar", 0, 0, videoInfoPtr->xResolution, KGC_BAR_HEIGHT, NULL);

    KGC_DrawBar(kgcbar.container);

    KGC_ContainerAtTopZ(kgcbar.container);

    KGC_InitMenuBar();
    KGC_InitTaskBar();
    return 0;
}
    