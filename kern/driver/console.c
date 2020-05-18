#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <kbdreg.h>
#include <picirq.h>
#include <trap.h>
#include <memlayout.h>
#include <sync.h>

/* stupid I/O delay routine necessitated by historical PC design flaws */
static void
delay(void) {
    inb(0x84);
    inb(0x84);
    inb(0x84);
    inb(0x84);
}

/***** Serial I/O code *****/
#define COM1            0x3F8

#define COM_RX          0       // In:  Receive buffer (DLAB=0)
#define COM_TX          0       // Out: Transmit buffer (DLAB=0)
#define COM_DLL         0       // Out: Divisor Latch Low (DLAB=1)
#define COM_DLM         1       // Out: Divisor Latch High (DLAB=1)
#define COM_IER         1       // Out: Interrupt Enable Register
#define COM_IER_RDI     0x01    // Enable receiver data interrupt
#define COM_IIR         2       // In:  Interrupt ID Register
#define COM_FCR         2       // Out: FIFO Control Register
#define COM_LCR         3       // Out: Line Control Register
#define COM_LCR_DLAB    0x80    // Divisor latch access bit
#define COM_LCR_WLEN8   0x03    // Wordlength: 8 bits
#define COM_MCR         4       // Out: Modem Control Register
#define COM_MCR_RTS     0x02    // RTS complement
#define COM_MCR_DTR     0x01    // DTR complement
#define COM_MCR_OUT2    0x08    // Out2 complement
#define COM_LSR         5       // In:  Line Status Register
#define COM_LSR_DATA    0x01    // Data available
#define COM_LSR_TXRDY   0x20    // Transmit buffer avail
#define COM_LSR_TSRE    0x40    // Transmitter off

#define MONO_BASE       0x3B4
#define MONO_BUF        0xB0000
#define CGA_BASE        0x3D4
#define CGA_BUF         0xB8000
#define CRT_ROWS        25
#define CRT_COLS        80
#define CRT_SIZE        (CRT_ROWS * CRT_COLS)

#define LPTPORT         0x378

static uint16_t *crt_buf;
static uint16_t crt_pos;
static uint16_t addr_6845;

/* TEXT-mode CGA/VGA display output */

static void
cga_init(void) {
    volatile uint16_t *cp = (uint16_t *)(CGA_BUF + KERNBASE);
    uint16_t was = *cp;
    *cp = (uint16_t) 0xA55A;
    if (*cp != 0xA55A) {
        cp = (uint16_t*)(MONO_BUF + KERNBASE);
        addr_6845 = MONO_BASE;
    } else {
        *cp = was;
        addr_6845 = CGA_BASE;
    }

    // Extract cursor location
    uint32_t pos;
    outb(addr_6845, 14);
    pos = inb(addr_6845 + 1) << 8;
    outb(addr_6845, 15);
    pos |= inb(addr_6845 + 1);

    crt_buf = (uint16_t*) cp;
    crt_pos = pos;
}

static bool serial_exists = 0;

static void
serial_init(void) {
    // Turn off the FIFO
    outb(COM1 + COM_FCR, 0);

    // Set speed; requires DLAB latch
    outb(COM1 + COM_LCR, COM_LCR_DLAB);
    outb(COM1 + COM_DLL, (uint8_t) (115200 / 9600));
    outb(COM1 + COM_DLM, 0);

    // 8 data bits, 1 stop bit, parity off; turn off DLAB latch
    outb(COM1 + COM_LCR, COM_LCR_WLEN8 & ~COM_LCR_DLAB);

    // No modem controls
    outb(COM1 + COM_MCR, 0);
    // Enable rcv interrupts
    outb(COM1 + COM_IER, COM_IER_RDI);

    // Clear any preexisting overrun indications and interrupts
    // Serial port doesn't exist if COM_LSR returns 0xFF
    serial_exists = (inb(COM1 + COM_LSR) != 0xFF);
    (void) inb(COM1+COM_IIR);
    (void) inb(COM1+COM_RX);

    if (serial_exists) {
        pic_enable(IRQ_COM1);
    }
}

static void
lpt_putc_sub(int c) {
    int i;
    for (i = 0; !(inb(LPTPORT + 1) & 0x80) && i < 12800; i ++) {
        delay();
    }
    outb(LPTPORT + 0, c);
    outb(LPTPORT + 2, 0x08 | 0x04 | 0x01);
    outb(LPTPORT + 2, 0x08);
}

