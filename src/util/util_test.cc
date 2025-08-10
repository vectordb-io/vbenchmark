#include "util.h"

#include <gtest/gtest.h>

#include <iostream>
#include <thread>

namespace vbenchmark {

TEST(UtilTest, ToLower) {
  std::string str = "Hello, World!";
  ToLower(str);
  EXPECT_EQ(str, "hello, world!");
}

TEST(UtilTest, ToUpper) {
  std::string str = "hello, world!";
  ToUpper(str);
  EXPECT_EQ(str, "HELLO, WORLD!");
}

// 测试函数 CloserFirst
TEST(UtilTest, CloserFirst) {
  std::priority_queue<std::pair<float, size_t>> pq;
  pq.push(std::make_pair(1.6, 1));
  pq.push(std::make_pair(9.3, 2));
  pq.push(std::make_pair(3.0, 3));
  pq.push(std::make_pair(23.6, 4));
  pq.push(std::make_pair(5.3, 5));
  Print(pq);

  std::vector<std::pair<float, size_t>> v = CloserFirst(pq);
  Print(v);
}

TEST(UtilTest, RecallRate) {
  std::vector<std::pair<float, size_t>> cf_test = {
      {1.6, 1}, {9.3, 4}, {11.0, 9}, {23.6, 11}, {58.3, 5}};
  std::vector<std::pair<float, size_t>> cf_base = {
      {1.6, 1}, {3.3, 20}, {9.3, 4}, {9.7, 8}, {11.0, 9}};

  std::cout << "cf_test: " << std::endl;
  Print(cf_test);

  std::cout << "cf_base: " << std::endl;
  Print(cf_base);

  for (size_t topk = 1; topk <= 5; topk++) {
    float rr = RecallRate(cf_test, cf_base, topk);
    std::cout << "top" << topk << " recall rate: " << rr << std::endl;
  }
}

TEST(UtilTest, PrintPId) {
  pid_t pid = getpid();
  std::cout << "pid: " << pid << std::endl;

  std::thread threads[10];
  for (size_t i = 0; i < 10; i++) {
    threads[i] = std::thread([&]() {
      pid_t pid = getpid();
      pid_t tid = gettid();
      std::cout << "pid: " << pid << " tid: " << tid << std::endl;
    });
  }
  for (size_t i = 0; i < 10; i++) {
    threads[i].join();
  }
}

}  // namespace vbenchmark

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}