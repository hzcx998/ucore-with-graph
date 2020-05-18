/*
 * file:		include/kgc/window/even.h
 * auther:		Jason Hu
 * time:		2020/2/20
 * copyright:	(C) 2018-2020 by Book OS developers. All rights reserved.
 */

#ifndef _KGC_WINDOW_EVEN_H
#define _KGC_WINDOW_EVEN_H

#include <defs.h>

#include <kgceven.h>

void KGC_WindowMouseDown(int button, int mx, int my);
void KGC_WindowMouseUp(int button, int mx, int my);
void KGC_WindowMouseMotion(int mx, int my);

int KGC_WindowKeyDown(KGC_KeyboardEven_t *even);
int KGC_WindowKeyUp(KGC_KeyboardEven_t *even);

void KGC_WindowDoTimer(int ticks);

#endif   /* _KGC_WINDOW_EVEN_H */
