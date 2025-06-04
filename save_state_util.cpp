#include "save_state_util.h"
#include <sys/stat.h>

bool FileExists(const std::string &path) {
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}
