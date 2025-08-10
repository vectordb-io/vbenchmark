#include "config.h"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace vbenchmark {

TEST(Config, Basic) {
  const char* argv[] = {"./a.out", "--version", "--output", "xxx",
                        "--dim",   "77",        "--otype",  "leveldb"};
  int argc = 8;
  Config config(argc, const_cast<char**>(argv));
  std::cout << config.Help() << std::endl;
  config.Print();
}

}  // namespace vbenchmark

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}