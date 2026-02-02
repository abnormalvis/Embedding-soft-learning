#include "io_utils.h"

#include <errno.h>
#include <unistd.h>

ssize_t write_all(int fd, const void *buf, size_t count) {
	const char *p = (const char *)buf;
	size_t left = count;

	while (left > 0) {
		ssize_t n = write(fd, p, left);
		if (n > 0) {
			p += n;
			left -= (size_t)n;
			continue;
		}
		if (n == -1 && errno == EINTR) {
			continue;
		}
		return -1;
	}

	return (ssize_t)count;
}
