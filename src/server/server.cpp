#include "server.h"

#include <utils/daemon.h>
#include <utils/logger.h>
#include <utils/mkdirp.h>
#include <utils/paths.h>

#include <boost/make_shared.hpp>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thrift/TProcessor.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TServerSocket.h>

#include <clc_if_types.h>
#include <clc_if.h>

namespace tprotocol = apache::thrift::protocol;
namespace ttransport = apache::thrift::transport;
namespace tserver = apache::thrift::server;

namespace clc { namespace server {

class clcService : public clc::rpc::clc_ifIf {
public:
  clcService() {}

  void complete(clc::rpc::completion_answer& ret, const clc::rpc::request& r)
  {
    (void)ret;
    (void)r;

    LOG_INFO() << "complete";
  }

  void jump_to_declaration(clc::rpc::jump_answer& ret, const clc::rpc::request& r)
  {
    (void)ret;
    (void)r;

    LOG_INFO() << "goto_dec";
  }

  void jump_to_definition(clc::rpc::jump_answer& ret, const clc::rpc::request& r)
  {
    (void)ret;
    (void)r;

    LOG_INFO() << "goto_def";
  }

  void add_file(const std::vector<std::string>& argv)
  {
    (void)argv;

    LOG_INFO() << "add_file";
  }
};

int main()
{
  const auto sp = utils::sock_path();

  auto service = boost::make_shared<clcService>();
  auto proc = boost::make_shared<clc::rpc::clc_ifProcessor>(service);
  auto server_transport = boost::make_shared<ttransport::TServerSocket>(sp);
  auto transport_factory = boost::make_shared<ttransport::TBufferedTransportFactory>();
  auto protocol_factory = boost::make_shared<tprotocol::TCompactProtocolFactory>();

  tserver::TSimpleServer server(proc, server_transport, transport_factory,
                                protocol_factory);

  server.serve();

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
    exit(main());
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
  PLOG_FATAL(unlink(pp.c_str()) == -1) << pp.c_str();
  LOG_INFO() << "stopped server with pid " << pid;
}

}} // namespace clc::server