/* lpt_putc - copy console output to parallel port */
static void
lpt_putc(int c) {
    if (c != '\b') {
        lpt_putc_sub(c);
    }
    else {
        lpt_putc_sub('\b');
        lpt_putc_sub(' ');
        lpt_putc_sub('\b');
    }
}

/* cga_putc - print character to console */
static void
cga_putc(int c) {
    // set black on white
    if (!(c & ~0xFF)) {
        c |= 0x0700;
    }

    switch (c & 0xff) {
    case '\b':
        if (crt_pos > 0) {
            crt_pos --;
            crt_buf[crt_pos] = (c & ~0xff) | ' ';
        }
        break;
    case '\n':
        crt_pos += CRT_COLS;
    case '\r':
        crt_pos -= (crt_pos % CRT_COLS);
        break;
    default:
        crt_buf[crt_pos ++] = c;     // write the character
        break;
    }

    // What is the purpose of this?
    if (crt_pos >= CRT_SIZE) {
        int i;
        memmove(crt_buf, crt_buf + CRT_COLS, (CRT_SIZE - CRT_COLS) * sizeof(uint16_t));
        for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i ++) {
            crt_buf[i] = 0x0700 | ' ';
        }
        crt_pos -= CRT_COLS;
    }

    // move that little blinky thing
    outb(addr_6845, 14);
    outb(addr_6845 + 1, crt_pos >> 8);
    outb(addr_6845, 15);
    outb(addr_6845 + 1, crt_pos);
}

static void
serial_putc_sub(int c) {
    int i;
    for (i = 0; !(inb(COM1 + COM_LSR) & COM_LSR_TXRDY) && i < 12800; i ++) {
        delay();
    }
    outb(COM1 + COM_TX, c);
}

/* serial_putc - print character to serial port */
static void
serial_putc(int c) {
    if (c != '\b') {
        serial_putc_sub(c);
    }
    else {
        serial_putc_sub('\b');
        serial_putc_sub(' ');
        serial_putc_sub('\b');
    }
}

/* *
 * Here we manage the console input buffer, where we stash characters
 * received from the keyboard or serial port whenever the corresponding
 * interrupt occurs.
 * */

#define CONSBUFSIZE 512

static struct {
    uint8_t buf[CONSBUFSIZE];
    uint32_t rpos;
    uint32_t wpos;
} cons;

/* *
 * cons_intr - called by device interrupt routines to feed input
 * characters into the circular console input buffer.
 * */
static void
cons_intr(int (*proc)(void)) {
    int c;
    while ((c = (*proc)()) != -1) {
        if (c != 0) {
            cons.buf[cons.wpos ++] = c;
            if (cons.wpos == CONSBUFSIZE) {
                cons.wpos = 0;
            }
        }
    }
}

/* serial_proc_data - get data from serial port */
static int
serial_proc_data(void) {
    if (!(inb(COM1 + COM_LSR) & COM_LSR_DATA)) {
        return -1;
    }
    int c = inb(COM1 + COM_RX);
    if (c == 127) {
        c = '\b';
    }
    return c;
}

/* serial_intr - try to feed input characters from serial port */
void
serial_intr(void) {
    if (serial_exists) {
        cons_intr(serial_proc_data);
    }
}

/***** Keyboard input code *****/

#define NO              0

#define SHIFT           (1<<0)
#define CTL             (1<<1)
#define ALT             (1<<2)

#define CAPSLOCK        (1<<3)
#define NUMLOCK         (1<<4)
#define SCROLLLOCK      (1<<5)

#define E0ESC           (1<<6)

static uint8_t shiftcode[256] = {
    [0x1D] CTL,
    [0x2A] SHIFT,
    [0x36] SHIFT,
    [0x38] ALT,
    [0x9D] CTL,
    [0xB8] ALT
};

static uint8_t togglecode[256] = {
    [0x3A] CAPSLOCK,
    [0x45] NUMLOCK,
    [0x46] SCROLLLOCK
};

