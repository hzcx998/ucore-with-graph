/* 系统内核 */
#include <window/window.h>
#include <window/draw.h>
#include <bar/bar.h>
#include <container/container.h>
#include <stdio.h>

extern struct list_entry windowListHead;

/**
 * KGC_WindowSwitch - 切换窗口
 * @window: 窗口
 * 
 * 切换到指定窗口
 */
void KGC_SwitchWindow(KGC_Window_t *window)
{
    if (!window)
        return;
    
    /* 当前窗口状态设为非激活状态 */
    if (GET_CURRENT_WINDOW())
        KGC_WindowPaintActive(GET_CURRENT_WINDOW(), 0);

    /* 切换到的窗口设为激活状态 */
    KGC_WindowPaintActive(window, 1);

    /* 切换后地窗口放到顶部下面 */
    KGC_ContainerBelowTopZ(window->container);
    /* 设置当前窗口 */
    SET_CURRENT_WINDOW(window);
    /* 激活控制窗口 */
    KGC_WindowControllerActive(KGC_WindowControllerGet(window));

}

/**
 * KGC_SwitchFirstWindow - 切换到第一个窗口
 * 
 */
void KGC_SwitchFirstWindow()
{
    /* 获取首窗口 */
    KGC_Window_t *window = list_first_owner_or_null(&windowListHead, KGC_Window_t, list);
    /* 切换窗口，如果是NULL也没问题 */
    KGC_SwitchWindow(window);
}

/**
 * KGC_SwitchLastWindow - 切换到最后一个窗口
 * 
 */
void KGC_SwitchLastWindow()
{
    /* 获取首窗口 */
    KGC_Window_t *window = list_last_owner_or_null(&windowListHead, KGC_Window_t, list);
    /* 如果相同就不切换 */
    if (currentWindow == window)
        return;
    /* 切换窗口，如果是NULL也没问题 */
    KGC_SwitchWindow(window);
}

/**
 * KGC_SwitchNextWindow - 切换到下一个窗口
 * @window: 当前窗口
 * 
 */
void KGC_SwitchNextWindow(KGC_Window_t *window)
{
    if (!window)
        return;
    
    /* 获取下个窗口 */
    KGC_Window_t *win;
    struct list_entry *list, *tmp = window->list.next;
    do {
        list = tmp;
        tmp = tmp->next;
    } while (list == &windowListHead);
    
    win = list_owner(list, struct KGC_Window, list);

    /* 如果相同就不切换 */
    if (win == window)
        return;
    /* 切换窗口，如果是NULL也没问题 */
    KGC_SwitchWindow(win);
}
/**
 * KGC_SwitchNextWindow - 切换到上一个窗口
 * @window: 当前窗口
 * 
 */
void KGC_SwitchPrevWindow(KGC_Window_t *window)
{
    if (!window)
        return;
    
    /* 获取下个窗口 */
    KGC_Window_t *win;
    struct list_entry *list, *tmp = window->list.prev;
    do {
        list = tmp;
        tmp = tmp->prev;
    } while (list == &windowListHead);
    
    win = list_owner(list, struct KGC_Window, list);

    /* 如果相同就不切换 */
    if (win == window)
        return;
    /* 切换窗口，如果是NULL也没问题 */
    KGC_SwitchWindow(win);
}

/**
 * KGC_SwitchNextWindowAuto - 切换到下一个自动选择的窗口
 * 
 */
void KGC_SwitchNextWindowAuto()
{
    /* 如果当前窗口是空，说明有可能是才执行完关闭窗口，被置空 */
    if (currentWindow == NULL) {
        /* 如果还有窗口，那么就可以进行切换 */
        if (list_length(&windowListHead) > 0)
            KGC_SwitchFirstWindow();
        return;
    }

    KGC_Window_t *win;
    struct list_entry *list, *tmp = currentWindow->list.next;
    do {
        list = tmp;
        tmp = tmp->next;
    } while (list == &windowListHead);
    
    win = list_owner(list, struct KGC_Window, list);
    /*
    cprintf("switch from %x to %x\n", currentWindow, win);
    cprintf("window %d:%d, %d:%d\n", win->x, win->y, win->width, win->height);
    KGC_View_t *v = win->view;
    cprintf("view %x %d:%d, %d:%d\n", v, v->x, v->y, v->width, v->height);
    */
    if (win == currentWindow) { /* 说明没有其它窗口 */
        return;
    }
    /* 切换窗口，如果是NULL也没问题 */
    KGC_SwitchWindow(win);
}


/**
 * KGC_SwitchTopWindow - 切换最顶层窗口
 * 
 */
int KGC_SwitchTopWindow()
{
    /* 找到最顶层窗口,-1是鼠标层 */
    KGC_Container_t *container = KGC_ContainerFindByOffsetZ(-2);
    if (container == NULL) 
        return -1;
    
    if (!container->private) {
        /* 屏幕上已经没有窗口 */
        return -1;
    }

    KGC_Window_t *win;
    list_for_each_owner (win, &windowListHead, list) {
        /* 如果容器一样，说明找到 */
        if (win->container == container) {
    
            /* 如果相同就不切换 */
            if (win == currentWindow)
                return 0;
            /* 切换窗口，如果是NULL也没问题 */
            KGC_SwitchWindow(win);

            break;
        }
    }
    return 0;
}

/**
 * KGC_SwitchNextWindowAuto - 切换到上一个自动选择的窗口
 * 
 */
void KGC_SwitchPrevWindowAuto()
{
    /* 如果当前窗口是空，说明有可能是才执行完关闭窗口，被置空 */
    if (currentWindow == NULL) {
        cprintf("current window is null\n");
        /* 如果还有窗口，那么就可以进行切换 */
        if (list_length(&windowListHead) > 0)
            KGC_SwitchLastWindow();
        return;
    } 

    /* 获取上个窗口 */
    KGC_Window_t *win;
    struct list_entry *list, *tmp = currentWindow->list.prev;
    do {
        list = tmp;
        tmp = tmp->prev;
    } while (list == &windowListHead);
    
    win = list_owner(list, struct KGC_Window, list);
    /*
    cprintf("switch from %x to %x\n", currentWindow, win);
    cprintf("window %d:%d, %d:%d\n", win->x, win->y, win->width, win->height);
    */
    /* 如果相同就不切换 */
    if (win == currentWindow)
        return;
    /* 切换窗口，如果是NULL也没问题 */
    KGC_SwitchWindow(win);
}
