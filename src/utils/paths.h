#pragma once

#include <sstream>
#include <string>
#include <unistd.h>

namespace clc { namespace utils {

static inline std::string run_path()
{
  std::ostringstream os;

  os << "/run/user/" << getuid() << "/clang-cache";

  return os.str();
}

static inline std::string sock_path()
{
  return run_path() + "/sock";
}

static inline std::string pid_path()
{
  return run_path() + "/pid";
}

static inline std::string log_path()
{
  return run_path() + "/log";
}

}} // namespace clc::utils
