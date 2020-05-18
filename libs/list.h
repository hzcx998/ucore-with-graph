#ifndef __LIBS_LIST_H__
#define __LIBS_LIST_H__

#ifndef __ASSEMBLER__

#include <defs.h>

/* *
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when manipulating
 * whole lists rather than single entries, as sometimes we already know
 * the next/prev entries and we can generate better code by using them
 * directly rather than using the generic single-entry routines.
 * */

struct list_entry {
    struct list_entry *prev, *next;
};


/* 为链表结构赋值 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/* 创建并赋值 */
#define LIST_HEAD(name) \
        struct list_entry name = LIST_HEAD_INIT(name)

typedef struct list_entry list_entry_t;

static inline void list_init(list_entry_t *elm) __attribute__((always_inline));
static inline void list_add(list_entry_t *listelm, list_entry_t *elm) __attribute__((always_inline));
static inline void list_add_before(list_entry_t *listelm, list_entry_t *elm) __attribute__((always_inline));
static inline void list_add_after(list_entry_t *listelm, list_entry_t *elm) __attribute__((always_inline));
static inline void list_del(list_entry_t *listelm) __attribute__((always_inline));
static inline void list_del_init(list_entry_t *listelm) __attribute__((always_inline));
static inline bool list_empty(list_entry_t *list) __attribute__((always_inline));
static inline list_entry_t *list_next(list_entry_t *listelm) __attribute__((always_inline));
static inline list_entry_t *list_prev(list_entry_t *listelm) __attribute__((always_inline));

static inline void __list_add(list_entry_t *elm, list_entry_t *prev, list_entry_t *next) __attribute__((always_inline));
static inline void __list_del(list_entry_t *prev, list_entry_t *next) __attribute__((always_inline));

/* *
 * list_init - initialize a new entry
 * @elm:        new entry to be initialized
 * */
static inline void
list_init(list_entry_t *elm) {
    elm->prev = elm->next = elm;
}

/* *
 * list_add - add a new entry
 * @listelm:    list head to add after
 * @elm:        new entry to be added
 *
 * Insert the new element @elm *after* the element @listelm which
 * is already in the list.
 * */
static inline void
list_add(list_entry_t *listelm, list_entry_t *elm) {
    list_add_after(listelm, elm);
}

/*
 * ListAddTail - 添加一个新的节点到链表尾
 * @new： 要新添加的节点
 * @head：要添加到哪个链表头
 * 
 * 把一个节点添加到链表头前面，相当于添加到整个链表的最后面。
 */
static inline void list_add_tail(list_entry_t *_new, list_entry_t *head)
{
   // :) 插入到链表头前一个和链表头之间
   __list_add(_new, head->prev, head);
}

/* *
 * list_add_before - add a new entry
 * @listelm:    list head to add before
 * @elm:        new entry to be added
 *
 * Insert the new element @elm *before* the element @listelm which
 * is already in the list.
 * */
static inline void
list_add_before(list_entry_t *listelm, list_entry_t *elm) {
    __list_add(elm, listelm->prev, listelm);
}

/* *
 * list_add_after - add a new entry
 * @listelm:    list head to add after
 * @elm:        new entry to be added
 *
 * Insert the new element @elm *after* the element @listelm which
 * is already in the list.
 * */
static inline void
list_add_after(list_entry_t *listelm, list_entry_t *elm) {
    __list_add(elm, listelm, listelm->next);
}

/* *
 * list_del - deletes entry from list
 * @listelm:    the element to delete from the list
 *
 * Note: list_empty() on @listelm does not return true after this, the entry is
 * in an undefined state.
 * */
static inline void
list_del(list_entry_t *listelm) {
    __list_del(listelm->prev, listelm->next);
}

/* *
 * list_del_init - deletes entry from list and reinitialize it.
 * @listelm:    the element to delete from the list.
 *
 * Note: list_empty() on @listelm returns true after this.
 * */
