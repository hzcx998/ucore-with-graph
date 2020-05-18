#ifndef __KERN_DRIVER_VBE_H__
#define __KERN_DRIVER_VBE_H__

#include <defs.h>
#include <stdio.h>
#include <mmu.h>
#include "pmm.h"
#include "memlayout.h"
#define VESA_MODE_ADDR 0xc0001300
#define VIRTUAL_DISPLAY_ADDR 0x40000000

struct VbeModeInfoBlock {
    /* Mandatory information for all VBE revisions */
    uint16_t modeAttributes;        /* mode attributes */
    uint8_t winAAttributes;        /* window A attributes */
    uint8_t winBAttributes;        /* window B attributes */
    uint16_t winGranulaity;         /* window granulaity */
    uint16_t winSize;               /* window size */
    uint16_t winASegment;           /* window A start segment */
    uint16_t winBSegment;           /* window B start segment */
    uint32_t winFuncPtr;           /* real mode pointer to window function */
    uint16_t bytesPerScanLine;      /* bytes per scan line */
    /* Mandatory information for VBE1.2 and above */
    uint16_t xResolution;           /* horizontal resolution in pixels or characters */
    uint16_t yResolution;           /* vertical resolution in pixels or characters */
    uint8_t xCharSize;             /* character cell width in pixels */
    uint8_t yCharSize;             /* character cell height in pixels */
    uint8_t numberOfPlanes;        /* number of banks */
    uint8_t bitsPerPixel;          /* bits per pixel */
    uint8_t numberOfBanks;         /* number of banks */
    uint8_t memoryModel;           /* memory model type */
    uint8_t bankSize;              /* bank size in KB */
    uint8_t numberOfImagePages;    /* number of images */
    uint8_t reserved0;             /* reserved for page function: 1 */
    uint8_t redMaskSize;           /* size of direct color red mask in bits */
    uint8_t redFieldPosition;      /* bit position of lsb of red mask */
    uint8_t greenMaskSize;         /* size of direct color green mask in bits */
    uint8_t greenFieldPosition;    /* bit position of lsb of green mask */
    uint8_t blueMaskSize;          /* size of direct color blue mask in bits */
    uint8_t blueFieldPosition;     /* bit position of lsb of blue mask */
    uint8_t rsvdMaskSize;          /* size of direct color reserved mask in bits */
    uint8_t rsvdFieldPosition;     /* bit position of lsb of reserved mask */
    uint8_t directColorModeInfo;   /* direct color mode attributes */
    
    /* Mandatory information for VBE2.0 and above */
    uint32_t phyBasePtr;           /* physical address for flat memory frame buffer */
    uint32_t reserved1;            /* reserved-always set to 0 */
    uint16_t reserved2;             /* reserved-always set to 0 */
    /* Mandatory information for VBE3.0 and above */
    uint16_t linebytesPerScanLine;  /* bytes per scan line for linear modes */
    uint8_t bnkNumberOfImagePages; /* number of images for banked modes */
    uint8_t linNumberOfImagePages; /* number of images for linear modes */
    uint8_t linRedMaskSize;        /* size of direct color red mask(linear modes) */
    uint8_t linRedFieldPosition;   /* bit position of lsb of red mask(linear modes) */
    uint8_t linGreenMaskSize;      /* size of direct color green mask(linear modes) */
    uint8_t linGreenFieldPosition; /* bit position of lsb of green mask(linear modes) */
    uint8_t linBlueMaskSize;       /* size of direct color blue mask(linear modes) */
    uint8_t linBlueFieldPosition;  /* bit position of lsb of blue mask(linear modes) */
    uint8_t linRsvdMaskSize;       /* size of direct color reserved mask(linear modes) */
    uint8_t linRsvdFieldPosition;  /* bit position of lsb of reserved mask(linear modes) */
    uint32_t maxPixelClock;        /* maximum pixel clock (in HZ) for graphics mode */
    uint8_t reserved3[189];        /* remainder of ModeInfoBlock */
} __attribute__((packed));



typedef struct VideoInfo {
    uint8_t bitsPerPixel;               /* 每个像素的位数 */
    uint32_t bytesPerScanLine;          /* 单行的字节数 */
    uint32_t xResolution, yResolution;  /* 分辨率x，y */
    uint32_t phyBasePtr;                /* 物理显存的指针 */
    
    uint32_t bytesScreenSize;           /* 整个屏幕占用的字节大小 */
    uint32_t screenSize;                /* 整个屏幕占用的大小（x*y） */
    uint8_t *virBasePtr;                /* 显存映射到内存中的地址 */

    
    void (*writePixel)(int , int, uint32_t );   /* 写像素指针 */
    void (*readPixel)(int , int, uint32_t *);

} VideoInfo_t;

extern VideoInfo_t* videoInfoPtr;

/* 视频点 */
typedef struct VideoPoint {
    int x, y;   /* 坐标 */
    unsigned int color; /* 颜色 */
} VideoPoint_t;

/* 视频填充矩形 */
typedef struct VideoRect {
    VideoPoint_t point; /* 点 */
    int width, height;  /* 大小 */
} VideoRect_t;

/* 视频传输位图 */
typedef struct VideoBitmap {
    unsigned int *bitmap;   /* 位图 */
    int x, y;       /* 位图位置 */
    int width, height;      /* 大小 */
} VideoBitmap_t;

void initVesaDriver();
void VideoWritePixel(VideoPoint_t *point);
void VideoReadPixel(VideoPoint_t *point);
void VideoBlank(uint32_t color);
void VideoFillRect(VideoRect_t *rect);
void VideoBitmapBlit(VideoBitmap_t *bitmap);

#endif  //__KERN_DRIVER_VBE_H__