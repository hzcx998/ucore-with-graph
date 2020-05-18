/* 图形容器 */
#ifndef _KGC_CONTAINER_H
#define _KGC_CONTAINER_H

#include <defs.h>
#include <list.h>

/* 容器名字长度 */
#define KGC_CONTAINER_NAME_LEN     32

/* 容器数量，最多255个 */
#define KGC_MAX_CONTAINER_NR     32

/* 容器的标志 */
#define KGC_CONTAINER_UNUSED  0X00
#define KGC_CONTAINER_USING   0X80000000

#define KGC_CONTAINER_BUTTOM   0X01     /*位于底层的容器*/
#define KGC_CONTAINER_TOP      0X02     /*位于底层的容器*/
#define KGC_CONTAINER_WINDOW   0X04     /*是一个窗口的容器*/
#define KGC_CONTAINER_FLOAT    0X08     /*是一个悬浮的容器*/
#define KGC_CONTAINER_FIXED    0X10     /*是一个固定的容器*/
#define KGC_CONTAINER_HIDE     0X20     /*是一个隐藏的容器*/

/* 一个像素使用的字节数 */
#define KGC_CONTAINER_BPP       4

/* 带透明色计算的刷新方法 */
#define KGC_CONTAINER_REFRESH_ALPHA 0

/* 容器 */
typedef struct KGC_Container {
    int x, y;                   /* 容器在屏幕上的位置 */
    int z;                      /* z轴序，多个容器叠加的顺序 */
    int width, height;          /* 容器的宽高 */
    int id;                     /* 容器的ID */
    char bytesPerPixel;         /* 每像素的字节数 */
    uint32_t *buffer;           /* 缓冲区 */
    uint32_t flags;             /* 标志 */
    void *private;              /* 容器私有数据 */
    /* 控件链表 */
    struct list_entry widgetListHead; /* 控件链表头 */
    char name[KGC_CONTAINER_NAME_LEN]; /* 缓冲区名 */    
} KGC_Container_t;

typedef struct KGC_ContainerManager {
    uint8_t *map;               /* 容器id的地图 */
    uint32_t *buffer;           /* 缓冲区 */
    int width, height;          /* 最大的容器 */
    int top;                    /* 最顶层的容器 */
    struct KGC_Container containerTable[KGC_MAX_CONTAINER_NR];  /* 容器表 */
    struct KGC_Container *containerPtr[KGC_MAX_CONTAINER_NR];   /* 容器指针表 */
    struct KGC_Container *currentContainer;                     /* 当前容器 */
} KGC_ContainerManager_t;

void KGC_InitContainer();
KGC_Container_t *KGC_ContainerAlloc();
void KGC_ContainerFree(KGC_Container_t *container);
void KGC_ContainerRefresh(KGC_Container_t *container, int x0, int y0, int x1, int y1);
void KGC_ContainerSlide(KGC_Container_t *container, int x, int y);
void KGC_ContainerZ(KGC_Container_t *container, int z);
void KGC_ContainerAboveTopZ(KGC_Container_t *container);
void KGC_ContainerBelowTopZ(KGC_Container_t *container);
void KGC_ContainerAtTopZ(KGC_Container_t *container);
KGC_Container_t *KGC_ContainerFindByZ(int z);
KGC_Container_t *KGC_ContainerFindByOffsetZ(int zoff);

int KGC_ContainerInit(KGC_Container_t *container,
    char *name,
    int x, int y,
    int width, int height,
    void *private);

#endif   /* _KGC_CONTAINER_H */
