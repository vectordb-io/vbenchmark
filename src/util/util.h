#ifndef VBENCHMARK_UTIL_UTIL_H
#define VBENCHMARK_UTIL_UTIL_H

#include <queue>
#include <string>
#include <vector>

namespace vbenchmark {

void ToLower(std::string &str);
void ToUpper(std::string &str);
void Normalize(std::vector<float> &v);
float L2(const std::vector<float> &v1, const std::vector<float> &v2);
float RandomFloat(float min, float max);
float Norm(const std::vector<float> &v);

std::vector<float> GenerateVector(int32_t dim, bool normalized = true);

std::vector<std::pair<float, size_t>> CloserFirst(
    std::priority_queue<std::pair<float, size_t>>);

std::string ToString(std::vector<std::pair<float, size_t>> data);
std::string ToString(std::priority_queue<std::pair<float, size_t>> pq);
std::string ToString(std::vector<float> data);

void Print(std::vector<std::pair<float, size_t>> data);
void Print(std::priority_queue<std::pair<float, size_t>> pq);
void Print(std::vector<float> data);

// cf means closer first
float RecallRate(std::vector<std::pair<float, size_t>> cf_test,
                 std::vector<std::pair<float, size_t>> cf_base, size_t topk);

}  // namespace vbenchmark

#endif  // VBENCHMARK_UTIL_UTIL_H