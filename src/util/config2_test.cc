#include "config2.h"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace vbenchmark {

TEST(Config2, Basic) {
  {
    const char* argv[] = {"./a.out", "generate", "--dim",      "16",
                          "--otype", "file",     "--normalize"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    Config2 config(argc, const_cast<char**>(argv));
    std::cout << config.Help() << std::endl;
    config.Print();
  }

  {
    const char* argv[] = {"./a.out", "generate"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    Config2 config(argc, const_cast<char**>(argv));
    std::cout << config.Help() << std::endl;
    config.Print();
  }

  {
    const char* argv[] = {"./a.out",
                          "recall",
                          "--dim=1024",
                          "--etype=hnswlib",
                          "--dtype=inner",
                          "--Mvalue=16",
                          "--ef_construction=200",
                          "--sample=10",
                          "--path=./prepare"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    Config2 config(argc, const_cast<char**>(argv));
    std::cout << config.Help() << std::endl;
    config.Print();
  }
}

}  // namespace vbenchmark

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}