#include "cache.h"

#include <utils/logger.h>

namespace clc { namespace server {

cache::cache()
  : clang_index_(clang_createIndex(0, 0))
{
}

void cache::fill(const std::vector<std::string>& argv)
{
  std::vector<const char*> strargv;

  strargv.reserve(argv.size());
  for (const auto& str : argv)
    strargv.push_back(str.c_str());

  auto tu = clang_createTranslationUnitFromSourceFile(clang_index_, NULL,
                                                      argv.size(),
                                                      strargv.data(),
                                                      0, NULL);
  auto tu_filename = clang_getTranslationUnitSpelling(tu);
  std::string filename = clang_getCString(tu_filename);
  auto it = file_cache_.find(filename);

  if (it == file_cache_.end()) {
    file_cache_[filename] = { tu, {} };
  } else {
    clang_disposeTranslationUnit(tu);
    tu = it->second.tu;
  }

  if (clang_reparseTranslationUnit(tu, 0, NULL, 0) != 0)
    LOG_WARN() << "Fail to reparse.";
}

}} // namespace clc::server
