#include "util.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

namespace vbenchmark {

void ToLower(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void ToUpper(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void Normalize(std::vector<float> &v) {
  float norm = 0.0;
  for (size_t i = 0; i < v.size(); i++) {
    norm += v[i] * v[i];
  }
  norm = std::sqrt(norm);

  if (norm > 0) {
    for (size_t i = 0; i < v.size(); i++) {
      v[i] /= norm;
    }
  }
}

float L2(const std::vector<float> &v1, const std::vector<float> &v2) {
  assert(v1.size() == v2.size());

  float distance = 0.0;
  for (size_t i = 0; i < v1.size(); i++) {
    distance += (v1[i] - v2[i]) * (v1[i] - v2[i]);
  }
  return distance;
}

float RandomFloat(float min, float max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(min, max);
  return dis(gen);
}

float Norm(const std::vector<float> &v) {
  float norm = 0.0;
  for (size_t i = 0; i < v.size(); i++) {
    norm += v[i] * v[i];
  }
  return std::sqrt(norm);
}

std::vector<std::pair<float, size_t>> CloserFirst(
    std::priority_queue<std::pair<float, size_t>> pq) {
  std::vector<std::pair<float, size_t>> result;
  size_t sz = pq.size();
  result.resize(sz);
  while (!pq.empty()) {
    result[--sz] = pq.top();
    pq.pop();
  }
  return result;
}

std::string ToString(std::vector<std::pair<float, size_t>> data) {
  std::string s;
  for (size_t i = 0; i < data.size(); i++) {
    char buf[128];
    snprintf(buf, sizeof(buf), "<%f, %ld> ", data[i].first, data[i].second);
    s.append(buf);
  }
  return s;
}

std::string ToString(std::priority_queue<std::pair<float, size_t>> data) {
  std::string s;
  while (!data.empty()) {
    char buf[128];
    auto top = data.top();
    snprintf(buf, sizeof(buf), "<%f, %ld> ", top.first, top.second);
    data.pop();
    s.append(buf);
  }
  return s;
}

std::string ToString(std::vector<float> data) {
  std::string s = "<";
  for (size_t i = 0; i < data.size(); i++) {
    char buf[128];
    snprintf(buf, sizeof(buf), "%f", data[i]);
    s += buf;
    if (i != data.size() - 1) {
      s += ", ";
    }
  }
  s += ">";
  return s;
}

void Print(std::vector<std::pair<float, size_t>> data) {
  std::cout << "print vector: " << ToString(data) << std::endl;
}

void Print(std::priority_queue<std::pair<float, size_t>> data) {
  std::cout << "print pqueue: " << ToString(data) << std::endl;
}

void Print(std::vector<float> data) {
  std::cout << "print floatV: " << ToString(data) << std::endl;
}

float RecallRate(std::vector<std::pair<float, size_t>> cf_test,
                 std::vector<std::pair<float, size_t>> cf_base, size_t topk) {
  float rr = 0.0;
  int32_t hit = 0;
  for (size_t i = 0; i < topk; i++) {
    size_t id = cf_test[i].second;
    for (size_t j = 0; j < topk; j++) {
      if (cf_base[j].second == id) {
        hit += 1;
      }
    }
  }
  // std::cout << "hit: " << hit << " topk: " << topk << std::endl;
  rr = static_cast<float>(hit) / static_cast<float>(topk);
  return rr;
}

std::vector<float> GenerateVector(int32_t dim, bool normalized) {
  std::vector<float> v(dim);
  for (int32_t i = 0; i < dim; i++) {
    v[i] = RandomFloat(-10000000, 10000000);
  }

  if (normalized) {
    Normalize(v);
  }
  return v;
}

}  // namespace vbenchmark