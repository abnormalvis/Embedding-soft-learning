#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    FILE *in = stdin;
    FILE *out = stdout;
    if (argc == 3) {
        in = fopen(argv[1], "r");
        if (!in) {
            perror("fopen input");
            return 1;
        }
        out = fopen(argv[2], "w");
        if (!out) {
            perror("fopen output");
            fclose(in);
            return 1;
        }
    }
    char buffer[256];
    size_t total = 0;
    while (fgets(buffer, sizeof buffer, in)) {
        size_t len = strlen(buffer);
        if (fwrite(buffer, 1, len, out) != len) {
            perror("fwrite");
            if (in != stdin) {
                fclose(in);
            }
            if (out != stdout) {
                fclose(out);
            }
            return 1;
        }
        ++total;
    }
    if (ferror(in)) {
        perror("fgets");
    }
    fprintf(stderr, "processed %zu line(s)\n", total);
    if (in != stdin) {
        fclose(in);
    }
    if (out != stdout) {
        fflush(out);
        fclose(out);
    }
    return ferror(in) ? 1 : 0;
}
