#include <vbe.h>
#include <stdio.h>
#include <mousedrv.h>
#include <core/kgc.h>
#include "gui.h"
#include <console.h>
#include <trap.h>
#include <picirq.h>

void gui_init(){
    cprintf("init gui.\n");
    kbd_init();
    
    InitMouseDriver();
    
    
    /* init graph driver */
    initVesaDriver();
    
    /* init kernel graphic core */
    InitKGC();
    
}