static uint8_t normalmap[256] = {
    NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
    'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
    '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  // 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
    [0xC7] KEY_HOME,    [0x9C] '\n' /*KP_Enter*/,
    [0xB5] '/' /*KP_Div*/,  [0xC8] KEY_UP,
    [0xC9] KEY_PGUP,    [0xCB] KEY_LF,
    [0xCD] KEY_RT,      [0xCF] KEY_END,
    [0xD0] KEY_DN,      [0xD1] KEY_PGDN,
    [0xD2] KEY_INS,     [0xD3] KEY_DEL
};

static uint8_t shiftmap[256] = {
    NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
    'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
    '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  // 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
    [0xC7] KEY_HOME,    [0x9C] '\n' /*KP_Enter*/,
    [0xB5] '/' /*KP_Div*/,  [0xC8] KEY_UP,
    [0xC9] KEY_PGUP,    [0xCB] KEY_LF,
    [0xCD] KEY_RT,      [0xCF] KEY_END,
    [0xD0] KEY_DN,      [0xD1] KEY_PGDN,
    [0xD2] KEY_INS,     [0xD3] KEY_DEL
};

#define C(x) (x - '@')

static uint8_t ctlmap[256] = {
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
    C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
    C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
    NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
    C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
    [0x97] KEY_HOME,
    [0xB5] C('/'),      [0xC8] KEY_UP,
    [0xC9] KEY_PGUP,    [0xCB] KEY_LF,
    [0xCD] KEY_RT,      [0xCF] KEY_END,
    [0xD0] KEY_DN,      [0xD1] KEY_PGDN,
    [0xD2] KEY_INS,     [0xD3] KEY_DEL
};

static uint8_t *charcode[4] = {
    normalmap,
    shiftmap,
    ctlmap,
    ctlmap
};

/* *
 * kbd_proc_data - get data from keyboard
 *
 * The kbd_proc_data() function gets data from the keyboard.
 * If we finish a character, return it, else 0. And return -1 if no data.
 * */
