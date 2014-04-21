#include "server.h"

/*
 * Include signal.h before all the others, to avoid issue with thrift
 * including it in the namespace apache::thrift.
 */
#include <signal.h>

#include <server/clc_service.h>
#include <utils/compiler.h>
#include <utils/daemon.h>
#include <utils/logger.h>
#include <utils/mkdirp.h>
#include <utils/paths.h>

#include <boost/make_shared.hpp>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thrift/TProcessor.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TServerSocket.h>

namespace clc { namespace server {

namespace tprotocol = apache::thrift::protocol;
namespace ttransport = apache::thrift::transport;
namespace tserver = apache::thrift::server;

static tserver::TSimpleServer *server;

static void stop_server(ATTR_UNUSED int sig)
{
  if (server != NULL)
    server->stop();
}

int run()
{
  const auto sp = utils::sock_path();
  const auto pp = utils::pid_path();

  auto service = boost::make_shared<clc_service>();
  auto proc = boost::make_shared<clc::rpc::clc_ifProcessor>(service);
  auto server_transport = boost::make_shared<ttransport::TServerSocket>(sp);
  auto transport_factory = boost::make_shared<ttransport::TBufferedTransportFactory>();
  auto protocol_factory = boost::make_shared<tprotocol::TCompactProtocolFactory>();

  signal(SIGTERM, stop_server);
  signal(SIGINT, stop_server);

  server = new tserver::TSimpleServer(proc, server_transport, transport_factory,
                                      protocol_factory);
  server->serve();

  delete server;

  PLOG_FATAL(unlink(pp.c_str()) == -1) << pp.c_str();
  PLOG_FATAL(unlink(sp.c_str()) == -1) << sp.c_str();

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
  pid_t pid;

  if (is_running()) {
    LOG_WARN() << "server already running";
    return;
  }

  /* Create run dir. */
  utils::mkdirp(rp.c_str());

  /* Create the server and run the main function. */
  if (foreground || utils::daemonize()) {
    PLOG_FATAL(!utils::write_pidfile(pp.c_str())) << pp.c_str();
    if (!foreground)
      SET_OUT(utils::log_path());
    exit(run());
  }

  PLOG_FATAL(!utils::read_pidfile(pp.c_str(), &pid)) << pp.c_str();
  LOG_INFO() << "started server with pid " << pid;
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
  LOG_INFO() << "stopped server with pid " << pid;
}

}} // namespace clc::server
