#include <defs.h>
#include <proc.h>
#include <kmalloc.h>
#include <kcode.h>
#include <string.h>
#include <stdio.h>
#include <gtty.h>

#include <window/message.h>
#include <window/window.h>
#include <window/draw.h>
#include <input/keycode.h>


#define DRV_NAME "gtty"

#define WIN_WIDTH   480
#define WIN_HEIGHT  360

#define CHAR_WIDTH  8
#define CHAR_HEIGHT 16

#define FRAME_PAGES 10	//有多少个显存帧

#define CHAR_TABLE_LEN 4	//table表示多少个空格

#define SCROLL_UP 1	//向上滚屏
#define SCROLL_DOWN 2	//向下滚屏

#define SAFE_CHECK 0

#define KEY_FILTER 0

typedef struct {
    int x, y;   /* 光标的位置 */
    int width, height;  /* 光标的宽高 */
    uint32_t color;     /* 光标的颜色 */
    int line;           /* 光标所在的行 */
} Cursor_t;

/* 图形tty终端 */
typedef struct {
    KGC_Window_t *window;       /* 对应的tty窗口 */
    
    int holdPid;              /* tty持有者进程id */

    uint8_t charWidth, charHeight;  /* 字符的宽度和高度 */
    int rows, columns ;   /* 行数和列数 */
    uint32_t backColor; /* 背景颜色 */
    uint32_t fontColor; /* 字体颜色 */
    
    size_t charBufferSize;  /* 字符缓冲大小 */
    int framePages;         /* 帧页数 */
    char *charBuffer;       /* 字符缓冲区 */
    char *charBufferCurrent;/* 字符缓冲区当前位置 */

    uint8_t tableLength;    /* table代表多少个空格：4，8常用 */
    
    Cursor_t cursor;    /* 光标 */
} Gtty_t;

/* gtty 表 */
Gtty_t gtty_object;

static void CursorDraw(Gtty_t *gtty);
static int CanScrollUp(Gtty_t *gtty);
static int CanScrollDown(Gtty_t *gtty);


static inline void CharGet(Gtty_t *gtty, char *ch, int x, int y)
{

	*ch = gtty->charBufferCurrent[y * gtty->columns + x];
}

static inline void CharSet(Gtty_t *gtty, char ch, int x, int y)
{
	//保存字符
	gtty->charBufferCurrent[y * gtty->columns + x] = ch;
}

/*
把光标从屏幕上去除
*/
static void CursorClear(Gtty_t *gtty)
{
	//在光标位置绘制背景
	int x = gtty->cursor.x * gtty->charWidth;
	int y = gtty->cursor.y * gtty->charHeight;

	//绘制背景
	KGC_WindowDrawRectangle(gtty->window, x, y,
        gtty->charWidth, gtty->charHeight, gtty->backColor);
    KGC_WindowRefresh(gtty->window, x, y, x + gtty->charWidth, y + gtty->charHeight);
}

static void CursorDraw(Gtty_t *gtty)
{
	//先在光标位置绘制背景，再绘制光标
	int x = gtty->cursor.x * gtty->charWidth;
	int y = gtty->cursor.y * gtty->charHeight;
	//绘制背景
	KGC_WindowDrawRectangle(gtty->window, x, y,
        gtty->charWidth, gtty->charHeight, gtty->backColor);
    
    //绘制光标
	KGC_WindowDrawRectangle(gtty->window, x, y,
        gtty->cursor.width, gtty->cursor.height, gtty->cursor.color);
    
	KGC_WindowRefresh(gtty->window, x, y, x + gtty->charWidth, y + gtty->charHeight);
}


void GttyLoadChars(Gtty_t *gtty)
{
	int bx, by, x, y;
	char ch;

	for (by = 0; by < gtty->rows; by++) {
		for (bx = 0; bx < gtty->columns; bx++) {
			CharGet(gtty, &ch, bx, by);
			
			x = bx * gtty->charWidth;
			y = by * gtty->charHeight;
			
			if (ch == '\n') {
				
			} else {
				KGC_WindowDrawChar(gtty->window, x, y, ch, gtty->fontColor);
			}
		}
    }
}


