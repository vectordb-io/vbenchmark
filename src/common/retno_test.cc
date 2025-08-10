#include "retno.h"

#include <gtest/gtest.h>

namespace vbenchmark {
TEST(RetNoTest, RetNoToString) {
  EXPECT_EQ(RetNoToString(RET_OK), "ok");
  EXPECT_EQ(RetNoToString(RET_ERROR), "error");
}

}  // namespace vbenchmark

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}