#include "core.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "config2.h"
#include "engine.h"
#include "hnswlib/hnswlib.h"
#include "leveldb/db.h"
#include "logger.h"
#include "rocksdb/db.h"
#include "timestamp.h"
#include "util.h"
#include "vdb.pb.h"

namespace vbenchmark {

const int32_t kPrintLogCount = 1000;

int32_t ReadLeveldb() {
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = false;
  options.error_if_exists = false;
  leveldb::Status status = leveldb::DB::Open(options, config->path(), &db);
  if (!status.ok()) {
    std::cerr << "error: Cannot open leveldb " << config->path() << " "
              << status.ToString() << std::endl;
    return -1;
  }

  int32_t read_count = 0;
  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
  for (it->SeekToLast(); it->Valid(); it->Prev()) {
    Key key;
    Value value;
    key.ParseFromString(it->key().ToString());
    value.ParseFromString(it->value().ToString());

    std::string line;
    char buf[128];
    snprintf(buf, sizeof(buf), "%ld; ", key.id());
    line.append(buf);

    for (int32_t j = 0; j < value.data_size() - 1; j++) {
      snprintf(buf, sizeof(buf), "%f ", value.data(j));
      line.append(buf);
    }
    snprintf(buf, sizeof(buf), "%f", value.data(value.data_size() - 1));
    line.append(buf);

    snprintf(buf, sizeof(buf), "; %f", value.length());
    line.append(buf);

    std::cout << line << std::endl;

    read_count++;
    if (read_count >= config->limit()) {
      break;
    }
  }

  delete it;
  delete db;

  return 0;
}

int32_t Query(VEngineSPtr engine) {
  int64_t total_ms = 0;
  int32_t i = 0;
  for (i = 0; i < config->count(); i++) {
    std::vector<float> v = GenerateVector(config->dim());
    int64_t start_ms = TimeStamp().MilliSeconds();
    std::priority_queue<std::pair<float, size_t>> pq =
        engine->Query(v, config->searchk());
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    total_ms += diff_ms;

    if (i % kPrintLogCount == 0) {
      char buf[128];
      std::string rst;
      size_t sz = pq.size();
      for (size_t j = 0; j < pq.size(); j++) {
        snprintf(buf, sizeof(buf), "<%f, %ld> ", pq.top().first,
                 pq.top().second);
        rst.append(buf);
        pq.pop();
      }

      logger->info("query {} records, size {}, rst {}", i, sz, rst);
      logger->flush();
    }
  }

  logger->info("query {} records", i);
  logger->flush();

  logger->info("cost {} ms, avg {} ms", total_ms, total_ms / i);
  logger->flush();

  return 0;
}

int32_t Prepare() {
  std::string path;
  if (config->path().empty()) {
    path = Path("./prepare");
  } else {
    path = config->path();
  }

  VEngineSPtr engine_brute =
      CreateEngine(config->etype(), kSTypeBrute, path + "/brute");
  engine_brute->New();

  VEngineSPtr engine_hnsw =
      CreateEngine(config->etype(), kSTypeHnsw, path + "/hnsw");
  engine_hnsw->New();

  int64_t total_brute_ms = 0;
  int64_t total_hnsw_ms = 0;
  int32_t i = 0;
  for (i = 0; i < config->count(); ++i) {
    std::vector<float> v = GenerateVector(config->dim());

    {
      int64_t start_ms = TimeStamp().MilliSeconds();
      engine_brute->Put(i, v);
      int64_t end_ms = TimeStamp().MilliSeconds();
      int64_t diff_ms = end_ms - start_ms;
      total_brute_ms += diff_ms;
    }

    {
      int64_t start_ms = TimeStamp().MilliSeconds();
      engine_hnsw->Put(i, v);
      int64_t end_ms = TimeStamp().MilliSeconds();
      int64_t diff_ms = end_ms - start_ms;
      total_hnsw_ms += diff_ms;
    }

    if (i % kPrintLogCount == 0) {
      logger->info("prepare {} records", i);
      logger->flush();
    }
  }
  logger->info("prepare {} records", i);
  logger->flush();

  logger->info("brute cost {} ms, avg {} ms", total_brute_ms,
               total_brute_ms / i);
  logger->flush();

  logger->info("hnsw cost {} ms, avg {} ms", total_hnsw_ms, total_hnsw_ms / i);
  logger->flush();

  {
    int64_t start_ms = TimeStamp().MilliSeconds();
    engine_brute->Persist();
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    logger->info("persist brute cost {} ms", diff_ms);
    logger->flush();
  }

  {
    int64_t start_ms = TimeStamp().MilliSeconds();
    engine_hnsw->Persist();
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    logger->info("persist hnsw cost {} ms", diff_ms);
    logger->flush();
  }

  return 0;
}

int32_t PrepareAndRecall() {
  if (config->path().empty()) {
    logger->info("prepare and recall error, path is empty");
    logger->flush();
    return -1;
  }

  int32_t r = Prepare();
  assert(r == 0);

  r = Recall();
  assert(r == 0);

  return 0;
}

int32_t Recall() {
  VEngineSPtr engine_brute =
      CreateEngine(config->etype(), kSTypeBrute, config->path() + "/brute");
  engine_brute->Load();

  VEngineSPtr engine_hnsw =
      CreateEngine(config->etype(), kSTypeHnsw, config->path() + "/hnsw");
  engine_hnsw->Load();

  std::vector<float> rates;
  for (int32_t i = 0; i < config->sample(); i++) {
    std::vector<float> v = GenerateVector(config->dim());

    std::priority_queue<std::pair<float, size_t>> pq_brute =
        engine_brute->Query(v, config->searchk());
    std::priority_queue<std::pair<float, size_t>> pq_hnsw =
        engine_hnsw->Query(v, config->searchk());

    std::vector<std::pair<float, size_t>> cf_brute = CloserFirst(pq_brute);
    std::vector<std::pair<float, size_t>> cf_hnsw = CloserFirst(pq_hnsw);

    logger->info("simple {}:", i);
    logger->info("hnsw rst: {}", ToString(cf_hnsw));
    logger->info("brute rst: {}", ToString(cf_brute));
    logger->flush();

    float recall_rate = RecallRate(cf_hnsw, cf_brute, config->searchk());
    rates.push_back(recall_rate);
  }

  for (size_t i = 0; i < rates.size(); i++) {
    logger->info("recall rate {}", rates[i]);
  }

  logger->info("recall rate avg {}",
               std::accumulate(rates.begin(), rates.end(), 0.0) / rates.size());
  logger->flush();

  return 0;
}

int32_t ReadRocksdb() {
  rocksdb::DB* db;
  rocksdb::Options options;
  options.create_if_missing = false;
  options.error_if_exists = false;
  rocksdb::Status status = rocksdb::DB::Open(options, config->path(), &db);
  if (!status.ok()) {
    std::cerr << "error: Cannot open rocksdb " << config->path() << " "
              << status.ToString() << std::endl;
    return -1;
  }

  int32_t read_count = 0;
  rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
  for (it->SeekToLast(); it->Valid(); it->Prev()) {
    Key key;
    Value value;
    key.ParseFromString(it->key().ToString());
    value.ParseFromString(it->value().ToString());

    std::string line;
    char buf[128];
    snprintf(buf, sizeof(buf), "%ld; ", key.id());
    line.append(buf);

    for (int32_t j = 0; j < value.data_size() - 1; j++) {
      snprintf(buf, sizeof(buf), "%f ", value.data(j));
      line.append(buf);
    }
    snprintf(buf, sizeof(buf), "%f", value.data(value.data_size() - 1));
    line.append(buf);

    snprintf(buf, sizeof(buf), "; %f", value.length());
    line.append(buf);

    std::cout << line << std::endl;

    read_count++;
    if (read_count >= config->limit()) {
      break;
    }
  }

  delete it;
  delete db;

  return 0;
}

std::string Path(std::string name) {
  std::string path = name;
  path.append(".");
  path.append("dim_");
  path.append(std::to_string(config->dim()));
  path.append(".");
  path.append("cnt_");
  path.append(std::to_string(config->count()));
  path.append(".");
  path.append("ts_");
  path.append(std::to_string(TimeStamp().MilliSeconds()));
  return path;
}

int32_t WriteLeveldb() {
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  options.error_if_exists = true;
  std::string path = Path("vldb");
  leveldb::Status status = leveldb::DB::Open(options, path, &db);
  if (!status.ok()) {
    std::cerr << "error: Cannot open leveldb " << path << " "
              << status.ToString() << std::endl;
    return -1;
  }

  int64_t total_ms = 0;
  int32_t i = 0;
  for (i = 0; i < config->count(); i++) {
    Key key;
    Value value;

    key.set_id(i);

    std::vector<float> v = GenerateVector(config->dim(), config->norm());
    float length = Norm(v);
    for (size_t j = 0; j < v.size(); j++) {
      value.add_data(v[j]);
    }
    value.set_length(length);

    std::string key_str;
    key.SerializeToString(&key_str);

    std::string value_str;
    value.SerializeToString(&value_str);

    int64_t start_ms = TimeStamp().MilliSeconds();
    status = db->Put(leveldb::WriteOptions(), key_str, value_str);
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    total_ms += diff_ms;

    if (!status.ok()) {
      std::cerr << "Error: Cannot write leveldb " << path << std::endl;
    }

    if (i % kPrintLogCount == 0) {
      logger->info("write leveldb {} records", i);
      logger->flush();
    }
  }
  logger->info("write leveldb {} records", i);
  logger->flush();

  logger->info("cost {} ms, avg {} ms", total_ms, total_ms / i);
  logger->flush();

  delete db;
  return 0;
}

int32_t WriteRocksdb() {
  rocksdb::DB* db;
  rocksdb::Options options;
  options.create_if_missing = true;
  options.error_if_exists = true;
  std::string path = Path("vrdb");
  rocksdb::Status status = rocksdb::DB::Open(options, path, &db);
  if (!status.ok()) {
    std::cerr << "error: Cannot open rocksdb " << path << " "
              << status.ToString() << std::endl;
    return -1;
  }

  int64_t total_ms = 0;
  int32_t i = 0;
  for (i = 0; i < config->count(); i++) {
    Key key;
    Value value;

    key.set_id(i);

    std::vector<float> v = GenerateVector(config->dim(), config->norm());
    float length = Norm(v);
    for (size_t j = 0; j < v.size(); j++) {
      value.add_data(v[j]);
    }
    value.set_length(length);

    std::string key_str;
    key.SerializeToString(&key_str);

    std::string value_str;
    value.SerializeToString(&value_str);

    int64_t start_ms = TimeStamp().MilliSeconds();
    status = db->Put(rocksdb::WriteOptions(), key_str, value_str);
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    total_ms += diff_ms;

    if (!status.ok()) {
      std::cerr << "Error: Cannot write rocksdb " << path << std::endl;
    }

    if (i % kPrintLogCount == 0) {
      logger->info("write rocksdb {} records", i + 1);
      logger->flush();
    }
  }
  logger->info("write rocksdb {} records", i);
  logger->flush();

  logger->info("cost {} ms, avg {} ms", total_ms, total_ms / i);
  logger->flush();

  delete db;
  return 0;
}

int32_t WriteFile() {
  std::string path = Path("vfile");
  std::ofstream outfile(path);
  if (!outfile.is_open()) {
    std::cerr << "error: Cannot open file " << path << std::endl;
    return -1;
  }

  std::string line;
  char buf[128];

  int64_t total_ms = 0;
  int32_t i = 0;
  for (i = 0; i < config->count(); i++) {
    line.clear();
    snprintf(buf, sizeof(buf), "%d; ", i);
    line.append(buf);

    std::vector<float> v = GenerateVector(config->dim(), config->norm());
    for (size_t j = 0; j < v.size() - 1; j++) {
      snprintf(buf, sizeof(buf), "%f ", v[j]);
      line.append(buf);
    }
    snprintf(buf, sizeof(buf), "%f", v[v.size() - 1]);
    line.append(buf);

    if (config->length()) {
      float length = Norm(v);
      snprintf(buf, sizeof(buf), "; %f", length);
      line.append(buf);
    }

    int64_t start_ms = TimeStamp().MilliSeconds();
    outfile << line << std::endl;
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    total_ms += diff_ms;

    if (i % kPrintLogCount == 0) {
      logger->info("write file {} records", i + 1);
      logger->flush();
    }
  }
  logger->info("write file {} records", i);
  logger->flush();

  logger->info("cost {} ms, avg {} ms", total_ms, total_ms / i);
  logger->flush();

  outfile.close();
  return 0;
}

int32_t WriteStdout() {
  std::string line;
  char buf[128];

  int64_t total_ms = 0;
  int32_t i = 0;
  for (i = 0; i < config->count(); i++) {
    line.clear();
    snprintf(buf, sizeof(buf), "%d; ", i);
    line.append(buf);

    std::vector<float> v = GenerateVector(config->dim(), config->norm());
    for (size_t j = 0; j < v.size() - 1; j++) {
      snprintf(buf, sizeof(buf), "%f ", v[j]);
      line.append(buf);
    }
    snprintf(buf, sizeof(buf), "%f", v[v.size() - 1]);
    line.append(buf);

    if (config->length()) {
      float length = Norm(v);
      snprintf(buf, sizeof(buf), "; %f", length);
      line.append(buf);
    }

    int64_t start_ms = TimeStamp().MilliSeconds();
    std::cout << line << std::endl;
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    total_ms += diff_ms;

    std::cout << line << std::endl;

    if (i % kPrintLogCount == 0) {
      logger->info("write stdout {} records", i + 1);
      logger->flush();
    }
  }

  logger->info("write stdout {} records", i);
  logger->flush();

  logger->info("cost {} ms, avg {} ms", total_ms, total_ms / i);
  logger->flush();

  return 0;
}

void PrintHelp(std::string argv0) {
  std::cout << config->Help() << std::endl;
  std::cout << std::endl;
  std::cout << "example: " << std::endl;
  std::cout << argv0
            << " write --otype=stdout --dim=8 "
               "--length --count=100 --normalize"
            << std::endl;
  std::cout << argv0
            << " write --otype=file --dim=8 "
               "--length --count=100 --normalize"
            << std::endl;
  std::cout << argv0
            << " write --otype=leveldb --dim=8 "
               "--count 100 --normalize"
            << std::endl;
  std::cout << argv0
            << " write --otype=rocksdb --dim=8 "
               "--count 100 --normalize"
            << std::endl;
  std::cout << argv0 << " read --itype=leveldb --path=ldb --limit=10"
            << std::endl;
  std::cout << argv0 << " read --itype=rocksdb --path=rdb --limit=10"
            << std::endl;
  std::cout << argv0
            << " build --btype=generate --dim=1024 --normalize --etype=hnswlib "
               "--dtype=inner --stype=hnsw --Mvalue=16 --ef_construction=200  "
               "--count=1000"
            << std::endl;
  std::cout << argv0
            << " query --path=./hnswlib.dim_1024.cnt_1000.ts_1754740321381 "
               "--dim=1024 --etype=hnswlib --dtype=inner --stype=hnsw "
               "--Mvalue=16 --ef_construction=200 --count=1000"
            << std::endl;
  std::cout << argv0
            << " prepare --dim=1024 --etype=hnswlib --dtype=inner --Mvalue=16 "
               "--ef_construction=200 --count=10000"
            << std::endl;
  std::cout << argv0
            << " recall --dim=1024 --etype=hnswlib --dtype=inner --Mvalue=16 "
               "--ef_construction=200 --sample=10 --count=10000 --searchk=500 "
               "--path=./prepare.dim_1024.cnt_10000.ts_1754799214767/"
            << std::endl;
  std::cout << argv0
            << "pr --dim=1024 --etype=hnswlib --dtype=inner --Mvalue=16 "
               "--ef_construction=200 --count=10000 --searchk=500 --sample=10 "
               "--path=./prepare"
            << std::endl;
  std::cout << std::endl;
}

int32_t BuildFromLeveldb(VEngineSPtr engine) { return 0; }

int32_t BuildFromRocksdb(VEngineSPtr engine) { return 0; }

int32_t BuildFromFile(VEngineSPtr engine) { return 0; }

int32_t BuildFromGenerate(VEngineSPtr engine) {
  int64_t total_ms = 0;
  int32_t i = 0;
  for (i = 0; i < config->count(); ++i) {
    std::vector<float> v = GenerateVector(config->dim());

    int64_t start_ms = TimeStamp().MilliSeconds();
    engine->Put(i, v);
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;
    total_ms += diff_ms;

    if (i % kPrintLogCount == 0) {
      logger->info("build from generate {} records", i);
      logger->flush();
    }
  }
  logger->info("build from generate {} records", i);
  logger->flush();

  logger->info("cost {} ms, avg {} ms", total_ms, total_ms / i);
  logger->flush();

  return 0;
}

int32_t Do(std::string argv0) {
  int32_t r = 0;
  int64_t start_time = TimeStamp().MilliSeconds();
  logger->info("cmd {} start", config->cmd());
  logger->flush();

  if (config->cmd() == kCmdHelp) {
    PrintHelp(argv0);
    return 0;

  } else if (config->cmd() == kCmdWrite) {
    if (config->otype() == kOTypeLeveldb) {
      r = WriteLeveldb();

    } else if (config->otype() == kOTypeRocksdb) {
      r = WriteRocksdb();

    } else if (config->otype() == kOTypeFile) {
      r = WriteFile();

    } else if (config->otype() == kOTypeStdout) {
      r = WriteStdout();

    } else {
      std::cout << "Unknown output type: " << config->otype() << std::endl;
      return -1;
    }

  } else if (config->cmd() == kCmdRead) {
    if (config->itype() == kITypeLeveldb) {
      r = ReadLeveldb();

    } else if (config->itype() == kITypeRocksdb) {
      r = ReadRocksdb();

    } else {
      std::cout << "Unknown output type: " << config->itype() << std::endl;
      return -1;
    }

  } else if (config->cmd() == kCmdBuild) {
    VEngineSPtr engine = CreateEngine(config->etype());
    engine->New();

    if (config->btype() == kBTypeGenerate) {
      r = BuildFromGenerate(engine);

    } else if (config->btype() == kBTypeLeveldb) {
      r = BuildFromLeveldb(engine);

    } else if (config->btype() == kBTypeRocksdb) {
      r = BuildFromRocksdb(engine);

    } else if (config->btype() == kBTypeFile) {
      r = BuildFromFile(engine);

    } else {
      std::cout << "Unknown build type: " << config->btype() << std::endl;
      return -1;
    }

    int64_t start_ms = TimeStamp().MilliSeconds();
    engine->Persist();
    int64_t end_ms = TimeStamp().MilliSeconds();
    int64_t diff_ms = end_ms - start_ms;

    logger->info("persist cost {} ms", diff_ms);
    logger->flush();

  } else if (config->cmd() == kCmdQuery) {
    VEngineSPtr engine = CreateEngine(config->etype());
    engine->Load();

    r = Query(engine);

  } else if (config->cmd() == kCmdPrepare) {
    r = Prepare();

  } else if (config->cmd() == kCmdRecall) {
    r = Recall();

  } else if (config->cmd() == kCmdPrepareAndRecall) {
    r = PrepareAndRecall();

  } else {
    std::cout << "Unknown command: " << config->cmd() << std::endl;
    std::cout << config->Help() << std::endl;
    return -1;
  }

  int64_t end_time = TimeStamp().MilliSeconds();
  logger->info("cmd {} end", config->cmd());
  logger->info("cmd {} cost {} ms", config->cmd(), (end_time - start_time));
  logger->flush();

  return r;
}

}  // namespace vbenchmark