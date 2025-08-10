#ifndef VBENCHMARK_ENGINE_H
#define VBENCHMARK_ENGINE_H

#include <string>
#include <vector>

#include "hnswlib/hnswlib.h"

namespace vbenchmark {

class VEngine {
 public:
  VEngine() {}
  virtual ~VEngine() {}

  VEngine(const VEngine&) = delete;
  VEngine& operator=(const VEngine&) = delete;

  virtual void New() = 0;
  virtual void Load() = 0;

  virtual int32_t Put(int32_t id, const std::vector<float>& v) = 0;
  virtual std::priority_queue<std::pair<float, size_t>> Query(
      const std::vector<float>& v, int32_t k) = 0;
  virtual int32_t Persist() = 0;
};

using VEngineSPtr = std::shared_ptr<VEngine>;

VEngineSPtr CreateEngine(const std::string& etype);
VEngineSPtr CreateEngine(const std::string& etype, const std::string& stype);
VEngineSPtr CreateEngine(const std::string& etype, const std::string& stype,
                         const std::string& path);

class HnswLibEngine : public VEngine {
 public:
  HnswLibEngine(const std::string& path, const std::string& dtype,
                const std::string& stype, int32_t dim, int32_t M,
                int32_t ef_construction, int32_t max_num);
  ~HnswLibEngine();

  HnswLibEngine(const HnswLibEngine&) = delete;
  HnswLibEngine& operator=(const HnswLibEngine&) = delete;

  void New() override;
  void Load() override;

  int32_t Put(int32_t id, const std::vector<float>& v) override;
  std::priority_queue<std::pair<float, size_t>> Query(
      const std::vector<float>& v, int32_t k) override;
  int32_t Persist() override;

 private:
  void PersistDescription();

 private:
  std::string path_;
  std::string index_path_;
  std::string description_file_;

  std::string dtype_;
  std::string stype_;
  int32_t dim_;

  int32_t M_;
  int32_t ef_construction_;
  int32_t max_num_;

  std::shared_ptr<hnswlib::AlgorithmInterface<float>> hindex_;
  std::shared_ptr<hnswlib::SpaceInterface<float>> hspace_;
};

}  // namespace vbenchmark

#endif  // VBENCHMARK_ENGINE_H