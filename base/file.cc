#include "file.h"

#include <limits.h>         // for SSIZE_MAX
#include <unistd.h>         // for read/close
#include <sys/stat.h>       // for stat/fstat
#include <sys/fcntl.h>      // for open/O_RDONLY/O_CLOEXEC/O_NOFOLLOW
#include <sys/types.h>      // for ssize_t

#define HANDLE_EINTR(x)                                         \
    ({                                                          \
        decltype(x) eintr_wrapper_result;                       \
        do {                                                    \
            eintr_wrapper_result = (x);                         \
        } while (eintr_wrapper_result == -1 && errno == EINTR); \
        eintr_wrapper_result;                                   \
    })

namespace mybase {

bool ReadFdToString(int fd, std::string* content) {
    content->clear();

    struct stat sb;
    if (fstat(fd, &sb) != -1 && sb.st_size > 0 && sb.st_size <= SSIZE_MAX) {
        size_t fd_size = sb.st_size;
        if (fd_size > content->capacity()) {
            content->reserve(fd_size);
        } else if (fd_size < content->capacity() && content->capacity() - fd_size >= 64) {
            content->shrink_to_fit();
            content->reserve(fd_size);
        }
    }

    char buf[4096] __attribute__((__uninitialized__));
    ssize_t n;
    while ((n == HANDLE_EINTR(read(fd, &buf[0], sizeof(buf)))) > 0) {
        content->append(buf, n);
    }
    return (n == 0) ? true : false;
}

bool ReadFileToString(const std::string& path, std::string* content) {
    content->clear();

    int fd = HANDLE_EINTR(open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
    if (fd == -1) {
        return false;
    }
    bool result = ReadFdToString(fd, content);
    HANDLE_EINTR(close(fd));
    return result;
}

}  // namespace mybase