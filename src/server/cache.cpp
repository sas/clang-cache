#include "cache.h"

#include <utils/abspath.h>
#include <utils/compiler.h>
#include <utils/logger.h>
#include <utils/waitdir.h>

#include <unistd.h>

namespace clc { namespace server {

cache::cache()
  : clang_index_(clang_createIndex(0, 0))
{
}

void cache::fill(const std::vector<std::string>& argv,
                 const std::string& cwd)
{
  std::vector<const char*> strargv;

  strargv.reserve(argv.size());
  for (const auto& str : argv)
    strargv.push_back(str.c_str());

  if (!utils::waitdir(cwd.c_str())) {
    PLOG_WARN(true) << "chdir()";
    return;
  }

  auto tu = clang_createTranslationUnitFromSourceFile(clang_index_, NULL,
                                                      argv.size(),
                                                      strargv.data(),
                                                      0, NULL);
  auto path = clang_getTranslationUnitSpelling(tu);
  auto path_str = clang_getCString(path);
  auto abspath = utils::abspath(path_str, cwd.c_str());
  auto it = file_cache_.find(abspath);

  if (it == file_cache_.end()) {
    file_cache_[abspath] = { tu, {} };
  } else {
    clang_disposeTranslationUnit(tu);
    tu = it->second.tu;
  }

  if (clang_reparseTranslationUnit(tu, 0, NULL, 0) != 0)
    LOG_WARN() << "Fail to reparse.";

  /* Release the current working directory. */
  utils::waitdir();
}

}} // namespace clc::server
