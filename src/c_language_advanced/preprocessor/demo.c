#include <stdio.h>
#define STR(x) #x
#define CONCAT(a, b) a##b
#define ERROR_TABLE \
    X(ERR_OK, "ok") \
    X(ERR_IO, "io error") \
    X(ERR_TIMEOUT, "timeout")

typedef enum {
#define X(name, msg) name,
    ERROR_TABLE
#undef X
} ErrorCode;
static const char *error_to_string(ErrorCode code) {
    switch (code) {
#define X(name, msg) case name: return msg;
        ERROR_TABLE
#undef X
    default:
        return "unknown";
    }
}
int main(void) {
    puts(STR(hello world));
    int CONCAT(value, 1) = 42;
    printf("value1=%d message=%s\n", value1, error_to_string(ERR_TIMEOUT));
    return 0;
}