static inline void
list_del_init(list_entry_t *listelm) {
    list_del(listelm);
    list_init(listelm);
}

/* *
 * list_empty - tests whether a list is empty
 * @list:       the list to test.
 * */
static inline bool
list_empty(list_entry_t *list) {
    return list->next == list;
}

/* *
 * list_next - get the next entry
 * @listelm:    the list head
 **/
static inline list_entry_t *
list_next(list_entry_t *listelm) {
    return listelm->next;
}

/* *
 * list_prev - get the previous entry
 * @listelm:    the list head
 **/
static inline list_entry_t *
list_prev(list_entry_t *listelm) {
    return listelm->prev;
}

/* *
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 * */
static inline void
__list_add(list_entry_t *elm, list_entry_t *prev, list_entry_t *next) {
    prev->next = next->prev = elm;
    elm->next = next;
    elm->prev = prev;
}

/* *
 * Delete a list entry by making the prev/next entries point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 * */
static inline void
__list_del(list_entry_t *prev, list_entry_t *next) {
    prev->next = next;
    next->prev = prev;
}


/*
 * list_owner - 获取节点的宿主
 * @ptr： 节点的指针
 * @type： 宿主结构体的类型
 * @member: 节点在宿主结构体中的名字 
 */
#define list_owner(ptr, type, member) container_of(ptr, type, member)

/*
 * list_first_owner - 获取链表中的第一个宿主
 * @head： 链表头
 * @type： 宿主结构体的类型
 * @member: 节点在宿主结构体中的名字 
 * 
 * 注：链表不能为空
 */
#define list_first_owner(head, type, member) \
      list_owner((head)->next, type, member)


/*
 * list_last_owner - 获取链表中的最后一个宿主
 * @head:  链表头
 * @type： 宿主结构体的类型
 * @member: 节点在宿主结构体中的名字 
 * 
 * 注：链表不能为空
 */
#define list_last_owner(head, type, member) \
      list_owner((head)->prev, type, member)

/*
 * list_first_owner_or_null - 获取链表中的第一个宿主
 * @head： 链表头
 * @type： 宿主结构体的类型
 * @member: 节点在宿主结构体中的名字 
 * 
 * 注：如果链表是空就返回NULL
 */
#define list_first_owner_or_null(head, type, member) ({ \
      struct list_entry *__head = (head); \
      struct list_entry *__pos = (__head->next); \
      __pos != __head ? list_owner(__pos, type, member) : NULL; \
})

/*
 * list_last_owner_or_null - 获取链表中的最后一个宿主
 * @head： 链表头
 * @type： 宿主结构体的类型
 * @member: 节点在宿主结构体中的名字 
 * 
 * 注：如果链表是空就返回NULL
 */
#define list_last_owner_or_null(head, type, member) ({ \
      struct list_entry *__head = (head); \
      struct list_entry *__pos = (__head->prev); \
      __pos != __head ? list_owner(__pos, type, member) : NULL; \
})

/*
 * list_next_owner - 获取链表中的下一个宿主
 * @pos： 临时宿主的指针
 * @member: 节点在宿主结构体中的名字 
 */
#define list_next_owner(pos, member) \
      list_owner((pos)->member.next, typeof(*(pos)), member)

/*
 * list_prev_onwer - 获取链表中的前一个宿主
 * @pos： 临时宿主的指针
 * @member: 节点在宿主结构体中的名字 
 */
#define list_prev_onwer(pos, member) \
      list_owner((pos)->member.prev, typeof(*(pos)), member)

/* 把代码自己打一遍，好累啊！但是感觉这些东西也更加明白了 */

/* 记住啦，这是遍历链表节点，不是宿主 -->>*/

/*
 * list_for_each - 从前往后遍历每一个链表节点
 * @pos： 节点指针
 * @head: 链表头 
 */
#define list_for_each(pos, head) \
      for (pos = (head)->next; pos != (head); pos = pos->next)


