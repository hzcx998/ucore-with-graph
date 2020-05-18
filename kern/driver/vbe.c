#include "vbe.h"

VideoInfo_t videoInfo;
VideoInfo_t* videoInfoPtr = &videoInfo;

/* 单个像素由多少位组成 */
enum BitsPerPixel {
    BITS_PER_PIXEL_8    = 8,    /* 1像素8位 */
    BITS_PER_PIXEL_16   = 16,   /* 1像素16位 */
    BITS_PER_PIXEL_24   = 24,   /* 1像素24位 */
    BITS_PER_PIXEL_32   = 32,   /* 1像素32位 */
};


/* 求位数值 */
#define LOW8(a) (unsigned char)((a) & 0xff) 
#define HIGH8(a) (unsigned char)(((a) >> 8) & 0xff) 

#define LOW16(a) (unsigned short)((a) & 0xffff) 
#define HIGH16(a) (unsigned short)(((a) >> 16) & 0xffff) 

#define LOW32(a) (unsigned int)((a) & 0xffffffff) 
#define HIGH32(a) (unsigned int)(((a) >> 32) & 0xffffffff) 

/* 生成5-6-5格式的像素 */
#define ARGB_TO_16(r, g, b) ((((r >> 3) & 0x1f) << 11)   | \
    (((g >> 2)& 0x3f) << 5)   | ((b >> 3) & 0x1f ))

void VideoWritePixel8(int x, int y, uint32_t value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + (y * videoInfoPtr->xResolution + x);

    /* 写入像素数据 */
    *p = LOW8(value);
}

/**
 * VideoWritePixe16 - 写入单个像素16位
 * @value: 像素值，ARGB格式
 * 可以先根据像素宽度设置一个函数指针，指向对应的操作函数，就不用每次都进行判断了。
 */
void VideoWritePixe16(int x, int y, uint32_t value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + ((y * videoInfoPtr->xResolution + x) * 2); // 左位移1相当于乘以2

    /* 写入像素数据 */
    *(uint16_t *)p = ARGB_TO_16((value >> 16) , (value >> 8), (value));
}

/**
 * VideoWritePixe24 - 写入单个像素24位
 * @value: 像素值，ARGB格式
 * 可以先根据像素宽度设置一个函数指针，指向对应的操作函数，就不用每次都进行判断了。
 */
void VideoWritePixe24(int x, int y, uint32_t value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + (y * videoInfoPtr->xResolution + x)*3;
    
    /* 写入像素数据 */
    *(uint16_t *)p = LOW16(value);
    p += 2;
    *p = LOW8(HIGH16(value));

}

/**
 * VideoWritePixe32 - 写入单个像素32位
 * @value: 像素值，ARGB格式
 * 可以先根据像素宽度设置一个函数指针，指向对应的操作函数，就不用每次都进行判断了。
 */
void VideoWritePixe32(int x, int y, uint32_t value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + ((y * videoInfoPtr->xResolution + x) << 2); // 左移2位相当于乘以4
    
    /* 写入像素数据 */
    *(uint32_t *)p = value;
}


/**
 * VideoReadPixe8 - 读取单个像素8位
 * @value: 像素值，储存格式
 */
void VideoReadPixel8(int x, int y, uint32_t *value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + (y * videoInfoPtr->xResolution + x);

    *value = *p;
}

/**
 * VideoReadPixel16 - 读取单个像素16位
 * @value: 像素值，储存格式
 */
void VideoReadPixel16(int x, int y, uint32_t *value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + ((y * videoInfoPtr->xResolution + x) * 2); // 左位移1相当于乘以2

    *value = *(uint16_t *)p;
}

/**
 * VideoReadPixel24 - 读取单个像素24位
 * @value: 像素值，储存格式
 */
void VideoReadPixel24(int x, int y, uint32_t *value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + (y * videoInfoPtr->xResolution + x)*3;
    
    *value = *(uint16_t *)p;
    p += 2;
    *value += *p << 16;
}

/**
 * VideoReadPixel32 - 读取单个像素32位
 * @value: 像素值，储存格式
 */
void VideoReadPixel32(int x, int y, uint32_t *value)
{
    /* 根据像素大小选择对应得像素写入方法 */
    uint8_t *p;
    
    /* 获取像素位置 */
    p = videoInfoPtr->virBasePtr + ((y * videoInfoPtr->xResolution + x) << 2); // 左移2位相当于乘以4
    
    *value = *(uint32_t *)p;
}


/**
 * VideoWritePixel - 视频写像素
 * @x: 横坐标
 * @y: 纵坐标
 * @color: 颜色
 */
void VideoWritePixel(VideoPoint_t *point)
{
    if (videoInfoPtr->writePixel) {
        /* 在屏幕范围内才绘制 */
        if (point->x >= 0 && point->y >= 0 && point->x < videoInfoPtr->xResolution && 
            point->y < videoInfoPtr->yResolution) {
            /* 绘制像素 */
            videoInfoPtr->writePixel( point->x, point->y, point->color);
        }
    }
}

/**
 * VideoWritePixel - 视频写像素
 * @x: 横坐标
 * @y: 纵坐标
 * @color: 颜色
 */
void VideoReadPixel(VideoPoint_t *point)
{
    if (videoInfoPtr->readPixel) {
        /* 在屏幕范围内才绘制 */
        if (point->x >= 0 && point->y >= 0 && point->x < videoInfoPtr->xResolution && 
            point->y < videoInfoPtr->yResolution) {
            /* 绘制像素 */
            videoInfoPtr->readPixel(point->x, point->y, &point->color);
        }
    }
}