static int
kbd_proc_data(void) {
    int c;
    uint8_t data;
    static uint32_t shift;

    if ((inb(KBSTATP) & KBS_DIB) == 0) {
        return -1;
    }

    data = inb(KBDATAP);

    if (data == 0xE0) {
        // E0 escape character
        shift |= E0ESC;
        return 0;

    } else if (data & 0x80) {
        // Key released
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(shiftcode[data] | E0ESC);

        c = charcode[shift & (CTL | SHIFT)][data];
        if (shift & CAPSLOCK) {
            if ('a' <= c && c <= 'z')
                c += 'A' - 'a';
            else if ('A' <= c && c <= 'Z')
                c += 'a' - 'A';
        }
        c |= 0x80; /* or 0x80 to identify it as key up. */
        return c;
    } else if (shift & E0ESC) {
        // Last character was an E0 escape; or with 0x80
        data |= 0x80;
        shift &= ~E0ESC;
    }

    shift |= shiftcode[data];
    shift ^= togglecode[data];

    c = charcode[shift & (CTL | SHIFT)][data];
    if (shift & CAPSLOCK) {
        if ('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if ('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }
    
    // Process special keys
    // Ctrl-Alt-Del: reboot
    if (!(~shift & (CTL | ALT)) && c == KEY_DEL) {
        cprintf("Rebooting!\n");
        outb(0x92, 0x3); // courtesy of Chris Frost
    }
    return c;
}

/* kbd_intr - try to feed input characters from keyboard */
static void
kbd_intr(void) {
    cons_intr(kbd_proc_data);
}

/* 键盘控制器端口 */
enum KeyboardControllerPorts {
    KBC_READ_DATA   = 0x60,     /* 读取数据端口(R) */
    KBC_WRITE_DATA  = 0x60,     /* 写入数据端口(W) */
    KBC_STATUS      = 0x64,     /* 获取控制器状态(R) */
    KBC_CMD         = 0x64,     /* 向控制器发送命令(W) */
};

/* 键盘控制器的命令 */
enum KeyboardControllerCmds {
    KBC_CMD_READ_CONFIG     = 0x20,     /* 读取配置命令 */
    KBC_CMD_WRITE_CONFIG    = 0x60,     /* 写入配置命令 */
    KBC_CMD_DISABLE_MOUSE   = 0xA7,     /* 禁止鼠标端口 */
    KBC_CMD_ENABLE_MOUSE    = 0xA8,     /* 开启鼠标端口 */
    KBC_CMD_DISABLE_KEY     = 0xAD,     /* 禁止键盘通信，自动复位1控制器状态的第4位 */
    KBC_CMD_ENABLE_KEY      = 0xAE,     /* 开启键盘通信，自动置位0控制器状态的第4位 */
    KBC_CMD_SEND_TO_MOUSE   = 0xD4,     /* 向鼠标发送数据 */
    KBC_CMD_REBOOT_SYSTEM   = 0xFE,     /* 系统重启 */    
};

/* 键盘配置位 */
enum KeyboardControllerConfigBits {
    KBC_CFG_ENABLE_KEY_INTR     = (1 << 0), /* bit 0=1: 使能键盘中断IRQ1(IBE) */
    KBC_CFG_ENABLE_MOUSE_INTR   = (1 << 1), /* bit 1=1: 使能鼠标中断IRQ12(MIBE) */
    KBC_CFG_INIT_DONE           = (1 << 2), /* bit 2=1: 设置状态寄存器的位2 */
    KBC_CFG_IGNORE_STATUS_BIT4  = (1 << 3), /* bit 3=1: 忽略状态寄存器中的位4 */
    KBC_CFG_DISABLE_KEY         = (1 << 4), /* bit 4=1: 禁止键盘 */
    KBC_CFG_DISABLE_MOUSE       = (1 << 5), /* bit 5=1: 禁止鼠标 */
    KBC_CFG_SCAN_CODE_TRANS     = (1 << 6), /* bit 6=1: 将第二套扫描码翻译为第一套 */
    /* bit 7 保留为0 */
};

/* 键盘控制器状态位 */
enum KeyboardControllerStatusBits {
    KBC_STATUS_OUT_BUF_FULL     = (1 << 0), /* OUT_BUF_FULL: 输出缓冲器满置1，CPU读取后置0 */
    KBC_STATUS_INPUT_BUF_FULL   = (1 << 1), /* INPUT_BUF_FULL: 输入缓冲器满置1，i8042 取走后置0 */
    KBC_STATUS_SYS_FLAG         = (1 << 2), /* SYS_FLAG: 系统标志，加电启动置0，自检通过后置1 */
    KBC_STATUS_CMD_DATA         = (1 << 3), /* CMD_DATA: 为1，输入缓冲器中的内容为命令，为0，输入缓冲器中的内容为数据。 */
    KBC_STATUS_KYBD_INH         = (1 << 4), /* KYBD_INH: 为1，键盘没有被禁止。为0，键盘被禁止。 */
    KBC_STATUS_TRANS_TMOUT      = (1 << 5), /* TRANS_TMOUT: 发送超时，置1 */
    KBC_STATUS_RCV_TMOUT        = (1 << 6), /* RCV-TMOUT: 接收超时，置1 */
    KBC_STATUS_PARITY_EVEN      = (1 << 7), /* PARITY-EVEN: 从键盘获得的数据奇偶校验错误 */
};

/* 键盘控制器发送命令后 */
enum KeyboardControllerReturnCode {
    /* 当击键或释放键时检测到错误时，则在Output Bufer后放入此字节，
    如果Output Buffer已满，则会将Output Buffer的最后一个字节替代为此字节。
    使用Scan code set 1时使用00h，Scan code 2和Scan Code 3使用FFh。 */
    KBC_RET_KEY_ERROR_00    = 0x00,
    KBC_RET_KEY_ERROR_FF    = 0xFF,
    
    /* AAH, BAT完成代码。如果键盘检测成功，则会将此字节发送到8042 Output Register中。 */
    KBC_RET_BAT_OK          = 0xAA,

    /* EEH, Echo响应。Keyboard使用EEh响应从60h发来的Echo请求。 */
    KBC_RET_ECHO            = 0xEE,

    /* F0H, 在Scan code set 2和Scan code set 3中，被用作Break Code的前缀。*/
    KBC_RET_BREAK           = 0xF0,
    /* FAH, ACK。当Keyboard任何时候收到一个来自于60h端口的合法命令或合法数据之后，
    都回复一个FAh。 */
    KBC_RET_ACK             = 0xFA,
    
    /* FCH, BAT失败代码。如果键盘检测失败，则会将此字节发送到8042 Output Register中。 */
    KBC_RET_BAT_BAD         = 0xFC,

    /* FEH, 当Keyboard任何时候收到一个来自于60h端口的非法命令或非法数据之后，
    或者数据的奇偶交验错误，都回复一个FEh，要求系统重新发送相关命令或数据。 */
    KBC_RET_RESEND          = 0xFE,
};

/* 单独发送给键盘的命令，有别于键盘控制器命令
这些命令是发送到数据端口0x60，而不是命令0x64，
如果有参数，就在发送一次到0x60即可。
 */
enum KeyboardCmds {
    /* LED灯亮/灭，参数如下：
        位2：Caps Lock灯 1（亮）/0（灭）
        位1：Num Lock灯 1（亮）/0（灭）
        位0：Scroll Lock灯 1（亮）/0（灭）
     */
    KEY_CMD_LED_CODE        =  0xED,    /* 控制LED灯 */   
    
    /* 扫码集的参数：
        0x01： 取得当前扫描码（有返回值）
        0x02： 代表第一套扫描码
        0x03： 代表第二套扫描码
        0x04： 代表第三套扫描码
     */
    KEY_CMD_SET_SCAN_CODE   =  0xF0,    /* 设置键盘使用的扫码集*/        
    KEY_CMD_GET_DEVICE_ID   =  0xF2,    /* 获取键盘设备的ID号（2B） */                
    KEY_CMD_START_SCAN      =  0xF4,    /* 开启键盘扫描 */
    KEY_CMD_STOP_SCAN       =  0xF5,    /* 停止键盘扫描 */
    KEY_CMD_RESTART         =  0xFF,    /* 重启键盘 */
};

/* 键盘控制器配置 */
#define KBC_CONFIG	(KBC_CFG_ENABLE_KEY_INTR | KBC_CFG_ENABLE_MOUSE_INTR | \
                    KBC_CFG_INIT_DONE | KBC_CFG_SCAN_CODE_TRANS)

