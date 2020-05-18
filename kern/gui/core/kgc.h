/* 图形绘制 */
#ifndef _KGC_H
#define _KGC_H

#include <defs.h>
#include <input/input.h>

/* KGC-kernel graph core 内核图形核心 */
void *KGC_AllocBuffer(size_t size);
int InitKGC();
void KeyboardEvenProbe();
void KGC_KeyboardInput(uint32_t key);
void KGC_MouseInput(InputBufferMouse_t *packet);

#endif   /* _KGC_H */
