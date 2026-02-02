#include "io_utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    return 1;
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    // child: write
    close(pipefd[0]);
    const char *msg = "hello from child via pipe\n";
    if (write_all(pipefd[1], msg, strlen(msg)) == -1) {
      perror("write");
      close(pipefd[1]);
      return 1;
    }
    close(pipefd[1]);
    return 0;
  }

  // parent: read
  close(pipefd[1]);
  char buf[256];
  for (;;) {
    ssize_t n = read(pipefd[0], buf, sizeof(buf));
    if (n > 0) {
      if (write_all(STDOUT_FILENO, buf, (size_t)n) == -1) {
        perror("write");
        break;
      }
      continue;
    }
    if (n == 0) {
      break;
    }
    if (errno == EINTR) {
      continue;
    }
    perror("read");
    break;
  }
  close(pipefd[0]);

  int status = 0;
  waitpid(pid, &status, 0);
  return 0;
}
