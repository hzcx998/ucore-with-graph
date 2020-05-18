#ifndef _KGC_WINDOW_DRAW_H
#define _KGC_WINDOW_DRAW_H

#include <defs.h>
#include <container/container.h>
#include <window/window.h>

void KGC_WindowDrawPixel(
    KGC_Window_t *window, 
    int x, 
    int y, 
    uint32_t color);

void KGC_WindowDrawRectangle(
    KGC_Window_t *window, 
    int left, 
    int top, 
    int width, 
    int height, 
    uint32_t color);

void KGC_WindowDrawBitmap(
    KGC_Window_t *window, 
    int left,
    int top, 
    int width, 
    int height, 
    uint32_t *bitmap);

void KGC_WindowDrawLine(
    KGC_Window_t *window,
    int x0,
    int y0,
    int x1,
    int y1, 
    uint32_t color);

void KGC_WindowDrawChar(
    KGC_Window_t *window,
    int x,
    int y,
    char ch,
    uint32_t color);
void KGC_WindowDrawString(
    KGC_Window_t *window,
    int x,
    int y,
    char *str,
    uint32_t color);

void KGC_WindowPaintMoving(
    KGC_Container_t *container,
    KGC_Window_t *window,
    int draw);

void KGC_WindowPaintActive(KGC_Window_t *window, int active);

#endif   /* _KGC_WINDOW_DRAW_H */
