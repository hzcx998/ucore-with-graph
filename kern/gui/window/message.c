/* 系统内核 */
#include <vbe.h>
#include <kmalloc.h>
#include <input/mouse.h>
#include <window/message.h>
#include <window/window.h>
#include <core/kgc.h>
#include <sync.h>
#include <proc.h>

KGC_MessageNode_t *KGC_CreateMessageNode()
{
    KGC_MessageNode_t *node = kmalloc(sizeof(KGC_MessageNode_t));
    return node;
}

void KGC_FreeMessageList(KGC_Window_t *window)
{
    KGC_MessageNode_t *node, *next;
    /* 释放消息队列 */
    list_for_each_owner_safe (node, next, &window->messageListHead, list) {
        /* 从链表中删除 */
        list_del(&node->list);
        /* 释放消息节点 */
        kfree(node);
    }
}

void KGC_AddMessageNode(KGC_MessageNode_t *node, KGC_Window_t *window)
{
    unsigned int flags;
    local_intr_save(flags);

    /* 如果消息数量超过最大数量，那么就删除掉队列中的第一个消息 */
    list_add_tail(&node->list, &window->messageListHead);
    local_intr_restore(flags);   
}

/**
 * KGC_RecvMessage - 获取一个消息
 * @message: 消息
 * 
 * 成功返回0，失败返回-1
 */
int KGC_RecvMessage(KGC_Message_t *message)
{
    /* 从窗口的消息队列种获取一个消息 */
    
    /* 获取指针并检测 */
    struct proc_struct *cur = current;
    if (!cur->window) 
        return -1;
    KGC_Window_t *window = cur->window;
    if (!window)
        return -1;

    unsigned int flags;
    local_intr_save(flags);

    /* 没有消息 */
    if (list_empty(&window->messageListHead)) {
        local_intr_restore(flags);
        return -1;
    }
    //printk("[receive]");
    
    /* 获取一个消息 */
    KGC_MessageNode_t *node = list_owner(window->messageListHead.next, KGC_MessageNode_t, list);
    /* 从链表删除 */
    list_del(&node->list);
    
    local_intr_restore(flags);
    *message = node->message;

    kfree(node);
    //printk("[receive]");
    return 0;
}

/**
 * KGC_SetMessage - 放置一个消息
 * @message: 消息
 * 
 * 成功返回0，失败返回-1
 */
int KGC_SendMessage(KGC_Message_t *message)
{
    int retval = -1;
    switch (message->type)
    {
    /* 窗口管理 */
    case KGC_MSG_WINDOW_CREATE:
    case KGC_MSG_WINDOW_CLOSE:
        retval = KGC_MessageDoWindow(&message->window);
        break;
    /* 绘图 */        
    case KGC_MSG_DRAW_PIXEL:
    case KGC_MSG_DRAW_BITMAP:
    case KGC_MSG_DRAW_RECTANGLE:
    case KGC_MSG_DRAW_LINE:
    case KGC_MSG_DRAW_CHAR:
    case KGC_MSG_DRAW_STRING:
    case KGC_MSG_DRAW_PIXEL_PLUS:
    case KGC_MSG_DRAW_BITMAP_PLUS:
    case KGC_MSG_DRAW_RECTANGLE_PLUS:
    case KGC_MSG_DRAW_LINE_PLUS:
    case KGC_MSG_DRAW_CHAR_PLUS:
    case KGC_MSG_DRAW_STRING_PLUS:
    case KGC_MSG_DRAW_UPDATE:
        retval = KGC_MessageDoDraw(&message->draw);
        break;
    default:
        break;
    }
    
    return retval;
}

int SysKGC_Message(int operate, KGC_Message_t *message)
{
    if (operate == KGC_MSG_SEND) {
        return KGC_SendMessage(message);
    } else if (operate == KGC_MSG_RECV) {
        return KGC_RecvMessage(message);
    }
    return -1;
}
