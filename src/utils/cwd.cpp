#include "cwd.h"

#include <utils/logger.h>

#include <limits.h>
#include <unistd.h>

namespace clc { namespace utils {

const char* cwd()
{
  static thread_local char buf[PATH_MAX + 1];

  PLOG_FATAL(getcwd(buf, sizeof buf) == NULL) << "getcwd()";

  return buf;
}

}} // namespace clc::utils
