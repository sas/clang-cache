#include "daemon.h"

#include "config.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace clc { namespace daemon {

bool is_running()
{
  struct stat buf;
  int ret;

  ret = stat(clc::config::sock_path, &buf);

  return ret == 0 && S_ISSOCK(buf.st_mode);
}

}} // namespace clc::daemon