/**
 * VideoBlank - 视频填充空白
 * @color: 颜色
 */
void VideoBlank(uint32_t color)
{
    if (videoInfoPtr->writePixel) {
        int x, y;
        for (y = 0; y < videoInfoPtr->yResolution; y++) {
            for (x = 0; x < videoInfoPtr->xResolution; x++) {
                /* 在屏幕范围内才绘制 */
                if (x >= 0 && y >= 0 && x < videoInfoPtr->xResolution && 
                    y < videoInfoPtr->yResolution) {
                    /* 绘制像素 */
                    videoInfoPtr->writePixel(x, y, color);
                }
            }
        }
    }
}

/**
 * VideoFillRect - 视频填充矩形
 * @rect: 矩形
 */
void VideoFillRect(VideoRect_t *rect)
{
    if (videoInfoPtr->writePixel) {
        int x, y;
        VideoPoint_t *point = &rect->point;
        for (y = 0; y < rect->height; y++) {
            for (x = 0; x < rect->width; x++) {
                /* 在屏幕范围内才绘制 */
                if (x + point->x >= 0 && y + point->y >= 0 && 
                    x + point->x < videoInfoPtr->xResolution &&
                    y + point->x< videoInfoPtr->yResolution) {
                    /* 绘制像素 */
                    videoInfoPtr->writePixel(point->x + x, point->y + y,
                        point->color);
                }
            }
        }
    }
}

/**
 * VideoBitmapBlit - 视频传输位图
 * @bitmap: 位图
 */
void VideoBitmapBlit(VideoBitmap_t *bitmap)
{
    if (videoInfoPtr->writePixel) {
        int x, y;
        for (y = 0; y < bitmap->height; y++) {
            for (x = 0; x < bitmap->width; x++) {
                /* 在屏幕范围内才绘制 */
                if (x + bitmap->x >= 0 && y + bitmap->y >= 0 && 
                    x + bitmap->x < videoInfoPtr->xResolution &&
                    y + bitmap->x< videoInfoPtr->yResolution) {
                    /* 绘制像素 */
                    videoInfoPtr->writePixel(bitmap->x + x, bitmap->y + y,
                        bitmap->bitmap[y * bitmap->width + x]);
                }
            }
        }
    }
}


//ucore自带的虚实地址映射函数，先声明一下
void boot_map_segment(pde_t *pgdir, uintptr_t la, size_t size, uintptr_t pa, uint32_t perm);

void initVesaDriver(){
    struct VbeModeInfoBlock *vbeModeInfo = (struct VbeModeInfoBlock *)VESA_MODE_ADDR;

    /* 保存参数 */
    videoInfoPtr->bitsPerPixel = vbeModeInfo->bitsPerPixel;
    videoInfoPtr->bytesPerScanLine = vbeModeInfo->bytesPerScanLine;
    videoInfoPtr->phyBasePtr = vbeModeInfo->phyBasePtr;
    videoInfoPtr->xResolution = vbeModeInfo->xResolution;
    videoInfoPtr->yResolution = vbeModeInfo->yResolution;

    videoInfoPtr->bytesScreenSize = videoInfoPtr->bytesPerScanLine * videoInfoPtr->yResolution;

    cprintf("bitsPerPixel:%d\n", videoInfoPtr->bitsPerPixel);
    cprintf("bytesPerScanLine:%d\n", videoInfoPtr->bytesPerScanLine);
    cprintf("phyBasePtr:%x\n", videoInfoPtr->phyBasePtr);
    cprintf("xResolution:%d\n", videoInfoPtr->xResolution);
    cprintf("yResolution:%d\n", videoInfoPtr->yResolution);

    /* 对设备的显存进行IO映射到虚拟地址，就可以访问该地址来访问显存 */
    boot_map_segment(boot_pgdir, videoInfoPtr->phyBasePtr, 1024*768*2, videoInfoPtr->phyBasePtr, PTE_W);
    videoInfoPtr->virBasePtr = (uint8_t *)videoInfoPtr->phyBasePtr;
    
    /* 设置函数处理指针 */
    switch (videoInfoPtr->bitsPerPixel)
    {
    case BITS_PER_PIXEL_8:
        videoInfoPtr->writePixel = VideoWritePixel8;
        videoInfoPtr->readPixel = VideoReadPixel8;
            
        break;
    case BITS_PER_PIXEL_16:
        videoInfoPtr->writePixel = VideoWritePixe16;    
        videoInfoPtr->readPixel = VideoReadPixel16;
        
        break;
    case BITS_PER_PIXEL_24:
        videoInfoPtr->writePixel = VideoWritePixe24;    
        videoInfoPtr->readPixel = VideoReadPixel24;
        
        break;
    case BITS_PER_PIXEL_32:
        videoInfoPtr->writePixel = VideoWritePixe32;    
        videoInfoPtr->readPixel = VideoReadPixel32;
        
        break;
    default:
        videoInfoPtr->writePixel = NULL;    
        videoInfoPtr->readPixel = NULL;
        
        break;
    }
    VideoRect_t rect;
    rect.point.x = 0;
    rect.point.y = 0;
    rect.point.color = 0XFFFF00;
    rect.width = 800;
    rect.height = 600;
    VideoFillRect(&rect);
}

