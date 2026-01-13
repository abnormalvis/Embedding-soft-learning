#ifndef __DSTRUCT_H
#define __DSTRUCT_H 

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct dStruct{
    struct dStruct * next;
    struct dStruct * prev;
} core_linked_list;

core_linked_list * create_core_linked_list(void);
void free_ccore_linked_list(core_linked_list * cll);



#endif 




