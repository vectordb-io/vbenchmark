#ifndef VBENCHMARK_UTIL_LOGGER_H
#define VBENCHMARK_UTIL_LOGGER_H

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace vbenchmark {

extern const std::string kLoggerName;
extern std::shared_ptr<spdlog::logger> logger;

void InitLogger(const std::string &logfile);
void DestroyLogger();
void DisableLog();
void EnableLog();

}  // namespace vbenchmark

#endif  // VBENCHMARK_UTIL_LOGGER_H