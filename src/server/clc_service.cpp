#include "clc_service.h"

#include <utils/compiler.h>
#include <utils/logger.h>

#include <utility>

namespace clc { namespace server {

clc_service::clc_service()
{
  clang_index_ = clang_createIndex(0, 0);
}

void clc_service::complete(ATTR_UNUSED clc::rpc::completion_answer& ret,
                           ATTR_UNUSED const clc::rpc::request& r)
{
  LOG_INFO() << "complete";
}

void clc_service::jump_to_declaration(ATTR_UNUSED clc::rpc::jump_answer& ret,
                                      ATTR_UNUSED const clc::rpc::request& r)
{
  LOG_INFO() << "goto_dec";
}

void clc_service::jump_to_definition(ATTR_UNUSED clc::rpc::jump_answer& ret,
                                     ATTR_UNUSED const clc::rpc::request& r)
{
  LOG_INFO() << "goto_def";
}

void clc_service::add_file(const std::vector<std::string>& argv)
{
  std::vector<const char*> strargv;

  strargv.reserve(argv.size());

  for (const auto& str : argv) {
    strargv.push_back(str.c_str());
  }

  auto tu = clang_createTranslationUnitFromSourceFile(clang_index_, NULL,
                                                      argv.size(),
                                                      strargv.data(),
                                                      0, NULL);
  auto tu_filename = clang_getTranslationUnitSpelling(tu);
  auto filename = clang_getCString(tu_filename);
  auto it = open_files_.find(filename);
  if (it == open_files_.end()) {
    open_files_[filename] = tu;
  } else {
    clang_disposeTranslationUnit(tu);
    tu = it->second;
  }
  PLOG_INFO((clang_reparseTranslationUnit(tu, 0, NULL, 0) != 0))
    << "Fail to reparse.";
}

}} // namespace clc::server
