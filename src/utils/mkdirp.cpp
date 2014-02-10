#include "mkdirp.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace clc { namespace utils {

void mkdirp(const char* path)
{
  char* last;

  last = strrchr(path, '/');

  if (last != NULL && last != path) {
    *last = '\0';
    mkdirp(path);
    *last = '/';
  }

  if (mkdir(path, 0777) == -1 && errno != EEXIST)
    err(EXIT_FAILURE, "%s", path);
}

}} // namespace clc::utils
