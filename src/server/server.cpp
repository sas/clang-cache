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
  const auto sp = config::sock_path();
  int fd;

  fd = utils::create_socket(sp.c_str());
  if (fd == -1)
    err(EXIT_FAILURE, "%s", sp.c_str());

  while (42);
}

bool is_running()
{
  const auto pp = config::pid_path();
  pid_t pid;

  if (!utils::read_pidfile(pp.c_str(), &pid))
    return false;

  return kill(pid, 0) == 0;
}

void start()
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
  if (utils::daemonize(pp.c_str()))
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
