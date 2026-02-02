#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return 2;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open('%s') failed: %s\n", argv[1], strerror(errno));
		return 1;
	}

	off_t before = lseek(fd, 0, SEEK_CUR);
	if (before == (off_t)-1) {
		perror("lseek");
		close(fd);
		return 1;
	}

	char buf[16];
	ssize_t n = pread(fd, buf, sizeof(buf), 0);
	if (n == -1) {
		perror("pread");
		close(fd);
		return 1;
	}

	off_t after = lseek(fd, 0, SEEK_CUR);
	if (after == (off_t)-1) {
		perror("lseek");
		close(fd);
		return 1;
	}

	printf("lseek before=%lld, after=%lld\n", (long long)before, (long long)after);
	printf("pread first %zd bytes: ", n);
	for (ssize_t i = 0; i < n; i++) {
		unsigned char c = (unsigned char)buf[i];
		if (c >= 32 && c <= 126) {
			putchar((char)c);
		} else {
			printf("\\x%02x", c);
		}
	}
	putchar('\n');

	close(fd);
	return 0;
}
