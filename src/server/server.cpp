#include "server.h"

#include <config.h>
#include <utils/daemon.h>
#include <utils/mkdirp.h>
#include <utils/socket.h>

#include <err.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace clc { namespace server {

int main()
{
  const auto sp = config::sock_path();
  int fd;
  int client_fd;

  fd = utils::create_socket(sp.c_str());
  if (fd == -1)
    err(EXIT_FAILURE, "%s", sp.c_str());

  while ((client_fd = accept(fd, NULL, NULL)) != -1) {
    close(client_fd);
  }

  return EXIT_FAILURE;
}

bool is_running()
{
  const auto pp = config::pid_path();
  pid_t pid;

  if (!utils::read_pidfile(pp.c_str(), &pid))
    return false;

  return kill(pid, 0) == 0;
}

void start(bool foreground)
{
  const auto rp = config::run_path();
  const auto pp = config::pid_path();

  if (is_running()) {
    warnx("server already running");
    return;
  }

  /* Create run dir. */
  utils::mkdirp(rp.c_str());

  /* Create the server and run the main function. */
  if (foreground || utils::daemonize(pp.c_str()))
    exit(main());
}

void stop()
{
  const auto pp = config::pid_path();
  pid_t pid;

  if (!is_running()) {
    warnx("server not running");
    return;
  }

  if (!utils::read_pidfile(pp.c_str(), &pid))
    err(EXIT_FAILURE, "%s", pp.c_str());

  if (kill(pid, SIGTERM) == -1)
    err(EXIT_FAILURE, "process %d", pid);

  if (unlink(pp.c_str()) == -1)
    err(EXIT_FAILURE, "%s", pp.c_str());
}

}} // namespace clc::server
