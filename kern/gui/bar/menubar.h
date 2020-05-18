/*
 * file:		include/kgc/bar/menubar.h
 * auther:		Jason Hu
 * time:		2020/2/20
 * copyright:	(C) 2018-2020 by Book OS developers. All rights reserved.
 */

#ifndef _KGC_BAR_MENU_BAR_H
#define _KGC_BAR_MENU_BAR_H

#include <defs.h>
#include <kgceven.h>
#include <color.h>
#include <container/container.h>

#define KGC_MENU_BAR_HEIGHT  24
struct KGC_MenuBar {
    KGC_Container_t *container;

};
extern struct KGC_MenuBar menuBar;

void KGC_MenuBarMouseDown(int button, int mx, int my);
void KGC_MenuBarMouseUp(int button, int mx, int my);
void KGC_MenuBarMouseMotion(int mx, int my);

int KGC_MenuBarKeyDown(KGC_KeyboardEven_t *even);
int KGC_MenuBarKeyUp(KGC_KeyboardEven_t *even);

int KGC_MenuBarPoll();
void KGC_MenuBarUpdateTime();

int KGC_InitMenuBar();

#endif   /* _KGC_BAR_MENU_BAR_H */
