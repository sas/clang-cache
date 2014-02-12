#include "mkdirp.h"

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace clc { namespace utils {

void mkdirp_rec(char* path)
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

void mkdirp(const char* path)
{
  char* path_copy;

  path_copy = strdup(path);
  mkdirp_rec(path_copy);
  free(path_copy);
}

}} // namespace clc::utils
