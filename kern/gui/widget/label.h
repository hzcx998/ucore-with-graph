/* 窗口控件之标签 */
#ifndef _KGC_WIDGET_LABEL_H
#define _KGC_WIDGET_LABEL_H

#include <defs.h>
#include <color.h>
#include <list.h>
#include <widget/widget.h>
#include <font/font.h>
#include <container/container.h>
#include <widget/image.h>

/* 标签名长度 */
#define KGC_LABEL_NAME_LEN     24

/* 默认文本长度 */
#define KGC_MAX_LABEL_TEXT_LEN     255
#define KGC_DEFAULT_LABEL_TEXT_LEN     32

/* 标签默认颜色 */
#define KGC_LABEL_BACK_COLOR    KGCC_ARGB(255, 50, 50, 50)
#define KGC_LABEL_FONT_COLOR    KGCC_WHITE
#define KGC_LABEL_DISABEL_COLOR KGCC_ARGB(255, 192, 192, 192)

/* 默认宽高 */
#define KGC_DEFAULT_LABEL_WIDTH         40
#define KGC_DEFAULT_LABEL_HEIGHT        20

typedef enum KGC_LabelTypes {
    KGC_LABEL_TEXT  = 0,    /* 文本标签 */
    KGC_LABEL_IMAGE,        /* 图像标签 */
    KGC_LABEL_PICTURE,      /* 图片标签 */
    KGC_LABEL_GRAPH         /* 图形标签 */
} KGC_LabelTypes_t;

typedef struct KGC_Label {
    KGC_Widget_t widget;            /* 继承控件：第一个成员 */
    int x, y;                       /* 位置属性 */
    int width, height;              /* 大小属性 */
    char visable;                   /* 可见 */
    char disabel;                   /* 禁止 */
    
    char name[KGC_LABEL_NAME_LEN];  /* 标签名 */
    uint8_t nameLength;             /* 名字长度 */
    /* 颜色 */
    kgcc_t backColor;               /* 背景色 */
    kgcc_t fontColor;               /* 字体色 */
    kgcc_t disableColor;            /* 禁止字体色 */
    
    /* 字体 */
    KGC_Font_t *font;

    /* 标签的内容 */
    KGC_LabelTypes_t type;          /* 标签类型 */
    char *text;                     /* 文本 */
    uint8_t textLength;             /* 文本长度 */
    uint8_t textMaxLength;          /* 文本最大长度 */
    KGC_WidgetAlign_t textAlign;    /* 文本对齐 */

    KGC_Image_t image;              /* 图像 */
    KGC_WidgetAlign_t imageAlign;   /* 图像对齐 */

    /* 图形 */
} KGC_Label_t;

KGC_Label_t *KGC_CreateLabel();
int KGC_LabelInit(KGC_Label_t *label);
void KGC_LabelSetLocation(KGC_Label_t *label, int x, int y);
void KGC_LabelSetSize(KGC_Label_t *label, int width, int height);
void KGC_LabelSetColor(KGC_Label_t *label, kgcc_t back, kgcc_t font);
void KGC_LabelSetName(KGC_Label_t *label, char *name);
int KGC_LabelSetTextMaxLength(KGC_Label_t *label, int length);
void KGC_LabelSetText(KGC_Label_t *label, char *text);
int KGC_LabelSetFont(KGC_Label_t *label, char *fontName);
void KGC_LabelAdd(KGC_Label_t *label, KGC_Container_t *container);
void KGC_LabelShow(KGC_Widget_t *widget);
void KGC_LabelSetImage(KGC_Label_t *label, int width,
    int height, uint8_t *raw, kgcc_t border, kgcc_t fill);
void KGC_LabelDestroy(KGC_Label_t *label);
void KGC_LabelDestroySub(KGC_Label_t *label);
void KGC_LabelSetImageAlign(KGC_Label_t *label, KGC_WidgetAlign_t align);
void KGC_LabelCreateImage(KGC_Image_t *image);

#endif   /* _KGC_WIDGET_LABEL_H */