/* 等待键盘控制器可写入，当输入缓冲区为空后才可以写入 */
#define WAIT_KBC_WRITE()    while (inb(KBC_STATUS) & KBC_STATUS_INPUT_BUF_FULL);
/* 等待键盘控制器可读取，当输出缓冲区为空后才可以读取 */
#define WAIT_KBC_READ()    while (inb(KBC_STATUS) & KBC_STATUS_OUT_BUF_FULL);

/* 等待键盘控制器应答，如果不是回复码就一直等待
这个本应该是宏的，但是在vmware虚拟机中会卡在那儿，所以改成宏类函数
 */
static void WAIT_KBC_ACK()
{
	unsigned char read;
	do {
		read = inb(KBC_READ_DATA);
	} while ((read != KBC_RET_ACK));
}

void
kbd_init(void) {
    // drain the kbd buffer
    //kbd_intr();

    pic_enable(IRQ_KBD);
    
    /* 发送写配置命令 */
    WAIT_KBC_WRITE();
	outb(KBC_CMD, KBC_CMD_WRITE_CONFIG);
    //WAIT_KBC_ACK();
    
    /* 往数据端口写入配置值 */
    WAIT_KBC_WRITE();
	outb(KBC_WRITE_DATA, KBC_CONFIG);
	//WAIT_KBC_ACK();


}

/* cons_init - initializes the console devices */
void
cons_init(void) {
    cga_init();
    serial_init();
    
    if (!serial_exists) {
        cprintf("serial port does not exist!!\n");
    }
}

/* cons_putc - print a single character @c to console devices */
void
cons_putc(int c) {
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        lpt_putc(c);
        cga_putc(c);
        serial_putc(c);
    }
    local_intr_restore(intr_flag);
}

/* *
 * cons_getc - return the next input character from console,
 * or 0 if none waiting.
 * */
int
cons_getc(void) {
    int c = 0;
    bool intr_flag;
    local_intr_save(intr_flag);
    {
        // poll for any pending input characters,
        // so that this function works even when interrupts are disabled
        // (e.g., when called from the kernel monitor).
        //serial_intr();
        kbd_intr();

        // grab the next character from the input buffer.
        if (cons.rpos != cons.wpos) {
            c = cons.buf[cons.rpos ++];
            if (cons.rpos == CONSBUFSIZE) {
                cons.rpos = 0;
            }
        }
    }
    local_intr_restore(intr_flag);
    return c;
}

