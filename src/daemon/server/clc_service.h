#pragma once

#include <clc_if.h>
#include <server/cache.h>

#include <string>
#include <vector>

namespace clc { namespace server {

class clc_service : public clc::rpc::clc_ifIf
{
public:
  virtual void register_compilation(const std::vector<std::string>& argv,
                                    const std::string& cwd);
  virtual void find_definition_from_usr(clc::rpc::cursor& ret,
                                        const std::string& usr);
  virtual void find_definition_from_cursor(clc::rpc::cursor& ret,
                                           const clc::rpc::cursor& c);

public:
  cache cache_;
};

}} // namespace clc::server