/*
向上或者向下滚动屏幕
dir 是方向
lines 是几行
accord 是不是自动
*/
static void GttyScroll(Gtty_t *gtty, int dir, int lines, int accord)
{
	if (dir == SCROLL_UP) {
		//判断是否能滚屏
		if (!CanScrollUp(gtty)) {
            //如果不能向下滚屏就返回
			return;
		}

        //清空背景
		KGC_WindowDrawRectangle(gtty->window, 0, 0,
            gtty->window->width, gtty->window->height, gtty->backColor);
        
        //修改显存起始位置
		gtty->charBufferCurrent -= gtty->columns * lines;
            
		//把字符全部加载到窗口
		GttyLoadChars(gtty);

        /* 刷新全部 */
        KGC_WindowRefresh(gtty->window, 0, 0, gtty->window->width, gtty->window->height);

        gtty->cursor.x = 0;
		gtty->cursor.y += lines;
		if (gtty->cursor.y > gtty->rows - 1) {
			gtty->cursor.y = gtty->rows - 1;
		}
		//修改光标位置
		CursorDraw(gtty);
	} else if (dir == SCROLL_DOWN) {
		
		//判断是否能滚屏
		if (!CanScrollDown(gtty)) {
			//如果不能向下滚屏就返回
			return;
		}
		
		//清空背景
		KGC_WindowDrawRectangle(gtty->window, 0, 0,
            gtty->window->width, gtty->window->height, gtty->backColor);
        
		//修改显存起始位置
		gtty->charBufferCurrent += gtty->columns * lines;

		//把字符全部加载到窗口
		GttyLoadChars(gtty);

		if (!accord) {
            gtty->cursor.x = 0;
			gtty->cursor.y -= lines;
			if (gtty->cursor.y < 0) {
				gtty->cursor.y = 0;
			}
		}
        /* 刷新全部 */
        KGC_WindowRefresh(gtty->window, 0, 0, gtty->window->width, gtty->window->height);
        
		//修改光标位置
		CursorDraw(gtty);
	}
}

static void CursorPositionCheck(Gtty_t *gtty)
{
	//如果光标向左移动超出，就切换到上一行最后
	if (gtty->cursor.x < 0) {
		
		if (gtty->cursor.y > 0) {
			//向左移动，如果发现y > 0，那么就可以移动行尾
			gtty->cursor.x = gtty->columns - 1;
		} else {
			//如果向左移动，发现y <= 0，那么就只能在行首
			gtty->cursor.x = 0;
		}
		//移动到上一行
		gtty->cursor.y--;
	}

	//如果光标向右移动超出，就切换到下一行
	if (gtty->cursor.x > gtty->columns - 1) {
        /* 如果行超出最大范围，就回到开头 */
        if (gtty->cursor.y < gtty->rows) {
            //如果y 没有到达最后一行，就移动到行首
			gtty->cursor.x = 0;
		} else {
			//如果y到达最后一行，就移动到行尾
			gtty->cursor.x = gtty->columns - 1;
		}
		//移动到下一行
		gtty->cursor.y++;
	}

	//如果光标向上移动超出，就修复
	if (gtty->cursor.y < 0) {
		//做修复处理
		gtty->cursor.y = 0;

	}

	//如果光标向下移动超出，就向下滚动屏幕
	if (gtty->cursor.y > gtty->rows -1) {
		//暂时做修复处理
		gtty->cursor.y = gtty->rows -1;

		GttyScroll(gtty, SCROLL_DOWN, 1, 1);
	}
}

/*
显示一个可见字符
*/
static void GttyOutVisual(Gtty_t *gtty, char ch, int x, int y)
{
	if (0x20 <= ch && ch <= 0x7e) {
        KGC_WindowDrawChar(gtty->window, x, y,
        ch, gtty->fontColor);
        
    	KGC_WindowRefresh(gtty->window, x, y, 
        x + gtty->charWidth, y + gtty->charHeight);
	}
}

