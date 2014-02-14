#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

namespace clc { namespace utils {

class logger
{
public:
  enum log_level
  {
    INVALID,
    INFO,
    WARN,
    FATAL,
  };

private:
  class logger_proxy
  {
  public:
    logger_proxy(std::ostream& out, log_level msg_level,
                 log_level current_level, bool do_strerror);
    ~logger_proxy();

    template<typename T>
    logger_proxy& operator<<(const T& rhs)
    {
      if (do_log_)
        out_ << rhs;
      return *this;
    }

  private:
    std::ostream& out_;
    bool do_log_;
    bool do_exit_;
    bool do_strerror_;
  };

public:
  logger();
  void set_level(log_level level);
  logger_proxy log(log_level level, bool do_strerror = false)
  {
    return logger_proxy(std::clog, level, level_, do_strerror);
  }

private:
  log_level level_;
};

/*
 * Helper macros and global instance.
 * XXX: The global instance stuff sucks a little bit, but it should be ok for
 * now.
 */
extern logger logger_instance;

#define _BASE_LOG(LEVEL, STRERR) ::clc::utils::logger_instance.log(clc::utils::logger::LEVEL, STRERR)
#define _NOLOG()            _BASE_LOG(INVALID, false)
#define _LOG(LEVEL)         _BASE_LOG(LEVEL, false)
#define _PLOG(LEVEL, COND)  ((COND) ? _BASE_LOG(LEVEL, true) : _NOLOG())

#define SET_LEVEL(LEVEL)    ::clc::utils::logger_instance.set_level(clc::utils::logger::LEVEL)
#define LOG_INFO()          _LOG(INFO)
#define LOG_WARN()          _LOG(WARN)
#define LOG_FATAL()         _LOG(FATAL)
#define PLOG_INFO(COND)     _PLOG(INFO, COND)
#define PLOG_WARN(COND)     _PLOG(WARN, COND)
#define PLOG_FATAL(COND)    _PLOG(FATAL, COND)

}} // namespace clc::utils
