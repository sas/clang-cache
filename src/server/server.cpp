#include "server.h"

#include <config.h>
#include <utils/daemon.h>
#include <utils/mkdirp.h>
#include <utils/socket.h>

#include <err.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace clc { namespace server {

int main()
{
  const char* sock_path = config::sock_path().c_str();
  int fd;

  fd = utils::create_socket(sock_path);
  if (fd == -1)
    err(EXIT_FAILURE, "%s", sock_path);

  while (42);
}

bool is_running()
{
  struct stat buf;

  return stat(config::sock_path().c_str(), &buf) == 0 && S_ISSOCK(buf.st_mode);
}

void start()
{
  const char* pid_path = config::pid_path().c_str();

  if (is_running()) {
    warnx("server already running");
    return;
  }

  /* Create run dir. */
  utils::mkdirp(config::run_path().c_str());

  /* Create the server and run the main function. */
  if (utils::daemonize(pid_path))
    exit(main());
}

void stop()
{
  const char* pid_path = config::pid_path().c_str();
  pid_t pid;

  if (!is_running()) {
    warnx("server not running");
    return;
  }

  if (!utils::read_pidfile(pid_path, &pid))
    err(EXIT_FAILURE, "%s", pid_path);

  if (kill(pid, SIGTERM) == -1)
    err(EXIT_FAILURE, "process %d", pid);

  if (unlink(pid_path) == -1)
    err(EXIT_FAILURE, "%s", pid_path);
}

}} // namespace clc::server
