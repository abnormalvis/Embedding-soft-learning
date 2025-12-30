#include "../include/sequential_list.h"
#include <stdio.h>
#include <assert.h>

void test_create_and_destroy()
{
    printf("\n=== Test: Create and Destroy ===\n");
    
    sqlist *list = create_sequential_list(10);
    assert(list != NULL);
    assert(list->length == 0);
    assert(list->capacity == 10);
    printf("✓ Created list with capacity 10\n");
    
    destroy_sequential_list(list);
    printf("✓ List destroyed successfully\n");
}

void test_basic_operations()
{
    printf("\n=== Test: Basic Operations ===\n");
    
    sqlist *list = create_sequential_list(10);
    
    // 测试空表
    assert(is_empty(list) == true);
    assert(is_full(list) == false);
    assert(get_length(list) == 0);
    printf("✓ Empty list checks passed\n");
    
    // 添加元素
    assert(append(list, 10) == true);
    assert(append(list, 20) == true);
    assert(append(list, 30) == true);
    assert(is_empty(list) == false);
    assert(get_length(list) == 3);
    printf("✓ Append operations passed\n");
    
    print_list(list);
    
    // 清空列表
    clear_list(list);
    assert(is_empty(list) == true);
    assert(get_length(list) == 0);
    printf("✓ Clear operation passed\n");
    
    destroy_sequential_list(list);
}

void test_insert_operations()
{
    printf("\n=== Test: Insert Operations ===\n");
    
    sqlist *list = create_sequential_list(10);
    
    // 在不同位置插入
    assert(insert_at(list, 0, 10) == true);  // [10]
    assert(insert_at(list, 0, 5) == true);   // [5, 10]
    assert(insert_at(list, 2, 15) == true);  // [5, 10, 15]
    assert(insert_at(list, 1, 7) == true);   // [5, 7, 10, 15]
    
    printf("✓ Insert at different positions: ");
    print_list(list);
    
    assert(get_length(list) == 4);
    
    int value;
    assert(get_at(list, 0, &value) == true && value == 5);
    assert(get_at(list, 1, &value) == true && value == 7);
    assert(get_at(list, 2, &value) == true && value == 10);
    assert(get_at(list, 3, &value) == true && value == 15);
    printf("✓ Values verified: [5, 7, 10, 15]\n");
    
    destroy_sequential_list(list);
}

void test_delete_operations()
{
    printf("\n=== Test: Delete Operations ===\n");
    
    sqlist *list = create_sequential_list(10);
    
    // 准备测试数据
    for (int i = 1; i <= 5; i++)
    {
        append(list, i * 10);
    }
    printf("Initial list: ");
    print_list(list);
    
    // 删除不同位置的元素
    int deleted_value;
    assert(delete_at(list, 2, &deleted_value) == true);
    assert(deleted_value == 30);
    printf("✓ Deleted value at index 2: %d\n", deleted_value);
    print_list(list);
    
    assert(delete_at(list, 0, &deleted_value) == true);
    assert(deleted_value == 10);
    printf("✓ Deleted value at index 0: %d\n", deleted_value);
    print_list(list);
    
    assert(get_length(list) == 3);
    
    destroy_sequential_list(list);
}

void test_search_operations()
{
    printf("\n=== Test: Search Operations ===\n");
    
    sqlist *list = create_sequential_list(10);
    
    append(list, 10);
    append(list, 20);
    append(list, 30);
    append(list, 20);  // 重复值
    append(list, 40);
    
    print_list(list);
    
    // 查找存在的值
    int index = find_value(list, 20);
    assert(index == 1);  // 返回第一次出现的位置
    printf("✓ Found value 20 at index: %d\n", index);
    
    index = find_value(list, 40);
    assert(index == 4);
    printf("✓ Found value 40 at index: %d\n", index);
    
    // 查找不存在的值
    index = find_value(list, 999);
    assert(index == -1);
    printf("✓ Value 999 not found (returns -1)\n");
    
    destroy_sequential_list(list);
}

void test_get_set_operations()
{
    printf("\n=== Test: Get and Set Operations ===\n");
    
    sqlist *list = create_sequential_list(10);
    
    for (int i = 0; i < 5; i++)
    {
        append(list, i * 10);
    }
    
    printf("Initial list: ");
    print_list(list);
    
    // 获取元素
    int value;
    assert(get_at(list, 2, &value) == true);
    assert(value == 20);
    printf("✓ Got value at index 2: %d\n", value);
    
    // 设置元素
    assert(set_at(list, 2, 999) == true);
    assert(get_at(list, 2, &value) == true);
    assert(value == 999);
    printf("✓ Set value at index 2 to 999\n");
    print_list(list);
    
    destroy_sequential_list(list);
}

void test_boundary_conditions()
{
    printf("\n=== Test: Boundary Conditions ===\n");
    
    sqlist *list = create_sequential_list(3);
    
    // 填满列表
    assert(append(list, 1) == true);
    assert(append(list, 2) == true);
    assert(append(list, 3) == true);
    assert(is_full(list) == true);
    printf("✓ List is full\n");
    print_list(list);
    
    // 尝试向已满的列表插入
    assert(append(list, 4) == false);
    printf("✓ Cannot append to full list\n");
    
    // 删除元素后再插入
    int value;
    assert(delete_at(list, 1, &value) == true);
    assert(is_full(list) == false);
    assert(append(list, 4) == true);
    printf("✓ After delete, can append again\n");
    print_list(list);
    
    // 测试边界索引
    assert(get_at(list, -1, &value) == false);
    assert(get_at(list, 100, &value) == false);
    printf("✓ Invalid index checks passed\n");
    
    destroy_sequential_list(list);
}

void test_comprehensive_scenario()
{
    printf("\n=== Test: Comprehensive Scenario ===\n");
    
    sqlist *list = create_sequential_list(20);
    
    // 创建一个简单的数字序列
    printf("Building sequence: ");
    for (int i = 1; i <= 10; i++)
    {
        append(list, i);
    }
    print_list(list);
    
    // 在中间插入几个元素
    insert_at(list, 5, 99);
    insert_at(list, 5, 88);
    printf("After inserting 88 and 99 at index 5: ");
    print_list(list);
    
    // 删除一些元素
    int deleted;
    delete_at(list, 5, &deleted);
    delete_at(list, 5, &deleted);
    printf("After deleting elements at index 5 (twice): ");
    print_list(list);
    
    // 修改一些元素
    set_at(list, 0, 100);
    set_at(list, get_length(list) - 1, 200);
    printf("After setting first to 100 and last to 200: ");
    print_list(list);
    
    // 查找和验证
    int pos = find_value(list, 5);
    printf("✓ Position of value 5: %d\n", pos);
    
    printf("✓ Final length: %d\n", get_length(list));
    
    destroy_sequential_list(list);
}

int main()
{
    printf("========================================\n");
    printf("  Sequential List Implementation Test\n");
    printf("========================================\n");
    
    test_create_and_destroy();
    test_basic_operations();
    test_insert_operations();
    test_delete_operations();
    test_search_operations();
    test_get_set_operations();
    test_boundary_conditions();
    test_comprehensive_scenario();
    
    printf("\n========================================\n");
    printf("  All Tests Passed! ✓\n");
    printf("========================================\n");
    
    return 0;
}
