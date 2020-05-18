#ifndef _KGC_WINDOW_SWITCH_H
#define _KGC_WINDOW_SWITCH_H

#include <defs.h>
#include <window/window.h>

void KGC_SwitchWindow(KGC_Window_t *window);
void KGC_SwitchFirstWindow();
void KGC_SwitchLastWindow();
void KGC_SwitchNextWindow(KGC_Window_t *window);
void KGC_SwitchNextWindowAuto();
void KGC_SwitchPrevWindow(KGC_Window_t *window);
void KGC_SwitchPrevWindowAuto();
int KGC_SwitchTopWindow();

#endif   /* _KGC_WINDOW_SWITCH_H */
