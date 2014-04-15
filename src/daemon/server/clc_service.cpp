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

void clc_service::find_definition_from_usr(clc::rpc::cursor& ret,
                                           const std::string& usr)
{
  auto sl = cache_.find_definition(usr);

  ret.file = sl.path;
  ret.line = sl.line;
  ret.column = sl.column;
}

void clc_service::find_definition_from_cursor(clc::rpc::cursor& ret,
                                              const clc::rpc::cursor& c)
{
  struct cache::source_location sl;

  sl.path = c.file;
  sl.line = c.line;
  sl.column = c.column;

  const auto& usr = cache_.get_usr(sl);

  if (usr == "") {
    ret.file = "";
    ret.line = -1;
    ret.column = -1;
  } else {
    LOG_INFO() << "querying `" << usr << "'";
    find_definition_from_usr(ret, usr);
  }
}

}} // namespace clc::server
