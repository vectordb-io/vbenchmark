#ifndef VBENCHMARK_UTIL_CONFIG2_H
#define VBENCHMARK_UTIL_CONFIG2_H

#include <cxxopts.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace vbenchmark {

extern const std::string kCmdHelp;
extern const std::string kCmdWrite;
extern const std::string kCmdRead;
extern const std::string kCmdBuild;
extern const std::string kCmdQuery;
extern const std::string kCmdPrepare;
extern const std::string kCmdRecall;
extern const std::string kCmdPrepareAndRecall;

extern const std::string kOTypeStdout;
extern const std::string kOTypeFile;
extern const std::string kOTypeLeveldb;
extern const std::string kOTypeRocksdb;

extern const std::string kITypeLeveldb;
extern const std::string kITypeRocksdb;

extern const std::string kBTypeLeveldb;
extern const std::string kBTypeRocksdb;
extern const std::string kBTypeFile;
extern const std::string kBTypeGenerate;

extern const std::string kETypeHnswlib;
extern const std::string kETypeFaiss;

extern const std::string kSTypeHnsw;
extern const std::string kSTypeBrute;

extern const std::string kDTypeL2;
extern const std::string kDTypeInner;

class Config2 {
 public:
  Config2(int argc, char** argv);
  ~Config2();

  Config2(const Config2&) = delete;
  Config2& operator=(const Config2&) = delete;

  void Print();
  std::string ToString() const;
  std::string Help() const;

  std::string cmd() const { return cmd_; }
  bool norm() const { return norm_; }
  bool length() const { return length_; }
  std::string path() const { return path_; }
  std::string otype() const { return otype_; }
  std::string itype() const { return itype_; }
  std::string btype() const { return btype_; }
  std::string etype() const { return etype_; }
  std::string dtype() const { return dtype_; }
  std::string stype() const { return stype_; }
  int32_t M() const { return M_; }
  int32_t ef_construction() const { return ef_construction_; }
  int32_t dim() const { return dim_; }
  int32_t count() const { return count_; }
  int32_t limit() const { return limit_; }
  int32_t searchk() const { return searchk_; }
  int32_t sample() const { return sample_; }

 private:
  void Init();

 private:
  int argc_;
  char** argv_;
  cxxopts::Options options_;
  cxxopts::ParseResult result_;

  std::string cmd_;
  int argc2_;
  std::vector<char*> argvs_;
  char** argv2_;

  bool version_;
  bool norm_;
  bool length_;
  std::string path_;
  std::string otype_;
  std::string itype_;
  std::string btype_;
  std::string etype_;
  std::string dtype_;
  std::string stype_;
  int32_t M_;
  int32_t ef_construction_;
  int32_t dim_;
  int32_t count_;
  int32_t limit_;
  int32_t searchk_;
  int32_t sample_;
};

using Config2SPtr = std::shared_ptr<Config2>;

extern Config2SPtr config;

}  // namespace vbenchmark

#endif  // VBENCHMARK_UTIL_CONFIG2_H