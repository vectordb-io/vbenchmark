#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common.h"
#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/slice.h"
#include "leveldb/write_batch.h"

// 测试LevelDB基本操作
class LevelDBTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 创建临时目录用于测试
    db_path_ = "/tmp/leveldb_test";

    // 删除可能存在的旧数据库
    leveldb::Options options;
    leveldb::DestroyDB(db_path_, options);

    // 设置数据库选项
    options.create_if_missing = true;
    options.error_if_exists = false;

    // 打开数据库
    leveldb::Status status = leveldb::DB::Open(options, db_path_, &db_);
    ASSERT_TRUE(status.ok()) << "无法打开数据库: " << status.ToString();
  }

  void TearDown() override {
    // 关闭数据库
    if (db_ != nullptr) {
      delete db_;
      db_ = nullptr;
    }

    // 清理测试数据库
    leveldb::Options options;
    leveldb::DestroyDB(db_path_, options);
  }

  std::string db_path_;
  leveldb::DB* db_ = nullptr;
};

// 测试基本的键值存储和读取
TEST_F(LevelDBTest, BasicPutGet) {
  leveldb::Status status;
  std::string value;

  // 写入键值对
  status = db_->Put(leveldb::WriteOptions(), "key1", "value1");
  ASSERT_TRUE(status.ok()) << "写入失败: " << status.ToString();

  // 读取键值对
  status = db_->Get(leveldb::ReadOptions(), "key1", &value);
  ASSERT_TRUE(status.ok()) << "读取失败: " << status.ToString();
  EXPECT_EQ("value1", value);

  // 读取不存在的键
  status = db_->Get(leveldb::ReadOptions(), "不存在的键", &value);
  EXPECT_TRUE(status.IsNotFound());
}

