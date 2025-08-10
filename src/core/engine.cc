#include "engine.h"

#include "common.h"
#include "config2.h"
#include "core.h"
#include "logger.h"

namespace vbenchmark {

VEngineSPtr CreateEngine(const std::string& etype) {
  return CreateEngine(etype, config->stype());
}

VEngineSPtr CreateEngine(const std::string& etype, const std::string& stype) {
  std::string path;
  if (config->path().empty()) {
    path = Path("./hnswlib");
  } else {
    path = config->path();
  }

  return CreateEngine(etype, stype, path);
}

VEngineSPtr CreateEngine(const std::string& etype, const std::string& stype,
                         const std::string& path) {
  std::shared_ptr<VEngine> engine;

  if (etype == kETypeHnswlib) {
    engine = std::make_shared<HnswLibEngine>(
        path, config->dtype(), stype, config->dim(), config->M(),
        config->ef_construction(), config->count());

  } else if (etype == kETypeFaiss) {
    ;

  } else {
    logger->error("error: invalid etype {}", etype);
    exit(-1);
  }

  return engine;
}

HnswLibEngine::HnswLibEngine(const std::string& path, const std::string& dtype,
                             const std::string& stype, int32_t dim, int32_t M,
                             int32_t ef_construction, int32_t max_num)
    : path_(path),
      dtype_(dtype),
      stype_(stype),
      dim_(dim),
      M_(M),
      ef_construction_(ef_construction),
      max_num_(max_num) {
  index_path_ = path + "/hnswlib.index";
  description_file_ = path + "/description.json";

  if (dtype_ == kDTypeL2) {
    hspace_ = std::make_shared<hnswlib::L2Space>(dim_);

  } else if (dtype_ == kDTypeInner) {
    hspace_ = std::make_shared<hnswlib::InnerProductSpace>(dim_);
  }
}

HnswLibEngine::~HnswLibEngine() {}

void HnswLibEngine::New() {
  if (fs::exists(path_)) {
    logger->error("error: path {} already exists", path_);
    exit(-1);
  }
  fs::create_directories(path_);

  if (stype_ == kSTypeHnsw) {
    hindex_ = std::make_shared<hnswlib::HierarchicalNSW<float>>(
        hspace_.get(), max_num_, M_, ef_construction_);

  } else if (stype_ == kSTypeBrute) {
    hindex_ = std::make_shared<hnswlib::BruteforceSearch<float>>(hspace_.get(),
                                                                 max_num_);

  } else {
    logger->error("error: invalid stype {}", stype_);
    exit(-1);
  }
}

void HnswLibEngine::Load() {
  if (!fs::exists(path_)) {
    logger->error("error: path {} not found", path_);
    exit(-1);
  }

  if (stype_ == kSTypeHnsw) {
    hindex_ = std::make_shared<hnswlib::HierarchicalNSW<float>>(
        hspace_.get(), index_path_, max_num_);

  } else if (stype_ == kSTypeBrute) {
    hindex_ = std::make_shared<hnswlib::BruteforceSearch<float>>(hspace_.get(),
                                                                 index_path_);

  } else {
    logger->error("error: invalid stype {}", stype_);
    exit(-1);
  }

  logger->info("load hnswlib engine from index_path:{} max_num:{} ok",
               index_path_, max_num_);
  logger->flush();
}

int32_t HnswLibEngine::Put(int32_t id, const std::vector<float>& v) {
  hindex_->addPoint(v.data(), id);
  return 0;
}

std::priority_queue<std::pair<float, size_t>> HnswLibEngine::Query(
    const std::vector<float>& v, int32_t k) {
  return hindex_->searchKnn(v.data(), k);
}

void HnswLibEngine::PersistDescription() {
  std::ofstream ofs(description_file_);
  json j;
  j["path"] = path_;
  j["index_path"] = index_path_;
  j["description_file"] = description_file_;
  j["dtype"] = dtype_;
  j["stype"] = stype_;
  j["dim"] = dim_;
  j["M"] = M_;
  j["ef_construction"] = ef_construction_;
  j["max_num"] = max_num_;
  ofs << j.dump(2);
  ofs.close();
}

int32_t HnswLibEngine::Persist() {
  if (hindex_) {
    hindex_->saveIndex(index_path_);
  }

  PersistDescription();

  return 0;
}

}  // namespace vbenchmark