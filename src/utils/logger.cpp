#include "logger.h"

#include <cstdlib>
#include <ctime>
#include <map>
#include <thrift/Thrift.h>

namespace clc { namespace utils {

static const char* level2str(logger::log_level level)
{
  static const std::map<logger::log_level, const char*> m = {
    { logger::INFO,     "INFO" },
    { logger::WARN,     "WARN" },
    { logger::FATAL,    "FATAL" },
    { logger::INVALID,  "INVALID" },
  };

  return m.at(level);
}

logger::logger_proxy::logger_proxy(std::ostream& out, log_level msg_level,
                                   log_level current_level, bool do_strerror)
  : out_(out), do_log_(msg_level >= current_level),
    do_exit_(msg_level == FATAL), do_strerror_(do_strerror)
{
  time_t t;
  char   t_str[20];

  if (!do_log_)
    return;

  time(&t);
  strftime(t_str, sizeof t_str, "%Y-%m-%d %H:%M:%S", gmtime(&t));

  *this << "[" << t_str << "] " << level2str(msg_level) << ": ";
}

logger::logger_proxy::~logger_proxy()
{
  if (do_strerror_)
    *this << ": " << strerror(errno);
  *this << "\n";

  if (do_exit_)
    exit(EXIT_FAILURE);
}

static void thrift_logger(const char* msg)
{
  LOG_INFO() << "Thrift: " << msg;
}

logger::logger()
  : level_(INFO), out_()
{
  out_.open("/dev/stderr");
  apache::thrift::GlobalOutput.setOutputFunction(thrift_logger);
}

logger::~logger()
{
  out_.close();
}

void logger::set_level(log_level level)
{
  level_ = level;
}

void logger::set_out(const std::string& out_file)
{
  out_.close();
  out_.open(out_file.c_str());
}

logger logger_instance;

}} // namespace clc::utils
