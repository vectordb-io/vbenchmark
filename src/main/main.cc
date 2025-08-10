#include <cstdint>
#include <fstream>
#include <iostream>

#include "config2.h"
#include "core.h"
#include "leveldb/db.h"
#include "logger.h"
#include "rocksdb/db.h"
#include "timestamp.h"
#include "util.h"
#include "vdb.pb.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <command> [options]" << std::endl;
    return -1;
  }

  std::string log_file =
      "vbenchmark" + vbenchmark::TimeStamp().ToString2() + ".log";
  vbenchmark::InitLogger(log_file);

  vbenchmark::config = std::make_shared<vbenchmark::Config2>(argc, argv);
  // vbenchmark::config->Print();
  vbenchmark::logger->info(vbenchmark::config->ToString());

  int32_t r = vbenchmark::Do(argv[0]);

  vbenchmark::DestroyLogger();
  return r;
}