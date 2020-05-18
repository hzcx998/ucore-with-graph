#ifndef _DRIVER_INPUT_H
#define _DRIVER_INPUT_H

#include <defs.h>

/* 输入设备类型 */
typedef enum InputDeviceType {
    INPUT_DEVICE_IOQUEUE = 1,
    INPUT_DEVICE_BUFFER,
} InputDeviceType_t;

/* 输入缓冲区类型 */
typedef enum InputBufferType {
    INPUT_BUFFER_MOUSE = 1,
} InputBufferType_t;

/* 鼠标缓冲区最大数 */
#define MAX_MOUSE_BUFFER_NR     16

/* 输入缓冲区之鼠标 */
typedef struct InputBufferMouse {
    InputBufferType_t type;
    uint8_t button;     /*  按钮 */
    int16_t xIncrease; /* 水平增长 */
    int16_t yIncrease; /* 垂直增长 */
    int16_t zIncrease; /* 纵深增长 */
} InputBufferMouse_t;

/* 缓冲区类型输入设备鼠标数据模型 */
typedef union InputBuffer {
    InputBufferType_t type;
    InputBufferMouse_t mouse;
} InputBuffer_t;

int InitInputSystem();

#endif  /* _DRIVER_INPUT_H */
