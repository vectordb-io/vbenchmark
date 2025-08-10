#include "config.h"

#include <iostream>

namespace vbenchmark {

Config::Config(int argc, char** argv)
    : argc_(argc), argv_(argv), options_("name", "description") {
  Init();
}

Config::~Config() {}

void Config::Init() {
  options_.add_options()("h,help", "print help");
  options_.add_options()("v,version", "print version");
  options_.add_options()(
      "output", "vectors output",
      cxxopts::value<std::string>()->default_value("output_vectors"));
  options_.add_options()("otype", "file/leveldb/rocksdb",
                         cxxopts::value<std::string>()->default_value("file"));
  options_.add_options()("dim", "dimension",
                         cxxopts::value<int>()->default_value("128"));

  result_ = options_.parse(argc_, const_cast<char**>(argv_));

  help_ = result_["help"].as<bool>();
  version_ = result_["version"].as<bool>();
  output_ = result_["output"].as<std::string>();
  otype_ = result_["otype"].as<std::string>();
  dim_ = result_["dim"].as<int32_t>();
}

void Config::Print() {
  std::cout << "config: " << std::endl;
  std::cout << "help: " << help_ << std::endl;
  std::cout << "version: " << version_ << std::endl;
  std::cout << "output: " << output_ << std::endl;
  std::cout << "otype: " << otype_ << std::endl;
  std::cout << "dim: " << dim_ << std::endl;
}

std::string Config::Help() const { return options_.help(); }

}  // namespace vbenchmark