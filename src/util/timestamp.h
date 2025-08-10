#ifndef VBENCHMARK_UTIL_TIMESTAMP_H
#define VBENCHMARK_UTIL_TIMESTAMP_H

#include <chrono>
#include <string>
#include <vector>

namespace vbenchmark {

class TimeStamp {
 public:
  TimeStamp();
  TimeStamp(int64_t milliseconds);
  ~TimeStamp();

  int64_t Seconds();
  int64_t MilliSeconds();
  int64_t MicroSeconds();
  int64_t NanoSeconds();
  std::string ToString() const;
  std::string ToString2() const;

 private:
  std::chrono::time_point<std::chrono::system_clock> start_time_;
};

}  // namespace vbenchmark

#endif  // VBENCHMARK_UTIL_TIMESTAMP_H