#include <stdio.h>
#include <string.h>
static size_t my_strlen(const char *s) {
    const char *p = s;
    while (*p) {
        ++p;
    }
    return (size_t)(p - s);
}
static char *my_strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0') {
        ;
    }
    return dest;
}
static char *my_strcat(char *dest, const char *src) {
    char *d = dest + my_strlen(dest);
    while ((*d++ = *src++) != '\0') {
        ;
    }
    return dest;
}
static int my_strcmp(const char *lhs, const char *rhs) {
    while (*lhs && *lhs == *rhs) {
        ++lhs;
        ++rhs;
    }
    return (unsigned char)*lhs - (unsigned char)*rhs;
}
int main(void) {
    char buffer[64];
    my_strcpy(buffer, "Hello");
    my_strcat(buffer, ", world");
    size_t len = my_strlen(buffer);
    int cmp = my_strcmp(buffer, "Hello, world");
    printf("buffer=%s len=%zu cmp=%d\n", buffer, len, cmp);
    return 0;
}