/*
光标移动一个位置
x是x方向上的移动
y是y方向上的移动
*/
void CursorMove(Gtty_t *gtty, int x, int y)
{
	//先把光标消除
	CursorClear(gtty);

	//把原来位置上的字符显示出来
	char ch;
	CharGet(gtty, &ch, gtty->cursor.x, gtty->cursor.y);
	
	//文字颜色
    GttyOutVisual(gtty, ch, gtty->cursor.x * gtty->charWidth,
        gtty->cursor.y * gtty->charHeight);

	//移动光标
	gtty->cursor.x = x;
	gtty->cursor.y = y;
	//修复位置
	CursorPositionCheck(gtty);

	//显示光标
	CursorDraw(gtty);
	//把光标所在的字符显示出来
	CharGet(gtty, &ch, gtty->cursor.x, gtty->cursor.y);
	
	//背景的颜色
	GttyOutVisual(gtty, ch, gtty->cursor.x * gtty->charWidth,
        gtty->cursor.y * gtty->charHeight);
}

static int CanScrollUp(Gtty_t *gtty)
{
	if (gtty->charBufferCurrent > gtty->charBuffer) {
		return 1;
	}
	return 0;
}

static int CanScrollDown(Gtty_t *gtty)
{
	if (gtty->charBufferCurrent < gtty->charBuffer + gtty->charBufferSize - \
        gtty->rows * gtty->columns) {
		return 1;
	}
	return 0;
}


static void GttyOutChar(Gtty_t *gtty, char ch)
{
	//先把光标去除
	CursorClear(gtty);
    int counts;
	//对字符进行设定，如果是可显示字符就显示
	switch (ch) {
		case '\n':
			//光标的位置设定一个字符
			CharSet(gtty, ch, gtty->cursor.x, gtty->cursor.y);
            
			//能否回车
			if (CanScrollDown(gtty))
				CursorMove(gtty, 0, gtty->cursor.y + 1);
			
			break;
		case '\b':
			//改变位置
			gtty->cursor.x--;

			//改变位置后需要做检测，因为要写入字符
			CursorPositionCheck(gtty);

			CharSet(gtty, 0, gtty->cursor.x, gtty->cursor.y);

			CursorDraw(gtty);
			break;
		case '\t':
            /* 离当前位置有多少个字符 */
            counts = ((gtty->cursor.x + 4) & (~(4 - 1))) - gtty->cursor.x;
            while (counts--) {
                CharSet(gtty, ' ', gtty->cursor.x, gtty->cursor.y);
                CursorMove(gtty, gtty->cursor.x + 1, gtty->cursor.y);
            }
			break;
        case '\r':  /* 不处理 */
            break;
		default :
			CharSet(gtty, ch, gtty->cursor.x, gtty->cursor.y);

            CursorMove(gtty, gtty->cursor.x + 1, gtty->cursor.y);
            break;
	}
}


/*
清除屏幕上的所有东西，
字符缓冲区里面的文字
*/
static void GttyClear(Gtty_t *gtty)
{
	//清空背景
    KGC_WindowDrawRectangle(gtty->window, 0, 0,
        gtty->window->width, gtty->window->height, gtty->backColor);
    KGC_WindowRefresh(gtty->window, 0, 0, 
        gtty->window->width, gtty->window->height);

	//清空字符缓冲区
	memset(gtty->charBuffer, 0, gtty->charBufferSize);

	//修改字符缓冲区指针
	gtty->charBufferCurrent = gtty->charBuffer;

	//重置光标
	gtty->cursor.x = 0;
	gtty->cursor.y = 0;

	//绘制光标
	CursorDraw(gtty);
}

