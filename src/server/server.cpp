#include "server.h"

#include <utils/daemon.h>
#include <utils/logger.h>
#include <utils/mkdirp.h>
#include <utils/paths.h>
#include <utils/socket.h>

#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace clc { namespace server {

int main()
{
  const auto sp = utils::sock_path();
  int fd;
  int client_fd;

  PLOG_FATAL((fd = utils::create_socket(sp.c_str())) == -1) << sp.c_str();

  while ((client_fd = accept(fd, NULL, NULL)) != -1) {
    close(client_fd);
  }

  return EXIT_FAILURE;
}

bool is_running()
{
  const auto pp = utils::pid_path();
  pid_t pid;

  if (!utils::read_pidfile(pp.c_str(), &pid))
    return false;

  return kill(pid, 0) == 0;
}

void start(bool foreground)
{
  const auto rp = utils::run_path();
  const auto pp = utils::pid_path();

  if (is_running()) {
    LOG_WARN() << "server already running";
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
  const auto pp = utils::pid_path();
  pid_t pid;

  if (!is_running()) {
    LOG_WARN() << "server not running";
    return;
  }

  PLOG_FATAL(!utils::read_pidfile(pp.c_str(), &pid)) << pp.c_str();
  PLOG_FATAL(kill(pid, SIGTERM) == -1) << "process " << pid;
  PLOG_FATAL(unlink(pp.c_str()) == -1) << pp.c_str();
}

}} // namespace clc::server
