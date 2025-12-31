#include "include/doubly_linked_list.h"

#include <assert.h>
#include <stdio.h>

static void test_init_destroy()
{
    printf("\n=== DLL Test: init/destroy ===\n");
    doubly_list *list = dll_init();
    assert(list != NULL);
    assert(dll_is_empty(list));
    assert(dll_length(list) == 0);
    dll_destroy(list);
    printf("✓ init/destroy\n");
}

static void test_push_pop_single()
{
    printf("\n=== DLL Test: single node push/pop ===\n");
    doubly_list *list = dll_init();

    assert(dll_push_front(list, 10));
    assert(dll_length(list) == 1);

    int v = 0;
    assert(dll_pop_back(list, &v));
    assert(v == 10);
    assert(dll_is_empty(list));

    dll_destroy(list);
    printf("✓ single node push/pop\n");
}

static void test_push_front_back_order()
{
    printf("\n=== DLL Test: push_front/push_back order ===\n");
    doubly_list *list = dll_init();

    assert(dll_push_back(list, 2));   // [2]
    assert(dll_push_front(list, 1));  // [1,2]
    assert(dll_push_back(list, 3));   // [1,2,3]

    int v = 0;
    assert(dll_get_at(list, 0, &v) && v == 1);
    assert(dll_get_at(list, 1, &v) && v == 2);
    assert(dll_get_at(list, 2, &v) && v == 3);

    dll_print_forward(list);
    dll_print_backward(list);

    dll_destroy(list);
    printf("✓ order maintained\n");
}

static void test_insert_delete_middle()
{
    printf("\n=== DLL Test: insert/delete middle ===\n");
    doubly_list *list = dll_init();

    for (int i = 1; i <= 5; i++)
    {
        assert(dll_push_back(list, i)); // [1,2,3,4,5]
    }

    assert(dll_insert_at(list, 2, 99)); // [1,2,99,3,4,5]

    int v = 0;
    assert(dll_get_at(list, 2, &v) && v == 99);
    assert(dll_length(list) == 6);

    int deleted = 0;
    assert(dll_delete_at(list, 2, &deleted));
    assert(deleted == 99);
    assert(dll_length(list) == 5);

    assert(dll_get_at(list, 2, &v) && v == 3);

    dll_print_forward(list);
    dll_print_backward(list);

    dll_destroy(list);
    printf("✓ insert/delete middle\n");
}

static void test_boundary_conditions()
{
    printf("\n=== DLL Test: boundary conditions ===\n");
    doubly_list *list = dll_init();

    // delete on empty
    assert(dll_pop_front(list, NULL) == false);

    // insert invalid
    assert(dll_insert_at(list, -1, 1) == false);
    assert(dll_insert_at(list, 1, 1) == false);

    // insert at 0 when empty ok
    assert(dll_insert_at(list, 0, 5));

    int v = 0;
    assert(dll_get_at(list, 1, &v) == false);

    int out = 0;
    assert(dll_delete_at(list, 0, &out));
    assert(out == 5);
    assert(dll_is_empty(list));

    dll_destroy(list);
    printf("✓ boundary conditions\n");
}

static void test_find()
{
    printf("\n=== DLL Test: find_first ===\n");
    doubly_list *list = dll_init();

    assert(dll_find_first(list, 1) == -1);

    assert(dll_push_back(list, 7));
    assert(dll_push_back(list, 8));
    assert(dll_push_back(list, 7));

    assert(dll_find_first(list, 7) == 0);
    assert(dll_find_first(list, 8) == 1);
    assert(dll_find_first(list, 42) == -1);

    dll_destroy(list);
    printf("✓ find_first\n");
}

int main(void)
{
    printf("========================================\n");
    printf("  Doubly Linked List Implementation Test \n");
    printf("========================================\n");

    test_init_destroy();
    test_push_pop_single();
    test_push_front_back_order();
    test_insert_delete_middle();
    test_boundary_conditions();
    test_find();

    printf("\n========================================\n");
    printf("  All DLL Tests Passed! ✓\n");
    printf("========================================\n");

    return 0;
}