// 测试批量写入
TEST_F(LevelDBTest, BatchWrite) {
  leveldb::Status status;
  std::string value;

  // 创建批量写入对象
  leveldb::WriteBatch batch;
  batch.Put("batch_key1", "batch_value1");
  batch.Put("batch_key2", "batch_value2");
  batch.Put("batch_key3", "batch_value3");

  // 执行批量写入
  status = db_->Write(leveldb::WriteOptions(), &batch);
  ASSERT_TRUE(status.ok()) << "批量写入失败: " << status.ToString();

  // 验证批量写入的数据
  status = db_->Get(leveldb::ReadOptions(), "batch_key1", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("batch_value1", value);

  status = db_->Get(leveldb::ReadOptions(), "batch_key2", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("batch_value2", value);

  status = db_->Get(leveldb::ReadOptions(), "batch_key3", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("batch_value3", value);
}

// 测试删除操作
TEST_F(LevelDBTest, DeleteOperation) {
  leveldb::Status status;
  std::string value;

  // 写入键值对
  status = db_->Put(leveldb::WriteOptions(), "delete_key", "delete_value");
  ASSERT_TRUE(status.ok());

  // 验证写入成功
  status = db_->Get(leveldb::ReadOptions(), "delete_key", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("delete_value", value);

  // 删除键值对
  status = db_->Delete(leveldb::WriteOptions(), "delete_key");
  ASSERT_TRUE(status.ok()) << "删除失败: " << status.ToString();

  // 验证删除成功
  status = db_->Get(leveldb::ReadOptions(), "delete_key", &value);
  EXPECT_TRUE(status.IsNotFound());
}

// 测试迭代器
TEST_F(LevelDBTest, Iterator) {
  leveldb::Status status;

  // 写入一系列键值对
  std::vector<std::pair<std::string, std::string>> test_data = {
      {"iter_key1", "iter_value1"},
      {"iter_key2", "iter_value2"},
      {"iter_key3", "iter_value3"},
      {"iter_key4", "iter_value4"},
      {"iter_key5", "iter_value5"}};

  for (const auto& pair : test_data) {
    status = db_->Put(leveldb::WriteOptions(), pair.first, pair.second);
    ASSERT_TRUE(status.ok());
  }

  // 创建迭代器
  leveldb::Iterator* it = db_->NewIterator(leveldb::ReadOptions());

  // 测试迭代所有键值对
  size_t count = 0;
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    // 只检查我们刚刚插入的键值对
    if (it->key().ToString().find("iter_key") == 0) {
      std::string key = it->key().ToString();
      std::string value = it->value().ToString();

      // 找到对应的测试数据
      bool found = false;
      for (const auto& pair : test_data) {
        if (pair.first == key) {
          EXPECT_EQ(pair.second, value);
          found = true;
          count++;
          break;
        }
      }
      EXPECT_TRUE(found) << "找到了意外的键: " << key;
    }
  }

  EXPECT_EQ(test_data.size(), count) << "迭代器没有遍历所有测试数据";

  // 检查迭代器状态
  ASSERT_TRUE(it->status().ok()) << "迭代器错误: " << it->status().ToString();

  // 释放迭代器
  delete it;
}

// 测试前缀扫描
TEST_F(LevelDBTest, PrefixScan) {
  leveldb::Status status;

  // 写入一系列带前缀的键值对
  std::vector<std::pair<std::string, std::string>> prefix_data = {
      {"prefix1_key1", "value1"},
      {"prefix1_key2", "value2"},
      {"prefix2_key1", "value3"},
      {"prefix2_key2", "value4"},
      {"other_key", "value5"}};

  for (const auto& pair : prefix_data) {
    status = db_->Put(leveldb::WriteOptions(), pair.first, pair.second);
    ASSERT_TRUE(status.ok());
  }

  // 创建迭代器
  leveldb::Iterator* it = db_->NewIterator(leveldb::ReadOptions());

  // 使用迭代器进行前缀扫描 - prefix1
  std::string prefix = "prefix1";
  size_t count = 0;

  for (it->Seek(prefix); it->Valid() && it->key().ToString().compare(
                                            0, prefix.size(), prefix) == 0;
       it->Next()) {
    count++;
  }

  EXPECT_EQ(static_cast<size_t>(2), count)
      << "前缀扫描 'prefix1' 应该返回2个结果";

  // 使用迭代器进行前缀扫描 - prefix2
  prefix = "prefix2";
  count = 0;

  for (it->Seek(prefix); it->Valid() && it->key().ToString().compare(
                                            0, prefix.size(), prefix) == 0;
       it->Next()) {
    count++;
  }

  EXPECT_EQ(static_cast<size_t>(2), count)
      << "前缀扫描 'prefix2' 应该返回2个结果";

  // 释放迭代器
  delete it;
}

// 测试快照功能
TEST_F(LevelDBTest, Snapshot) {
  leveldb::Status status;
  std::string value;

  // 写入初始键值对
  status = db_->Put(leveldb::WriteOptions(), "snapshot_key", "original_value");
  ASSERT_TRUE(status.ok());

  // 创建快照
  const leveldb::Snapshot* snapshot = db_->GetSnapshot();

  // 修改键值
  status = db_->Put(leveldb::WriteOptions(), "snapshot_key", "updated_value");
  ASSERT_TRUE(status.ok());

  // 不使用快照读取 - 应该得到更新后的值
  status = db_->Get(leveldb::ReadOptions(), "snapshot_key", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("updated_value", value);

  // 使用快照读取 - 应该得到原始值
  leveldb::ReadOptions read_options;
  read_options.snapshot = snapshot;
  status = db_->Get(read_options, "snapshot_key", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("original_value", value);

  // 释放快照
  db_->ReleaseSnapshot(snapshot);
}

// 测试并发写入
TEST_F(LevelDBTest, ConcurrentWrites) {
  leveldb::Status status;
  std::string value;

  // 创建多个WriteBatch进行并发写入
  leveldb::WriteBatch batch1, batch2, batch3;

  batch1.Put("concurrent_key1", "value1");
  batch1.Put("concurrent_key2", "value2");

  batch2.Put("concurrent_key3", "value3");
  batch2.Put("concurrent_key4", "value4");

  batch3.Put("concurrent_key5", "value5");
  batch3.Delete("concurrent_key1");  // 删除一个键

  // 执行批量写入
  status = db_->Write(leveldb::WriteOptions(), &batch1);
  ASSERT_TRUE(status.ok()) << "批量写入1失败: " << status.ToString();

  status = db_->Write(leveldb::WriteOptions(), &batch2);
  ASSERT_TRUE(status.ok()) << "批量写入2失败: " << status.ToString();

  status = db_->Write(leveldb::WriteOptions(), &batch3);
  ASSERT_TRUE(status.ok()) << "批量写入3失败: " << status.ToString();

  // 验证结果
  status = db_->Get(leveldb::ReadOptions(), "concurrent_key1", &value);
  EXPECT_TRUE(status.IsNotFound()) << "concurrent_key1应该被删除";

  status = db_->Get(leveldb::ReadOptions(), "concurrent_key2", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("value2", value);

  status = db_->Get(leveldb::ReadOptions(), "concurrent_key3", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("value3", value);

  status = db_->Get(leveldb::ReadOptions(), "concurrent_key4", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("value4", value);

  status = db_->Get(leveldb::ReadOptions(), "concurrent_key5", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("value5", value);
}

// 测试数据库选项
TEST_F(LevelDBTest, DatabaseOptions) {
  // 关闭数据库
  delete db_;
  db_ = nullptr;

  // 清理旧数据库
  leveldb::Options options;
  leveldb::DestroyDB(db_path_, options);

  // 设置不同的选项
  options.create_if_missing = true;
  options.max_open_files = 1000;
  options.write_buffer_size = 64 * 1024 * 1024;  // 64MB
  options.max_file_size = 2 * 1024 * 1024;       // 2MB
  options.compression = leveldb::kSnappyCompression;

  // 打开数据库
  leveldb::Status status = leveldb::DB::Open(options, db_path_, &db_);
  ASSERT_TRUE(status.ok()) << "无法使用自定义选项打开数据库: "
                           << status.ToString();

  // 测试基本操作
  status = db_->Put(leveldb::WriteOptions(), "options_test_key",
                    "options_test_value");
  ASSERT_TRUE(status.ok());

  std::string value;
  status = db_->Get(leveldb::ReadOptions(), "options_test_key", &value);
  ASSERT_TRUE(status.ok());
  EXPECT_EQ("options_test_value", value);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}