#include "mkdirp.h"

#include <utils/logger.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace clc { namespace utils {

void mkdirp(char* path)
{
  char* last;

  last = strrchr(path, '/');

  if (last != NULL && last != path) {
    *last = '\0';
    mkdirp(path);
    *last = '/';
  }

  PLOG_FATAL(mkdir(path, 0777) == -1 && errno != EEXIST) << path;
}

void mkdirp(const char* path)
{
  char* path_copy;

  path_copy = strdup(path);
  mkdirp(path_copy);
  free(path_copy);
}

}} // namespace clc::utils
