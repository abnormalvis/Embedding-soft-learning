#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t write_all(int fd, const void *buf, size_t count);

#ifdef __cplusplus
}
#endif
