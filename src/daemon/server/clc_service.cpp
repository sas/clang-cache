#include "clc_service.h"

#include <utils/compiler.h>
#include <utils/logger.h>

#include <string>
#include <utility>

namespace clc { namespace server {

void clc_service::register_compilation(const std::vector<std::string>& argv,
                                       const std::string& cwd)
{
  cache_.fill(argv, cwd);
}

void clc_service::find_definition(clc::rpc::cursor& ret,
                                  const std::string& usr)
{
  auto sl = cache_.find_definition(usr);

  ret.file = sl.path;
  ret.line = sl.line;
  ret.column = sl.column;
}

}} // namespace clc::server
