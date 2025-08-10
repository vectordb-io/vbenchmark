#ifndef VBENCHMARK_CORE_CORE_H
#define VBENCHMARK_CORE_CORE_H

#include <cstdint>
#include <string>
#include <vector>

#include "engine.h"

namespace vbenchmark {

int32_t Do(std::string argv0);

int32_t WriteLeveldb();
int32_t WriteRocksdb();
int32_t WriteFile();
int32_t WriteStdout();

int32_t BuildFromLeveldb(VEngineSPtr engine);
int32_t BuildFromRocksdb(VEngineSPtr engine);
int32_t BuildFromFile(VEngineSPtr engine);
int32_t BuildFromGenerate(VEngineSPtr engine);

int32_t ReadLeveldb();
int32_t ReadRocksdb();

int32_t Query(VEngineSPtr engine);

int32_t Prepare();
int32_t Recall();
int32_t PrepareAndRecall();

std::string Path(std::string name);
void PrintHelp(std::string argv0);

}  // namespace vbenchmark

#endif  // VBENCHMARK_CORE_CORE_H