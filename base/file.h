#ifndef LEARN_OPENGL_UTILS_FILE_H_
#define LEARN_OPENGL_UTILS_FILE_H_

#include <string>

namespace mybase {

bool ReadFdToString(int fd, std::string* content);
bool ReadFileToString(const std::string& path, std::string* content);

}  // namespace mybase

#endif  // LEARN_OPENGL_UTILS_FILE_H_