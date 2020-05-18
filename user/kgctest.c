#include <stdio.h>
#include <ulib.h>
#include <graph.h>
#include <unistd.h>
#include <stdio.h>

#define printf(...)                     fprintf(1, __VA_ARGS__)
#define putc(c)                         printf("%c", c)

int
main(void) {
    cprintf("in kgc now.\n");
    
    int retval = GUI_CreateWindow("ucore", "ucore", 0,
    100, 200, 640, 480, NULL);
    if (retval) {
        cprintf("create window failed!\n");
    }

    int rect_x = 100, rect_y = 200;
    int speed_x = 1, speed_y = 1;
    int ticks = 0;
    while (1)
    {
        ticks++;
        if (ticks % 0xfff000) {
            GUI_DrawRectanglePlus(rect_x, rect_y, 50, 50, 0XFFFFFF);
            rect_x += speed_x;
            rect_y += speed_y;
            if (rect_x <= 0 || rect_x >= 640 - 50) {
                speed_x = -speed_x;
            }
            if (rect_y <= 0 || rect_y >= 480 - 50) {
                speed_y = -speed_y;
            }
            
            GUI_DrawRectanglePlus(rect_x, rect_y, 50, 50, 0XFF0000);
        }
        char ch;
        if (read(0, &ch, sizeof(char)) < 0) {
            continue;
        } else {
            printf("key: %c\n", ch);

        }
    }
    
    
    retval = GUI_CloseWindow();
    if (retval) {
        cprintf("create window failed!\n");
    }

    return 0;
}

