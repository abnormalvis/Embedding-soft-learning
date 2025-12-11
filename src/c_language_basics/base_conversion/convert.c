#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void usage(const char *program) {
    fprintf(stderr, "usage: %s <number> [base]\n", program);
    fprintf(stderr, "example: %s FF 16\n", program);
}
int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return 1;
    }
    const char *text = argv[1];
    int base = 0;
    if (argc == 3) {
        base = atoi(argv[2]);
    }
    errno = 0;
    char *end = NULL;
    long value = strtol(text, &end, base);
    if (errno != 0 || end == text || *end != '\0') {
        fprintf(stderr, "invalid input: %s\n", text);
        return 1;
    }
    printf("decimal: %ld\n", value);
    printf("octal: %lo\n", value);
    printf("hex: %lX\n", value);
    printf("binary: ");
    for (int i = (int)(sizeof value * 8) - 1; i >= 0; --i) {
        putchar(((value >> i) & 1L) ? '1' : '0');
    }
    putchar('\n');
    return 0;
}
