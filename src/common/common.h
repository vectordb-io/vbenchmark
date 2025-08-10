#ifndef VBENCHMARK_COMMON_COMMON_H
#define VBENCHMARK_COMMON_COMMON_H

#include <experimental/filesystem>
#include <string>

#include "nlohmann/json.hpp"

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;

namespace vbenchmark {}  // namespace vbenchmark

#endif  // VBENCHMARK_COMMON_COMMON_H