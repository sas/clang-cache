#include "daemon.h"

#include <err.h>
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

  ret = fork();
  if (ret == -1) {
    err(EXIT_FAILURE, "fork()");
  } else if (ret > 0) {
    waitpid(ret, NULL, 0);
    return false;
  }

  ret = fork();
  if (ret == -1) {
    err(EXIT_FAILURE, "fork()");
  } else if (ret > 0) {
    exit(EXIT_SUCCESS);
  }

  /* Write PID. */
  if (!write_pidfile(pid_path))
    err(EXIT_FAILURE, "%s", pid_path);

  /* Change directory. */
  if (chdir("/") == -1)
    warn("%s", "/");

  /* Create new session ID. */
  if (setsid() == -1)
    warn("setsid()");

  /* Close standard streams. */
  int fd;
  if ((fd = open("/dev/null", O_RDWR)) >= 0)
    for (int i = 0; i < 3; ++i)
      dup2(fd, i);

  return true;
}

}} // namespace clc::utils
