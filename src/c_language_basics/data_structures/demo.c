#include <stdio.h>
#include <string.h>
#include <stddef.h>
struct Student {
    int id;
    char name[32];
    double score;
};
union Payload {
    int i;
    double d;
    char tag[4];
};
enum RecordType {
    RECORD_EMPTY,
    RECORD_GRADE,
    RECORD_NOTE
};
int main(void) {
    struct Student alice = { .id = 1, .name = "Alice", .score = 93.5 };
    union Payload payload = { .d = 42.0 };
    enum RecordType type = RECORD_GRADE;
    printf("sizeof(Student)=%zu offset(name)=%zu\n", sizeof alice, offsetof(struct Student, name));
    printf("student: %d %s %.1f\n", alice.id, alice.name, alice.score);
    if (type == RECORD_GRADE) {
        printf("payload double=%.1f\n", payload.d);
    }
    memcpy(payload.tag, "OK", 3);
    payload.tag[3] = '\0';
    type = RECORD_NOTE;
    if (type == RECORD_NOTE) {
        printf("payload tag=%s\n", payload.tag);
    }
    return 0;
}
