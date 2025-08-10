#include "config2.h"

#include <cassert>
#include <iostream>

#include "util.h"

namespace vbenchmark {

const std::string kCmdHelp = "help";
const std::string kCmdWrite = "write";
const std::string kCmdRead = "read";
const std::string kCmdBuild = "build";
const std::string kCmdQuery = "query";
const std::string kCmdPrepare = "prepare";
const std::string kCmdRecall = "recall";
const std::string kCmdPrepareAndRecall = "pr";

const std::string kOTypeStdout = "stdout";
const std::string kOTypeFile = "file";
const std::string kOTypeLeveldb = "leveldb";
const std::string kOTypeRocksdb = "rocksdb";

const std::string kITypeLeveldb = "leveldb";
const std::string kITypeRocksdb = "rocksdb";

const std::string kBTypeLeveldb = "leveldb";
const std::string kBTypeRocksdb = "rocksdb";
const std::string kBTypeFile = "file";
const std::string kBTypeGenerate = "generate";

const std::string kETypeHnswlib = "hnswlib";
const std::string kETypeFaiss = "faiss";

const std::string kSTypeHnsw = "hnsw";
const std::string kSTypeBrute = "brute";

const std::string kDTypeL2 = "l2";
const std::string kDTypeInner = "inner";

Config2::Config2(int argc, char** argv)
    : argc_(argc), argv_(argv), options_("name", "description") {
  Init();
}

Config2::~Config2() {}

void Config2::Init() {
  assert(argc_ >= 2);
  cmd_ = argv_[1];
  ToLower(cmd_);
  argc2_ = argc_ - 1;

  // 正确初始化argv2_
  argvs_.clear();
  for (int i = 0; i < argc_; ++i) {
    if (i != 1) {
      argvs_.push_back(argv_[i]);
    }
  }
  argv2_ = argvs_.data();

  options_.add_options()("version", "print version",
                         cxxopts::value<bool>()->default_value("false"));
  options_.add_options()("normalize", "normalize vectors",
                         cxxopts::value<bool>()->default_value("false"));
  options_.add_options()("length", "output vector length",
                         cxxopts::value<bool>()->default_value("false"));
  options_.add_options()("path", "file/dir",
                         cxxopts::value<std::string>()->default_value(""));
  options_.add_options()("otype", "stdout/file/leveldb/rocksdb",
                         cxxopts::value<std::string>()->default_value(""));
  options_.add_options()("itype", "leveldb/rocksdb",
                         cxxopts::value<std::string>()->default_value(""));
  options_.add_options()("btype", "leveldb/rocksdb/file/generate",
                         cxxopts::value<std::string>()->default_value(""));
  options_.add_options()("etype", "hnswlib/faiss",
                         cxxopts::value<std::string>()->default_value(""));
  options_.add_options()("dtype", "l2/inner",
                         cxxopts::value<std::string>()->default_value(""));
  options_.add_options()("stype", "hnsw/brute",
                         cxxopts::value<std::string>()->default_value(""));
  options_.add_options()("Mvalue", "M value",
                         cxxopts::value<int32_t>()->default_value("0"));
  options_.add_options()("ef_construction", "ef_construction",
                         cxxopts::value<int32_t>()->default_value("0"));
  options_.add_options()("dim", "dimension",
                         cxxopts::value<int32_t>()->default_value("0"));
  options_.add_options()("count", "vector count",
                         cxxopts::value<int32_t>()->default_value("0"));
  options_.add_options()("limit", "read limit",
                         cxxopts::value<int32_t>()->default_value("999999999"));
  options_.add_options()("searchk", "search k nearest neighbors",
                         cxxopts::value<int32_t>()->default_value("0"));
  options_.add_options()("sample", "sample count",
                         cxxopts::value<int32_t>()->default_value("0"));

  result_ = options_.parse(argc2_, argv2_);

  version_ = result_["version"].as<bool>();
  norm_ = result_["normalize"].as<bool>();
  length_ = result_["length"].as<bool>();
  path_ = result_["path"].as<std::string>();
  otype_ = result_["otype"].as<std::string>();
  itype_ = result_["itype"].as<std::string>();
  btype_ = result_["btype"].as<std::string>();
  etype_ = result_["etype"].as<std::string>();
  dtype_ = result_["dtype"].as<std::string>();
  stype_ = result_["stype"].as<std::string>();
  M_ = result_["Mvalue"].as<int32_t>();
  ef_construction_ = result_["ef_construction"].as<int32_t>();
  dim_ = result_["dim"].as<int32_t>();
  count_ = result_["count"].as<int32_t>();
  limit_ = result_["limit"].as<int32_t>();
  searchk_ = result_["searchk"].as<int32_t>();
  sample_ = result_["sample"].as<int32_t>();
}

std::string Config2::ToString() const {
  std::string s;
  s.append("\n--------------------------------\n");
  s.append("config:\n");
  s.append("cmd: " + cmd_ + "\n");
  s.append("version: " + std::to_string(version_) + "\n");
  s.append("normalize: " + std::to_string(norm_) + "\n");
  s.append("length: " + std::to_string(length_) + "\n");
  s.append("path: " + path_ + "\n");
  s.append("otype: " + otype_ + "\n");
  s.append("itype: " + itype_ + "\n");
  s.append("btype: " + btype_ + "\n");
  s.append("etype: " + etype_ + "\n");
  s.append("dtype: " + dtype_ + "\n");
  s.append("stype: " + stype_ + "\n");
  s.append("Mvalue: " + std::to_string(M_) + "\n");
  s.append("ef_construction: " + std::to_string(ef_construction_) + "\n");
  s.append("dim: " + std::to_string(dim_) + "\n");
  s.append("count: " + std::to_string(count_) + "\n");
  s.append("limit: " + std::to_string(limit_) + "\n");
  s.append("searchk: " + std::to_string(searchk_) + "\n");
  s.append("sample: " + std::to_string(sample_) + "\n");
  s.append("--------------------------------\n");

  return s;
}

void Config2::Print() { std::cout << ToString() << std::endl; }

std::string Config2::Help() const { return options_.help(); }

Config2SPtr config = nullptr;

}  // namespace vbenchmark