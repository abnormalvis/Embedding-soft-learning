#include "io_utils.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int cat_fd(int fd) {
	char buf[4096];
	for (;;) {
		ssize_t n = read(fd, buf, sizeof(buf));
		if (n > 0) {
			if (write_all(STDOUT_FILENO, buf, (size_t)n) == -1) {
				perror("write");
				return 1;
			}
			continue;
		}
		if (n == 0) {
			return 0;
		}
		if (errno == EINTR) {
			continue;
		}
		perror("read");
		return 1;
	}
}

int main(int argc, char **argv) {
	if (argc == 1) {
		return cat_fd(STDIN_FILENO);
	}

	if (argc != 2) {
		fprintf(stderr, "Usage: %s [file]\n", argv[0]);
		return 2;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open('%s') failed: %s\n", argv[1], strerror(errno));
		return 1;
	}

	int rc = cat_fd(fd);
	close(fd);
	return rc;
}
