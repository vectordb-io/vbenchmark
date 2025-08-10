#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace vbenchmark {}  // namespace vbenchmark

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}