int GttyOpen()
{
    Gtty_t *gtty = (Gtty_t *)&gtty_object;

    /* 打开一个窗口 */
    gtty->window = KGC_WindowCreate("gtty", "gtty", 0, 50, 100, 
        WIN_WIDTH, WIN_HEIGHT, NULL);
    if (gtty->window == NULL) {
        cprintf("open gtty device failed!\n");
        return -1;
    }
    
    /* 设置字符宽高 */
    gtty->charWidth = CHAR_WIDTH;
    gtty->charHeight = CHAR_HEIGHT;
    
    /* 设置行列数量 */
    gtty->rows = gtty->window->height / gtty->charHeight;
    gtty->columns = gtty->window->width / gtty->charWidth;
    
    /* 背景颜色 */
    gtty->backColor = KGCC_BLACK;
    /* 字体颜色 */
    gtty->fontColor = KGCC_WHITE;
    
    /* 页帧数 */
    gtty->framePages = FRAME_PAGES;
    
    gtty->tableLength = CHAR_TABLE_LEN;
    
    gtty->holdPid = current->pid;

    /* 配置缓冲区 */
    gtty->charBufferSize = gtty->rows * gtty->columns * gtty->framePages;
    
    
    gtty->charBuffer = kmalloc(gtty->charBufferSize);
    if (gtty->charBuffer == NULL) {
        KGC_WindowDestroy(gtty->window);
        cprintf("alloc char buffer failed!\n");
        return -1;
    }
    memset(gtty->charBuffer, 0, gtty->charBufferSize);
    gtty->charBufferCurrent = gtty->charBuffer;
    
    /* 设置光标 */
    Cursor_t *cursor = &gtty->cursor;
    cursor->x = 0;
    cursor->y = 0;
    cursor->line = 0;
    cursor->width = gtty->charWidth;
    cursor->height = gtty->charHeight;
    
    cursor->color = KGCC_WHITE;
    
    /* 添加窗口 */
    KGC_WindowAdd(gtty->window);

    /* 绘制背景以及光标 */
    KGC_WindowDrawRectangle(gtty->window, 0, 0,
        gtty->window->width, gtty->window->height, gtty->backColor);

    CursorDraw(gtty);

    KGC_WindowRefresh(gtty->window, 0, 0, gtty->window->width, gtty->window->height);
    
    return 0;
}

int GttyClose()
{
    Gtty_t *gtty = (Gtty_t *)&gtty_object;

    //cprintf("close gtty\n");
    
    int retval = -1;
    if (gtty->charBuffer) {

        kfree(gtty->charBuffer);
        //cprintf("close char buffer\n");   
    }
    if (gtty->window) {
        retval = KGC_WindowClose(gtty->window);
        gtty->window = NULL;
        //cprintf("close windows\n");
    }
    gtty->holdPid = 0;
    //cprintf("close done\n");
    
    return retval;
}

/**
 * GttyGetc - 终端读取接口
 * @device: tty设备
 * 
 * 从tty读取键盘输入,根据len读取不同的长度
 */