/*
 * list_for_each_prev - 从后往前遍历每一个链表节点
 * @pos： 节点指针
 * @head: 链表头 
 */
#define list_for_each_prev(pos, head) \
      for (pos = (head)->prev; pos != (head); pos = pos->prev)

/*
 * list_for_each_safe - 从前往后遍历每一个链表节点
 * @pos: 节点指针
 * @_next: 临时节点指针（为了避免和pos->next混淆，在前面加_）
 * @head: 链表头 
 * 
 * 用next来保存下一个节点指针，如果在遍历过程中pos出的节点被删除了，
 * 还是可以继续往后面遍历其它节点。
 */
#define list_for_each_safe(pos, _next, head) \
      for (pos = (head)->next, _next = pos->next; pos != (head); \
         pos = _next, _next = pos->next)

/*
 * list_for_each_prev_safe - 从后往前遍历每一个链表节点
 * @pos: 节点指针
 * @_prev: 临时节点指针（为了避免和pos->prev混淆，在前面加_）
 * @head: 链表头 
 * 
 * 用prev来保存前一个节点指针，如果在遍历过程中pos出的节点被删除了，
 * 还是可以继续往前面遍历其它节点。
 */
#define list_for_each_prev_safe(pos, _prev, head) \
      for (pos = (head)->prev, _prev = pos->prev; pos != (head); \
         pos = _prev, _prev = pos->prev)

/*  <<-- 遍历链表节点结束了，接下来开始的是遍历宿主 -->> */

/*
 * list_for_each_owner - 从前往后遍历每一个链表节点宿主
 * @pos: 宿主类型结构体指针
 * @head: 链表头 
 * @member: 节点在宿主中的名字
 */
#define list_for_each_owner(pos, head, member)                    \
      for (pos = list_first_owner(head, typeof(*pos), member);   \
         &pos->member != (head);                               \
         pos = list_next_owner(pos, member))

/*
 * list_for_each_owner_reverse - 从后往前遍历每一个链表节点宿主
 * @pos: 宿主类型结构体指针
 * @head: 链表头 
 * @member: 节点在宿主中的名字
 */
#define list_for_each_owner_reverse(pos, head, member)            \
      for (pos = list_last_owner(head, typeof(*pos), member);   \
         &pos->member != (head);                              \
         pos = list_prev_onwer(pos, member))

/*
 * list_for_each_owner_safe - 从前往后遍历每一个链表节点宿主
 * @pos: 宿主类型结构体指针
 * @next: 临时指向下一个节点的指针
 * @head: 链表头 
 * @member: 节点在宿主中的名字
 * 
 * 可以保证在遍历过程中如果
 */
#define list_for_each_owner_safe(pos, next, head, member)          \
      for (pos = list_first_owner(head, typeof(*pos), member),   \
         next = list_next_owner(pos, member);                    \
         &pos->member != (head);                               \
         pos = next, next = list_next_owner(next, member))

/*
 * list_for_each_owner_reverse_safe - 从后往前遍历每一个链表节点宿主
 * @pos: 宿主类型结构体指针
 * @_prev: 临时指向前一个节点的指针
 * @head: 链表头 
 * @member: 节点在宿主中的名字
 * 
 * 可以保证在遍历过程中如果
 */
#define list_for_each_owner_reverse_safe(pos, prev, head, member)   \
      for (pos = list_last_owner(head, typeof(*pos), member),    \
         prev = list_prev_onwer(pos, member);                    \
         &pos->member != (head);                               \
         pos = prev, prev = list_prev_onwer(prev, member))


/*
 * list_length - 获取链表长度
 * @head: 链表头
 */
static inline int list_length(struct list_entry *head)
{
   struct list_entry *list;
   int n = 0;
   list_for_each(list, head) {
      // 找到一样的
      if (list == head)
         break;
      n++;
   }
   return n;
}

#endif /* !__ASSEMBLER__ */

#endif /* !__LIBS_LIST_H__ */

