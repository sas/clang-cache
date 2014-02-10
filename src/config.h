#pragma once

#include <sstream>
#include <string>
#include <unistd.h>

namespace clc { namespace config {

std::string run_path()
{
  std::ostringstream os;

  os << "/run/user/" << getuid() << "/clang-cache";

  return os.str();
}

std::string sock_path()
{
  return run_path() + "/sock";
}

std::string pid_path()
{
  return run_path() + "/pid";
}


}} // namespace clc::config
