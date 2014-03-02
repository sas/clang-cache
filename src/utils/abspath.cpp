#include "abspath.h"

#include <utils/logger.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace clc { namespace utils {

const char* abspath(const char* path, const char* base)
{
  static thread_local char buf[PATH_MAX + 1];
  char tmp[2 * PATH_MAX + 2];

  sprintf(tmp, "%s/%s", base != nullptr ? base : ".", path);
  PLOG_FATAL(realpath(tmp, buf) == NULL) << tmp;

  return buf;
}

}} // namespace clc::utils
