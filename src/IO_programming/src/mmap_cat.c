#include "io_utils.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
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

	struct stat st;
	if (fstat(fd, &st) == -1) {
		perror("fstat");
		close(fd);
		return 1;
	}

	if (st.st_size == 0) {
		close(fd);
		return 0;
	}

	void *addr = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 1;
	}

	if (write_all(STDOUT_FILENO, addr, (size_t)st.st_size) == -1) {
		perror("write");
	}

	munmap(addr, (size_t)st.st_size);
	close(fd);
	return 0;
}
