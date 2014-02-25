#pragma once

#include <clc_if.h>

#include <clang-c/Index.h>
#include <map>
#include <string>

namespace clc { namespace server {

class clc_service : public clc::rpc::clc_ifIf {
public:
  clc_service();

  virtual void complete(clc::rpc::completion_answer& ret,
                        const clc::rpc::request& r);

  virtual void jump_to_declaration(clc::rpc::jump_answer& ret,
                                   const clc::rpc::request& r);

  virtual void jump_to_definition(clc::rpc::jump_answer& ret,
                                  const clc::rpc::request& r);

  virtual void add_file(const std::vector<std::string>& argv);

private:
  CXIndex clang_index_;
  std::map<std::string, CXTranslationUnit> open_files_;
};

}} // namespace clc::server
