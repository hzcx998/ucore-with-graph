
/* 窗口控件之图像 */
#ifndef _KGC_WIDGET_IMAGE_H
#define _KGC_WIDGET_IMAGE_H

#include <defs.h>
#include <color.h>

/* kgc 图像 */
typedef struct KGC_Image {
    int width, height;  /* 图像大小 */
    uint32_t *data;     /* 图像数据 */
    uint8_t *raw;      /* 原生数据 */
    kgcc_t borderColor; /* 边框颜色 */
    kgcc_t fillColor;   /* 填充颜色 */
} KGC_Image_t;


#endif   /* _KGC_WIDGET_IMAGE_H */
