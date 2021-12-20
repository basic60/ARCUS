#ifndef __ARCUS_INCLUDE_LIST_HEAD
#define __ARCUS_INCLUDE_LIST_HEAD
#include"types.h"

namespace arcus
{
    struct list_head {
        list_head *prev, *next;
    };

    #define LIST_HEAD_INIT(head) {&head, &head}
    #define LIST_ENTRY(ptr, type, member) CONTAINER_OF(ptr, type, member)

    static inline void list_add(list_head* ele, list_head* prev, list_head* next) {
        next->prev = ele;
        ele->prev = prev;
        ele->next = next;
        prev->next = ele;
    }
    
    static inline void list_add_after(list_head * ele, list_head* head) {
        list_add(ele, head, head->next);
    }

    static inline void list_del(list_head* ele) {
        ele->prev->next = ele->next;
        ele->next->prev = ele->prev;
        ele->next = ele->prev = nullptr;
    }
}
#endif