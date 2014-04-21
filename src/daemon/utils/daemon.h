#pragma once

#include <unistd.h>

namespace clc { namespace utils {

bool daemonize();
bool read_pidfile(const char* pid_path, pid_t* pid);
bool write_pidfile(const char* pid_path);

}} // namespace clc::utils
