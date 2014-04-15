#include "client.h"

#include <clc_if.h>
#include <clc_if_types.h>
#include <utils/cwd.h>
#include <utils/logger.h>
#include <utils/paths.h>

#include <boost/make_shared.hpp>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <vector>

namespace tprotocol = apache::thrift::protocol;
namespace ttransport = apache::thrift::transport;

namespace clc { namespace client {

static clc::rpc::clc_ifClient* connect_to_server(void)
{
  auto socket = boost::make_shared<ttransport::TSocket>(utils::sock_path());
  auto transport = boost::make_shared<ttransport::TBufferedTransport>(socket);
  auto protocol = boost::make_shared<tprotocol::TCompactProtocol>(transport);
  auto client = new clc::rpc::clc_ifClient(protocol);

  do {
    try {
      /*
       * When the server is being launched, we might end-up here before
       * the server has created the unix socket. Let's loop until it has
       * been, this shouldn't be long.
       */
      transport->open();
    } catch (ttransport::TTransportException& e) {
      /* I wonder if that is a bug in thrift. */
      transport->close();
    }
  } while (!transport->isOpen());

  return client;
}

void register_compilation(int argc, char** argv)
{
  auto client = connect_to_server();
  auto transport = client->getOutputProtocol()->getTransport();

  std::vector<std::string> cmd(argv, argv + argc);
  client->register_compilation(cmd, utils::cwd());
  transport->close();

  delete client;
}

void find_definition(const char* usr)
{
  auto client = connect_to_server();
  auto transport = client->getOutputProtocol()->getTransport();

  rpc::cursor ret;
  std::string arg(usr);
  client->find_definition_from_usr(ret, arg);
  transport->close();

  delete client;

  LOG_INFO() << "symbol " << arg << " is defined in " << ret.file << ":" << ret.line;
}

}} // namespace clc::client
