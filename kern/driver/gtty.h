#ifndef __KERN_DRIVER_GTTY_H__
#define __KERN_DRIVER_GTTY_H__

#include <defs.h>

int GttyPutc( unsigned int ch);
int GttyGetc();
int GttyClose();
int GttyOpen();
int GttyIoctl( int cmd, int arg);

#endif  //__KERN_DRIVER_GTTY_H__