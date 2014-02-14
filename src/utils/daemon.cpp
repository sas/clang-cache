#include "daemon.h"

#include <utils/logger.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace clc { namespace utils {

bool read_pidfile(const char* pid_path, pid_t* pid)
{
  FILE* f;

  f = fopen(pid_path, "r");
  if (f == NULL)
    return false;

  fscanf(f, "%d", pid);
  fclose(f);

  return true;
}

bool write_pidfile(const char* pid_path)
{
  FILE* f;

  f = fopen(pid_path, "w");
  if (f == NULL)
    return false;

  fprintf(f, "%d", getpid());
  fclose(f);

  return true;
}

bool daemonize(const char* pid_path)
{
  /* Double fork. */
  pid_t ret;

  PLOG_FATAL((ret = fork()) == -1) << "fork()";
  if (ret > 0) {
    waitpid(ret, NULL, 0);
    return false;
  }

  PLOG_FATAL((ret = fork()) == -1) << "fork()";
  if (ret > 0)
    exit(EXIT_SUCCESS);

  /* Write PID. */
  PLOG_FATAL(!write_pidfile(pid_path)) << pid_path;

  /* Change directory. */
  PLOG_WARN(chdir("/") == -1) << "/";

  /* Create new session ID. */
  PLOG_WARN(setsid() == -1) << "setsid()";

  /* Close standard streams. */
  int fd;
  if ((fd = open("/dev/null", O_RDWR)) >= 0)
    for (int i = 0; i < 3; ++i)
      dup2(fd, i);

  return true;
}

}} // namespace clc::utils
