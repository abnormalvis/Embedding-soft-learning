#include "dStruct.h"

// 创建一个内核链表的头部
core_linked_list * create_core_linked_list(void)
{
    core_linked_list * cstruct = (core_linked_list *)malloc(sizeof(core_linked_list));
    if(cstruct != NULL)
    {
        cstruct->next = NULL;
        cstruct->prev = NULL;
        return cstruct;
    }
    else{
        return NULL;
    }
}
// 删除内核链表, 需要让包含内核链表的结构体先调用free_core_linked_list
void free_core_linked_list(core_linked_list * cll)
{
    cll->next = NULL;
    cll->prev = NULL;
    free(cll);
}
