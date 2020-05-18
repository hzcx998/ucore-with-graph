#ifndef __KERN_DRIVER_MOUSE_H__
#define __KERN_DRIVER_MOUSE_H__

#include <defs.h>
#include <stdio.h>

void MouseHnadler();
int InitMouseDriver();
int MouseRead(void *buffer, unsigned int len);

#endif  //__KERN_DRIVER_MOUSE_H__