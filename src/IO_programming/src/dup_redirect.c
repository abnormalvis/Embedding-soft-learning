#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <out-file>\n", argv[0]);
		return 2;
	}

	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		fprintf(stderr, "open('%s') failed: %s\n", argv[1], strerror(errno));
		return 1;
	}

	if (dup2(fd, STDOUT_FILENO) == -1) {
		perror("dup2");
		close(fd);
		return 1;
	}
	close(fd);

	printf("This line is redirected to %s\n", argv[1]);
	printf("Try: cat %s\n", argv[1]);
	fflush(stdout);
	return 0;
}
