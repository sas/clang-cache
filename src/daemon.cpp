#include "daemon.h"

#include "config.h"
#include "utils/mkdirp.h"

#include <cstdlib>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

namespace clc { namespace daemon {

bool is_running()
{
  struct stat buf;
  int ret;

  ret = stat(clc::config::sock_path().c_str(), &buf);

  return ret == 0 && S_ISSOCK(buf.st_mode);
}

bool daemonize()
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

  /* Create run dir. */
  clc::utils::mkdirp(clc::config::run_path().c_str());

  /* Write PID. */
  if (FILE* f = fopen(clc::config::pid_path().c_str(), "w")) {
    fprintf(f, "%d", getpid());
    fclose(f);
  } else {
    err(EXIT_FAILURE, "%s", clc::config::pid_path().c_str());
  }

  /* Change directory. */
  if (chdir("/") == -1)
    warn("%s", "/");

  /* Create new session ID. */
  if (setsid() == -1)
    warn("setsid()");

  /* Close standard streams. */
  if (int fd = open("/dev/null", O_RDWR) > 0)
    for (int i = 0; i < 3; ++i)
      dup2(fd, i);

  return true;
}

int daemon_main()
{
  return 0;
}

void start()
{
  if (is_running())
    return;

  std::cout << "Starting daemon." << std::endl;

  if (daemonize())
    exit(daemon_main());
}

}} // namespace clc::daemon
