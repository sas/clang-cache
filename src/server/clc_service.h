#pragma once

#include <clc_if.h>
#include <server/cache.h>

#include <string>
#include <vector>

namespace clc { namespace server {

class clc_service : public clc::rpc::clc_ifIf
{
public:
  virtual void register_compilation(const std::vector<std::string>& argv);
  virtual void complete(clc::rpc::completion_answer& ret,
                        const clc::rpc::request& r);
  virtual void find_declaration(clc::rpc::jump_answer& ret,
                                const clc::rpc::request& r);
  virtual void find_definition(clc::rpc::jump_answer& ret,
                               const clc::rpc::request& r);

public:
  cache cache_;
};

}} // namespace clc::server