int GttyGetc()
{
    /* 获取控制台 */
    Gtty_t *gtty = (Gtty_t *)&gtty_object;

    int retval = 0;
    char flags = 0;
    /* 从窗口的消息队列中读取按键 */
#if SAFE_CHECK == 1
    if (gtty->holdPid == current->pid) {
#endif
        KGC_Message_t msg;
        if (!KGC_RecvMessage(&msg)) {
#if KEY_FILTER == 1
            /* 有消息产生 */
            switch (msg.type) {
            /* 处理按键按下事件 */
            case KGC_MSG_KEY_DOWN:
                /* 如果是特殊按键，会进行预处理，然后不传递给进程。 */
                /* 如果有ctrl键按下，就检测 */
                if (msg.key.modify & KGC_KMOD_CTRL) {                            
                    switch (msg.key.code) {
                    /* ctl + c 结束一个前台进程 */
                    case 'c':
                    case 'C':
                        if (gtty->holdPid > 0) {
                            /* 发送终止提示符 */
                            GttyPutc('^');
                            GttyPutc('C');
                            //GttyPutc('\n');
                            do_kill(gtty->holdPid);
                            flags = 1;
                        }
                        break;
                    /* ctl + \ 结束一个前台进程 */
                    case '\\':
                        if (gtty->holdPid > 0) {
                            GttyPutc('^');
                            GttyPutc('\\');
                            //GttyPutc('\n');
                            do_kill(gtty->holdPid);
                            flags = 1;
                        }
                        break;
                    /* ctl + z 让前台进程暂停运行 */
                    case 'z':
                    case 'Z':

                        if (gtty->holdPid > 0) {
                            GttyPutc('^');
                            GttyPutc('Z');
                            //GttyPutc('\n');
                            do_kill(gtty->holdPid);
                            flags = 1;
                        }
                        break;
                    /* ctl + x 恢复前台进程运行 */
                    case 'x':
                    case 'X':
                        if (gtty->holdPid > 0) {
                            GttyPutc('^');
                            GttyPutc('X');
                            //GttyPutc('\n');
                            do_kill(gtty->holdPid);
                            flags = 1;
                        }
                        break;
                    default:
                        break;
                    }
                }
                /* 特殊按键转换成ascii码 */
                if (!flags) {
                    switch (msg.key.code)
                    {
                    case IKEY_ENTER:
                        retval = '\n';
                        break;
                    case IKEY_BACKSPACE:
                        retval = '\b';
                        break;
                    /*case IKEY_TAB:    tab不是用于显示输出，而是用于控制
                        retval = '\t';*/
                        break;
                    /* 需要忽略的按键 */
                    case IKEY_LSHIFT:
                    case IKEY_RSHIFT:
                    case IKEY_LCTRL:
                    case IKEY_RCTRL:
                    case IKEY_LALT:
                    case IKEY_RALT:
                        retval = 0;
                        break;
                        
                    default:
                        retval = msg.key.code;
                        break;
                    }
                }
                //retval = msg.key.code;
                    
                break;
            case KGC_MSG_QUIT:  /* 退出消息 */
                /* 退出执行 */
                do_kill(gtty->holdPid);
                            
                break;
            default: 
                break;
            }
#else
            switch (msg.type) {
            /* 处理按键按下事件 */
            case KGC_MSG_KEY_DOWN:
                retval = msg.key.code;
                break;
            default:
                retval = 0;
                break;
            }
#endif
        }
#if SAFE_CHECK == 1
    } else {
        cprintf("getc: task %d not gtty holder %d, kill it!\n", current->pid, gtty->holdPid);
        /* 不是前台任务进行读取，就会产生SIGTTIN */
        //SysKill(current->pid, SIGTTIN);
        do_kill(current->pid);
    }
#endif
    return retval;
}

/**
 * GttyPutc - 终端写入接口
 * @device: 设备
 * 
 * 往tty对应的控制台写入字符数据
 * 由于Putc的效率可能比较低，所以采用write
 */
int GttyPutc( unsigned int ch)
{
    Gtty_t *gtty = (Gtty_t *)&gtty_object;
#if SAFE_CHECK == 1
    if (gtty->holdPid == current->pid) {    
#endif
        //cprintf("%c", ch);
        GttyOutChar(gtty, ch);
#if SAFE_CHECK == 1
    } else {
        /* 其它进程可以向此tty输出信息。 */
        //GttyOutChar(gtty, ch);
        //cprintf("putc: task %d not gtty %d holder %d, kill it!\n", current->pid, gtty->deviceID, gtty->holdPid);
        
        cprintf("task %d not holder %d, kill it!\n", current->pid, gtty->holdPid);
        /* 不是前台任务进行写入，就会产生SIGTTOU */
        //SysKill(current->pid, SIGTTOU);
        do_kill(current->pid);
        
    }
#endif
    return 0;
}

/**
 * GttyIoctl - tty的IO控制
 * @device: 设备
 * @cmd: 命令
 * @arg: 参数
 * 
 * 成功返回0，失败返回-1
 */
int GttyIoctl( int cmd, int arg)
{
    Gtty_t *gtty = (Gtty_t *)&gtty_object;

	int retval = 0;
	switch (cmd)
	{
    case 1:
        GttyClear(gtty);
        break;
    case 2:
        gtty->holdPid = arg;
        //cprintf("pid %d set gtty %d holder!\n", arg, gtty->deviceID);
        break;
    default:
		/* 失败 */
		retval = -1;
		break;
	}

	return retval;
}

int GttyInit()
{
    Gtty_t *gtty = (Gtty_t *)&gtty_object;

    gtty->window = NULL;
    gtty->holdPid = 0;
    return 0;
}
