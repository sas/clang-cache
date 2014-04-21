#pragma once

#include <clang-c/Index.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace clc { namespace server {

class cache
{
public:
  struct source_location
  {
    std::string   path;
    std::uint32_t line;
    std::uint32_t column;
  };

  cache();
  void fill(const std::vector<std::string>& argv,
            const std::string& cwd);
  source_location find_definition(const std::string& usr);

private:
  struct file_info
  {
    CXTranslationUnit                       tu;
    std::map<
      std::string,
      std::shared_ptr<source_location>
    >                                       definitions;
  };

  CXIndex clang_index_;

  /* Maps a file path to a file_info. */
  std::map<std::string, file_info> file_cache_;

  /* Maps an USR to a source_location. */
  std::map<std::string, std::weak_ptr<source_location>> definition_cache_;

  /* Visitors to use during processing. */
  struct fill_visitor_data
  {
    decltype(file_cache_)::value_type&  file_cache_entry;
    decltype(definition_cache_)&        definition_cache;
  };

  static enum CXChildVisitResult fill_visitor_(CXCursor cursor,
                                               CXCursor parent,
                                               CXClientData client_data);
};

}} // namespace clc::server
