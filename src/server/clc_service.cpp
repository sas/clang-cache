#include "clc_service.h"

#include <utils/compiler.h>
#include <utils/logger.h>

#include <string>
#include <utility>

namespace clc { namespace server {

void clc_service::register_compilation(const std::vector<std::string>& argv,
                                       const std::string& cwd)
{
  LOG_INFO() << __FUNCTION__;

  cache_.fill(argv, cwd);
}

void clc_service::find_definition(ATTR_UNUSED clc::rpc::cursor& ret,
                                  ATTR_UNUSED const std::string& usr)
{
  LOG_INFO() << __FUNCTION__;
}

}} // namespace clc::server
