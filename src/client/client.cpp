#include "client.h"

#include <clc_if.h>
#include <clc_if_types.h>
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

void send_file(int argc, char** argv)
{
  auto socket = boost::make_shared<ttransport::TSocket>(utils::sock_path());
  auto transport = boost::make_shared<ttransport::TBufferedTransport>(socket);
  auto protocol = boost::make_shared<tprotocol::TCompactProtocol>(transport);

  clc::rpc::clc_ifClient client(protocol);

  do {
    try {
      /*
       * When the server is being launched, we might end-up here before
       * the server has created the unix socket. Let's loop until it has
       * been, this shouldn't be long.
       */
      transport->open();
    } catch (ttransport::TTransportException& e) {
      /*
       * I wonder if that is a bug in thrift...
       */
      transport->close();
    }
  } while (!transport->isOpen());

  std::vector<std::string> cmd(argv, argv + argc);

  client.add_file(cmd);

  transport->close();
}

}} // namespace clc::client
