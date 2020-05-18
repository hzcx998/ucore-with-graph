/* 窗口控件之标签 */
#ifndef _KGC_WIDGET_H
#define _KGC_WIDGET_H

#include <defs.h>
#include <list.h>
#include <color.h>
#include <container/container.h>
#include <container/draw.h>

/* 控件的类型 */
typedef enum KGC_WidgetTypes {
    KGC_WIDGET_LABEL = 0,       /* 标签控件 */
    KGC_WIDGET_BUTTON,          /* 按钮控件 */
} KGC_WidgetType_t;

/* 控件对齐方式 */
typedef enum KGC_WidgetAlign {
    KGC_WIDGET_ALIGN_LEFT = 0,  /* 左对齐 */
    KGC_WIDGET_ALIGN_CENTER,    /* 居中对齐 */
    KGC_WIDGET_ALIGN_RIGHT,     /* 右对齐 */
} KGC_WidgetAlign_t;

/* 控件结构体 */
typedef struct KGC_Widget {
    struct list_entry list;           /* 控件组成的链表 */
    uint8_t type;               /* 控件类型 */
    KGC_Container_t *container; /* 控件所在的容器 */
    void (*draw) (struct KGC_Widget *widget);   /* 控件绘图指针 */
    uint8_t drawCounter;                /* 计数器：为0时才绘图，不然则不会图 */
    
    /* 鼠标事件 */
    void (*mouseButtonDown) (struct KGC_Widget *widget, int button, int mx, int my);
    void (*mouseButtonUp) (struct KGC_Widget *widget,  int button, int mx, int my);
    void (*mouseMotion) (struct KGC_Widget *widget, int mx, int my);

} KGC_Widget_t;

/* 控件绘图指针 */
typedef void (*KGC_WidgetDraw_t) (KGC_Widget_t *widget);
typedef void (*KGC_WidgetMouseButton_t) (KGC_Widget_t *widget, int button, int mx, int my);
typedef void (*KGC_WidgetMouseMotion_t) (KGC_Widget_t *widget, int mx, int my);

static inline void KGC_WidgetInit(KGC_Widget_t *widget,
    KGC_WidgetType_t type)
{
    list_init(&widget->list);
    widget->type = type;
    widget->draw = NULL;
    widget->drawCounter = 0;

    widget->mouseButtonDown = NULL;
    widget->mouseButtonUp = NULL;
    widget->mouseMotion = NULL;

} 

static inline void KGC_WidgetSetDraw(KGC_Widget_t *widget,
    KGC_WidgetDraw_t draw)
{
    widget->draw = draw;
} 

static inline void KGC_WidgetSetMouseEven(KGC_Widget_t *widget,
    KGC_WidgetMouseButton_t buttonDown,
    KGC_WidgetMouseButton_t buttonUp,
    KGC_WidgetMouseMotion_t motion)
{
    widget->mouseButtonDown = buttonDown;
    widget->mouseButtonUp = buttonUp;
    widget->mouseMotion = motion;
} 

static inline void KGC_AddWidget(KGC_Widget_t *widget, KGC_Container_t *container)
{
    /* 控件添加到容器 */
    list_add(&widget->list, &container->widgetListHead);
    widget->container = container;
} 

static inline void KGC_DelWidget(KGC_Widget_t *widget)
{
    /* 控件添加到容器 */
    list_del(&widget->list);
    widget->container = NULL;
} 

static inline void KGC_WidgetShow(KGC_Widget_t *widget)
{
    if (!widget->drawCounter) {
        /* 调用绘图 */
        if (widget->draw)
            widget->draw(widget);
        widget->drawCounter++;
    }
}

static inline void KGC_WidgetMouseButtonUp(struct list_entry *listHead, int button, int mx, int my)
{
    /* 控件检测 */
    KGC_Widget_t *widget;
    list_for_each_owner (widget, listHead, list) {
        if (widget->type == KGC_WIDGET_BUTTON) {
            if (widget->mouseButtonUp)
                widget->mouseButtonUp(widget, button, mx, my);
        }
        KGC_WidgetShow(widget);
    }
}


static inline void KGC_WidgetMouseButtonDown(struct list_entry *listHead, int button, int mx, int my)
{
    /* 控件检测 */
    KGC_Widget_t *widget;
    list_for_each_owner (widget, listHead, list) {
        if (widget->type == KGC_WIDGET_BUTTON) {
            if (widget->mouseButtonDown)
                widget->mouseButtonDown(widget, button, mx, my);
        }
        KGC_WidgetShow(widget);
    }
}

static inline void KGC_WidgetMouseMotion(struct list_entry *listHead, int mx, int my)
{
    /* 控件检测 */
    KGC_Widget_t *widget;
    list_for_each_owner (widget, listHead, list) {
        if (widget->type == KGC_WIDGET_BUTTON) {
            if (widget->mouseMotion)
                widget->mouseMotion(widget, mx, my);
        }
        
        KGC_WidgetShow(widget);
    }
}



#endif   /* _KGC_WIDGET_H */
