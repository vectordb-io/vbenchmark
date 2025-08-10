#include <gtest/gtest.h>

#include <cxxopts.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

TEST(CXXOptsTest, Basic) {
  cxxopts::Options options("name", "description");
  options.add_options()("h,help", "print help")(
      "v,version", "print version",
      cxxopts::value<bool>()->default_value("true"))(
      "f,file", "file to process",
      cxxopts::value<std::string>()->default_value("test.txt"))(
      "l,level", "log level", cxxopts::value<int>()->default_value("9"));

  // 测试空格分隔格式
  {
    const char* argv[] = {"./a.out", "-f", "test.txt"};
    int argc = 3;
    cxxopts::ParseResult result = options.parse(argc, const_cast<char**>(argv));

    std::cout << "file count: " << result.count("file") << std::endl;
    std::cout << "file value: " << result["file"].as<std::string>()
              << std::endl;

    std::cout << "help: " << result["help"].as<bool>() << std::endl;
    std::cout << "level: " << result["level"].as<int>() << std::endl;
    std::cout << "version: " << result["version"].as<bool>() << std::endl;

    EXPECT_TRUE(result.count("file"));
    EXPECT_EQ(result["file"].as<std::string>(), "test.txt");

    EXPECT_EQ(result["help"].as<bool>(), false);
    EXPECT_EQ(result["version"].as<bool>(), true);
    EXPECT_EQ(result["level"].as<int>(), 9);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}