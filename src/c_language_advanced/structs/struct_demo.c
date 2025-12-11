#include <stdio.h>
#include <string.h>
#include <stddef.h>

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point top_left;
    Point *center;
    int width;
    int height;
} Rectangle;

typedef struct {
    int id;
    char name[32];
    double score;
} Student;

typedef struct {
    size_t count;
    int data[];
} FlexArray;

static void demo_basic_struct(void) {
    puts("=== Basic Struct ===");
    Student alice = {1, "Alice", 95.5};
    printf("Student: id=%d name=%s score=%.1f\n", alice.id, alice.name, alice.score);
    
    printf("sizeof(Student)=%zu\n", sizeof(Student));
    printf("offset of name=%zu score=%zu\n", 
           offsetof(Student, name), offsetof(Student, score));
}

static void demo_nested_struct(void) {
    puts("\n=== Nested Struct ===");
    Point center = {50, 50};
    Rectangle rect = {{0, 0}, &center, 100, 80};
    
    printf("top_left=(%d,%d) center=(%d,%d) size=%dx%d\n",
           rect.top_left.x, rect.top_left.y,
           rect.center->x, rect.center->y,
           rect.width, rect.height);
}

static void demo_struct_array(void) {
    puts("\n=== Struct Array ===");
    Student students[] = {
        {1, "Alice", 90.5},
        {2, "Bob", 85.0},
        {3, "Carol", 92.3}
    };
    
    size_t count = sizeof(students) / sizeof(students[0]);
    for (size_t i = 0; i < count; ++i) {
        printf("ID=%d Name=%-10s Score=%.1f\n", 
               students[i].id, students[i].name, students[i].score);
    }
    
    // 指针访问
    Student *ptr = &students[1];
    ptr->score += 5.0;
    printf("Bob's updated score=%.1f\n", ptr->score);
}

static void demo_flexible_array(void) {
    puts("\n=== Flexible Array Member ===");
    size_t n = 5;
    FlexArray *arr = malloc(sizeof(FlexArray) + n * sizeof(int));
    if (!arr) {
        perror("malloc");
        return;
    }
    
    arr->count = n;
    for (size_t i = 0; i < n; ++i) {
        arr->data[i] = (int)(i * 10);
    }
    
    printf("FlexArray: count=%zu data=[", arr->count);
    for (size_t i = 0; i < arr->count; ++i) {
        printf("%d%s", arr->data[i], i + 1 < arr->count ? ", " : "");
    }
    puts("]");
    
    free(arr);
}

int main(void) {
    demo_basic_struct();
    demo_nested_struct();
    demo_struct_array();
    demo_flexible_array();
    return 0;
}
