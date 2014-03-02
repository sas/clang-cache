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
  cache();
  void fill(const std::vector<std::string>& argv);

private:
  struct source_location
  {
    std::string   path;
    std::uint32_t line;
    std::uint32_t column;
  };

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
};

}} // namespace clc::server