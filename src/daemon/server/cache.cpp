#include "cache.h"

#include <utils/abspath.h>
#include <utils/compiler.h>
#include <utils/logger.h>
#include <utils/waitdir.h>

#include <memory>
#include <unistd.h>

namespace clc { namespace server {

cache::cache()
  : clang_index_(clang_createIndex(0, 0))
{
}

enum CXChildVisitResult cache::fill_visitor_(CXCursor cursor,
                                             ATTR_UNUSED CXCursor parent,
                                             CXClientData client_data)
{
  auto data = (fill_visitor_data*) client_data;

  switch (clang_getCursorKind(cursor)) {
    case CXCursor_FunctionDecl: {
      if (!clang_isCursorDefinition(cursor))
        break;

      auto& path = data->file_cache_entry.first;
      auto& finfo = data->file_cache_entry.second;
      auto& defs = data->definition_cache;
      const auto& usr = clang_getCString(clang_getCursorUSR(cursor));
      const auto& sloc = clang_getCursorLocation(cursor);

      auto slp = std::make_shared<source_location>();

      slp->path = path;
      clang_getSpellingLocation(sloc, NULL, &slp->line, &slp->column, NULL);

      finfo.definitions[usr] = slp;
      defs[usr] = slp;

      LOG_INFO() << "populating cache for `" << usr << "'";

      break;
    }
    default:
      /* ignore */
      break;
  }

  return CXChildVisit_Recurse;
}

void cache::fill(const std::vector<std::string>& argv,
                 const std::string& cwd)
{
  std::vector<const char*> strargv;

  /* Convert cmdline to char**. */
  strargv.reserve(argv.size());
  for (const auto& str : argv)
    strargv.push_back(str.c_str());

  /* Wait until we are in the right directory. */
  if (!utils::waitdir(cwd.c_str())) {
    PLOG_WARN(true) << "chdir()";
    return;
  }

  /* Create the translation unit and extract some info. */
  auto tu = clang_createTranslationUnitFromSourceFile(clang_index_, NULL,
                                                      argv.size(),
                                                      strargv.data(),
                                                      0, NULL);
  auto path = clang_getTranslationUnitSpelling(tu);
  auto path_str = clang_getCString(path);
  auto abspath = utils::abspath(path_str, cwd.c_str());

  auto it = file_cache_.find(abspath);

  /* If this file does not aready exists... */
  if (it == file_cache_.end()) {
    /* ...create a new cache entry. */
    /* This gets the iterator to the new element that we can use later. */
    file_info fi = { tu, {} };
    it = file_cache_.insert(it, { abspath, fi });
  } else {
    /*
     * ...otherwise, dispose the newly created TU so we can just reparse the old
     * one, and invalidate the old definitions cache.
     */
    clang_disposeTranslationUnit(tu);
    tu = it->second.tu;
    it->second.definitions.clear();
  }

  if (clang_reparseTranslationUnit(tu, 0, NULL, 0) != 0)
    LOG_WARN() << "Fail to reparse.";

  fill_visitor_data fvd = {
    *it,
    this->definition_cache_
  };
  clang_visitChildren(clang_getTranslationUnitCursor(tu), fill_visitor_, &fvd);

  /* Release the current working directory. */
  utils::waitdir();
}

const std::string cache::get_usr(const source_location& loc)
{
  const auto& it = this->file_cache_.find(loc.path);
  if (it == file_cache_.end()) {
    return "";
  } else {
    const auto tu = it->second.tu;
    const auto& clang_file = clang_getFile(tu, loc.path.c_str());
    const auto& clang_loc = clang_getLocation(tu, clang_file,
                                              loc.line, loc.column);
    const auto& clang_cursor = clang_getCursor(tu, clang_loc);
    if (clang_Cursor_isNull(clang_cursor))
      return "";

    const auto& ref_cursor = clang_getCursorReferenced(clang_cursor);
    if (clang_Cursor_isNull(ref_cursor))
      return "";

    return clang_getCString(clang_getCursorUSR(ref_cursor));
  }
}

cache::source_location cache::find_definition(const std::string& usr)
{
  auto it = definition_cache_.find(usr);

  if (it == definition_cache_.end())
    /* XXX: This sucks, we need real error reporting here. */
    return source_location();

  auto observe = it->second.lock();
  return observe ? *observe : source_location();
}

}} // namespace clc::server
