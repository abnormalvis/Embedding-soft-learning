#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
int main(void)
{
    linked_list *list = init_linked_list();
    if (list == NULL)
    {
        fprintf(stderr, "Failed to initialize linked list\n");
        return 1;
    }

    append_node(10, list);
    append_node(20, list);
    insert_node(15, list, 1);
    delete_node(list, 0);
    show_linked_list(list);
    destroy_linked_list(list);

    return 0;
}