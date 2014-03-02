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

void clc_service::complete(ATTR_UNUSED clc::rpc::completion_answer& ret,
                           ATTR_UNUSED const clc::rpc::request& r)
{
  LOG_INFO() << __FUNCTION__;
}

void clc_service::find_declaration(ATTR_UNUSED clc::rpc::jump_answer& ret,
                                   ATTR_UNUSED const clc::rpc::request& r)
{
  LOG_INFO() << __FUNCTION__;
}

void clc_service::find_definition(ATTR_UNUSED clc::rpc::jump_answer& ret,
                                  ATTR_UNUSED const clc::rpc::request& r)
{
  LOG_INFO() << __FUNCTION__;
}

}} // namespace clc::server
