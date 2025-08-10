#ifndef VBENCHMARK_UTIL_CONFIG_H
#define VBENCHMARK_UTIL_CONFIG_H

#include <cxxopts.hpp>
#include <string>
#include <unordered_map>

namespace vbenchmark {

class Config {
 public:
  Config(int argc, char** argv);
  ~Config();

  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

  void Init();
  void Print();
  std::string Help() const;

 private:
  int argc_;
  char** argv_;
  cxxopts::Options options_;
  cxxopts::ParseResult result_;

  bool help_;
  bool version_;
  std::string output_;
  std::string otype_;
  int32_t dim_;
};

}  // namespace vbenchmark

#endif  // VBENCHMARK_UTIL_CONFIG_H