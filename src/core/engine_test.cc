#include "engine.h"

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common.h"
#include "config2.h"
#include "core.h"
#include "util.h"

namespace vbenchmark {

void TestHnswLibEngine(int argc, char** argv) {
  config = std::make_shared<Config2>(argc, argv);
  config->Print();

  VEngineSPtr engine = CreateEngine(config->etype());
  engine->New();

  for (int32_t i = 0; i < config->count(); ++i) {
    std::vector<float> v = GenerateVector(config->dim());
    engine->Put(i, v);
  }

  std::vector<float> v = GenerateVector(config->dim());
  std::priority_queue<std::pair<float, size_t>> pq = engine->Query(v, 10);
  std::vector<std::pair<float, size_t>> cf = CloserFirst(pq);

  Print(pq);
  Print(cf);

  engine->Persist();
}

void TestHnswLibEngineLoad(int argc, char** argv) {
  std::cout << "------------test_load---------\n";
  config = std::make_shared<Config2>(argc, argv);
  config->Print();
  std::vector<float> vq = GenerateVector(config->dim());

  // new
  {
    VEngineSPtr engine = CreateEngine(config->etype());
    engine->New();

    for (int32_t i = 0; i < config->count(); ++i) {
      std::vector<float> v = GenerateVector(config->dim());
      engine->Put(i, v);
    }

    std::priority_queue<std::pair<float, size_t>> pq = engine->Query(vq, 10);
    std::vector<std::pair<float, size_t>> cf = CloserFirst(pq);

    Print(pq);
    Print(cf);

    engine->Persist();
  }

  // load
  {
    VEngineSPtr engine = CreateEngine(config->etype());
    engine->Load();

    std::priority_queue<std::pair<float, size_t>> pq = engine->Query(vq, 10);
    std::vector<std::pair<float, size_t>> cf = CloserFirst(pq);

    Print(pq);
    Print(cf);
  }
}

TEST(HnswLibEngine, NewHnsw) {
  const char* argv[] = {"./vbenchmark",
                        "build",
                        "--btype=generate",
                        "--dim=1024",
                        "--count=100",
                        "--normalize",
                        "--etype=hnswlib",
                        "--stype=hnsw",
                        "--dtype=l2",
                        "--Mvalue=16",
                        "--ef_construction=200",
                        "--count=1000",
                        "--searchk=10",
                        "--path=./test_new_hnsw"};
  int argc = 14;

  TestHnswLibEngine(argc, const_cast<char**>(argv));
  fs::remove_all("./test_new_hnsw");
}

TEST(HnswLibEngine, NewBrute) {
  const char* argv[] = {"./vbenchmark",
                        "build",
                        "--btype=generate",
                        "--dim=1024",
                        "--count=100",
                        "--normalize",
                        "--etype=hnswlib",
                        "--stype=brute",
                        "--dtype=l2",
                        "--Mvalue=16",
                        "--ef_construction=200",
                        "--count=1000",
                        "--searchk=10",
                        "--path=./test_new_brute"};
  int argc = 14;

  TestHnswLibEngine(argc, const_cast<char**>(argv));
  fs::remove_all("./test_new_brute");
}

TEST(HnswLibEngine, Load) {
  const char* argv[] = {"./vbenchmark",
                        "build",
                        "--btype=generate",
                        "--dim=1024",
                        "--count=100",
                        "--normalize",
                        "--etype=hnswlib",
                        "--stype=brute",
                        "--dtype=l2",
                        "--Mvalue=16",
                        "--ef_construction=200",
                        "--count=1000",
                        "--searchk=10",
                        "--path=./test_load"};
  int argc = 14;

  TestHnswLibEngineLoad(argc, const_cast<char**>(argv));
  fs::remove_all("./test_load");
}

}  // namespace vbenchmark

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}