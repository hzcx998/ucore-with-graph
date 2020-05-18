
/* 系统内核 */
#include <window/window.h>
#include <window/message.h>
#include <window/draw.h>
#include <font/font.h>
#include <string.h>
#include <proc.h>
#include <sync.h>

/**
 * KGC_MessageDoWindow - 窗口消息处理
 * 
 * 窗口管理相关的消息处理
 * 
 */
int KGC_MessageDoWindow(KGC_MessageWindow_t *message)
{
    int retval = -1;

    KGC_Window_t *window;
    switch (message->type)
    {
    case KGC_MSG_WINDOW_CREATE:
        window = KGC_WindowCreate(message->name, message->title ,
            message->style, message->x, message->y ,message->width, 
            message->height, message->param);
        if (!window)
            break;
        
        if (KGC_WindowAdd(window)) {
            break;
        }
        
        retval = 0;
        break;
    case KGC_MSG_WINDOW_CLOSE:
        window = current->window;
        if (window) {
            retval = KGC_WindowClose(window);
        }
        break;
    default:
        break;
    }

    return retval;
}

/**
 * KGC_MessageDoDraw - 绘制消息处理
 * 
 * 绘制相关的消息处理
 * 
 */
int KGC_MessageDoDraw(KGC_MessageDraw_t *message)
{
    /* 获取指针并检测 */
    if (!current->window) 
        return -1;

    int retval = -1;

    KGC_Window_t *window = current->window;
    
    switch (message->type)
    {

    case KGC_MSG_DRAW_PIXEL:
        KGC_WindowDrawPixel(window, message->left, message->top,
            message->color);
        retval = 0;
        break;
    case KGC_MSG_DRAW_RECTANGLE:
        KGC_WindowDrawRectangle(window, message->left, message->top, 
            message->width, message->height, message->color);
        retval = 0;
        break;
    case KGC_MSG_DRAW_BITMAP:
        KGC_WindowDrawBitmap(window, message->left, message->top, 
            message->width, message->height, message->bitmap);
        retval = 0;
        break;
    case KGC_MSG_DRAW_LINE:
        KGC_WindowDrawLine(window, message->left, message->top, 
            message->right, message->buttom, message->color);
        retval = 0;
        break;
    case KGC_MSG_DRAW_CHAR:
        KGC_WindowDrawChar(window, message->left, message->top, 
            message->character, message->color);
        retval = 0;
        break;
    case KGC_MSG_DRAW_STRING:
        KGC_WindowDrawString(window, message->left, message->top, 
            message->string, message->color);
        retval = 0;
        break;
        
    case KGC_MSG_DRAW_PIXEL_PLUS:
        KGC_WindowDrawPixel(window, message->left, message->top, 
            message->color);
        KGC_WindowRefresh(window, message->left, message->top, 
            message->left + 1, message->top + 1);
            retval = 0;
        break;
        
    case KGC_MSG_DRAW_RECTANGLE_PLUS:
        KGC_WindowDrawRectangle(window, message->left, message->top, 
            message->width, message->height, message->color);
        KGC_WindowRefresh(window, message->left, message->top, 
            message->left + message->width, message->top + message->height);
            retval = 0;
        break;
    case KGC_MSG_DRAW_BITMAP_PLUS:
        KGC_WindowDrawBitmap(window, message->left, message->top, 
            message->width, message->height, message->bitmap);
        KGC_WindowRefresh(window, message->left, message->top, 
            message->left + message->width, message->top + message->height);
        retval = 0;
        break;
    case KGC_MSG_DRAW_LINE_PLUS:
        KGC_WindowDrawLine(window, message->left, message->top, 
            message->right, message->buttom, message->color);
        KGC_WindowRefresh(window, message->left, message->top, 
            message->right, message->buttom);
        retval = 0;
        break;
    case KGC_MSG_DRAW_CHAR_PLUS:
        KGC_WindowDrawChar(window, message->left, message->top, 
            message->character, message->color);
        KGC_WindowRefresh(window, message->left, message->top, 
            message->left + currentFont->width, message->top + currentFont->height);
        retval = 0;
        break;
    case KGC_MSG_DRAW_STRING_PLUS:
        KGC_WindowDrawString(window, message->left, message->top, 
            message->string, message->color);
        KGC_WindowRefresh(window, message->left, message->top, 
            message->left + strlen(message->string) * currentFont->width,
            message->top + currentFont->height);
        
        retval = 0;
        break;
    case KGC_MSG_DRAW_UPDATE:
        KGC_WindowRefresh(window, message->left, message->top, 
            message->right, message->buttom);
        retval = 0;
        break;
    default:
        break;
    }
    return retval;
}
