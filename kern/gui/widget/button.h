/* 窗口控件之按钮 */
#ifndef _KGC_WIDGET_BUTTON_H
#define _KGC_WIDGET_BUTTON_H

#include <defs.h>
#include <color.h>
#include <list.h>
#include <font/font.h>
#include <widget/label.h>

#define KGC_BUTTON_DEFAULT      0   /* 按钮默认状态 */
#define KGC_BUTTON_FOCUS        1   /* 按钮聚焦状态 */
#define KGC_BUTTON_SELECTED     2   /* 按钮选择状态 */

/* 按钮默认颜色 */
#define KGC_BUTTON_DEFAULT_COLOR    KGCC_ARGB(255, 50, 50, 50)
#define KGC_BUTTON_FOCUS_COLOR      KGCC_ARGB(255, 200, 200, 200)
#define KGC_BUTTON_SELECTED_COLOR   KGCC_ARGB(255, 100, 100, 100)

typedef struct KGC_Button {
    KGC_Label_t label;      /* 继承标签：第一个成员 */
    int state;              /* 按钮状态：默认，聚焦，点击 */
    kgcc_t defaultColor;    /* 默认颜色 */
    kgcc_t focusColor;      /* 聚焦颜色 */
    kgcc_t selectedColor;   /* 选择颜色 */
    void (*handler) (struct KGC_Button *button);

} KGC_Button_t;

typedef void (*KGC_ButtonHandler_t) (struct KGC_Button *button);

KGC_Button_t *KGC_CreateButton();
int KGC_ButtonInit(KGC_Button_t *button);
void KGC_ButtonSetLocation(KGC_Button_t *button, int x, int y);
void KGC_ButtonSetSize(KGC_Button_t *button, int width, int height);
void KGC_ButtonSetName(KGC_Button_t *button, char *name);
int  KGC_ButtonSetTextMaxLength(KGC_Button_t *button, int length);
void KGC_ButtonSetText(KGC_Button_t *button, char *text);
void KGC_ButtonAdd(KGC_Button_t *button, KGC_Container_t *container);
void KGC_ButtonShow(KGC_Widget_t *widget);
void KGC_ButtonSetHandler(KGC_Button_t *button, KGC_ButtonHandler_t handler);
void KGC_ButtonSetImage(KGC_Button_t *button, int width,
    int height, uint8_t *raw, kgcc_t border, kgcc_t fill);
void KGC_ButtonDel(KGC_Button_t *button);

void KGC_ButtonDestroySub(KGC_Button_t *button);
void KGC_ButtonDestroy(KGC_Button_t *button);
void KGC_ButtonSetImageAlign(KGC_Button_t *button, KGC_WidgetAlign_t align);

#endif   /* _KGC_WIDGET_BUTTON_H */
