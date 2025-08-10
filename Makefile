CC = g++
CFLAGS = -std=c++17 -Wall -g 
# 添加ASAN选项，默认为no
ASAN = no
ifeq ($(ASAN),yes)
    CFLAGS += -fsanitize=address -fno-omit-frame-pointer
    LDFLAGS += -fsanitize=address
endif

INCLUDES = -I.
INCLUDES += -I./src/common
INCLUDES += -I./src/main
INCLUDES += -I./src/util
INCLUDES += -I./src/misc
INCLUDES += -I./src/core

INCLUDES += -I./third_party/googletest/googletest/include 
INCLUDES += -I./third_party/spdlog/include 
INCLUDES += -I./third_party/nlohmann_json/single_include 
INCLUDES += -I./third_party/rocksdb/include
INCLUDES += -I./third_party/leveldb/include
INCLUDES += -I./third_party/hnswlib
INCLUDES += -I./third_party/protobuf/src
INCLUDES += -I./third_party/cxxopts/include

LDFLAGS = -L./third_party/googletest/build/lib

LIBS = -lgtest -lgtest_main -lpthread -lstdc++fs 

LIBS_ROCKSDB = ./third_party/rocksdb/librocksdb.a -lz -lsnappy -llz4 -lzstd -lbz2
LIBS_LEVELDB = ./third_party/leveldb/build/libleveldb.a -lsnappy
LIBS_PROTOBUF = ./third_party/protobuf/src/.libs/libprotobuf.a

# 目录结构
SRC_DIR = src
OBJ_DIR = output/obj
TEST_DIR = output/test
MAIN_DIR = output/main

# 默认目标
all: prepare test

# 准备目录
prepare:
	@mkdir -p $(OBJ_DIR)/main
	@mkdir -p $(OBJ_DIR)/common
	@mkdir -p $(OBJ_DIR)/misc
	@mkdir -p $(OBJ_DIR)/util
	@mkdir -p $(OBJ_DIR)/core
	@mkdir -p $(TEST_DIR)
	@mkdir -p $(MAIN_DIR)

################################################
# 源程序 -> 目标文件
$(OBJ_DIR)/main/%.o: $(SRC_DIR)/main/%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/common/%.o: $(SRC_DIR)/common/%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/misc/%.o: $(SRC_DIR)/misc/%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/util/%.o: $(SRC_DIR)/util/%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/core/%.o: $(SRC_DIR)/core/%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

################################################
# 目标文件定义

#-----------------------------------------------
# src/main
MAIN_OBJS = $(OBJ_DIR)/main/main.o

#-----------------------------------------------
#src/common
RETNO_OBJS = $(OBJ_DIR)/common/retno.o
RETNO_TEST_OBJS = $(OBJ_DIR)/common/retno_test.o

#-----------------------------------------------
#src/misc
HNSWLIB_TEST_OBJS = $(OBJ_DIR)/misc/hnswlib_test.o
JSON_TEST_OBJS = $(OBJ_DIR)/misc/json_test.o
ROCKSDB_TEST_OBJS = $(OBJ_DIR)/misc/rocksdb_test.o
LEVELDB_TEST_OBJS = $(OBJ_DIR)/misc/leveldb_test.o

PERSON_PB_OBJS = $(OBJ_DIR)/misc/person.pb.o
VDB_PB_OBJS = $(OBJ_DIR)/misc/vdb.pb.o
PROTOBUF_TEST_OBJS = $(OBJ_DIR)/misc/protobuf_test.o
CXXOPTS_TEST_OBJS = $(OBJ_DIR)/misc/cxxopts_test.o

#-----------------------------------------------
#src/util
LOGGER_OBJS = $(OBJ_DIR)/util/logger.o
LOGGER_TEST_OBJS = $(OBJ_DIR)/util/logger_test.o

TIMESTAMP_OBJS = $(OBJ_DIR)/util/timestamp.o
TIMESTAMP_TEST_OBJS = $(OBJ_DIR)/util/timestamp_test.o

UTIL_OBJS = $(OBJ_DIR)/util/util.o
UTIL_TEST_OBJS = $(OBJ_DIR)/util/util_test.o

CONFIG_OBJS = $(OBJ_DIR)/util/config.o
CONFIG_TEST_OBJS = $(OBJ_DIR)/util/config_test.o

CONFIG2_OBJS = $(OBJ_DIR)/util/config2.o
CONFIG2_TEST_OBJS = $(OBJ_DIR)/util/config2_test.o

#-----------------------------------------------
#src/core
CORE_OBJS = $(OBJ_DIR)/core/core.o
CORE_TEST_OBJS = $(OBJ_DIR)/core/core_test.o

ENGINE_OBJS = $(OBJ_DIR)/core/engine.o
ENGINE_TEST_OBJS = $(OBJ_DIR)/core/engine_test.o

################################################
# 可执行程序定义

#-----------------------------------------------
#src/main
MAIN = $(MAIN_DIR)/vbenchmark

#-----------------------------------------------
#src/common
RETNO_TEST = $(TEST_DIR)/retno_test

#-----------------------------------------------
#src/misc
HNSWLIB_TEST = $(TEST_DIR)/hnswlib_test
JSON_TEST = $(TEST_DIR)/json_test
ROCKSDB_TEST = $(TEST_DIR)/rocksdb_test
LEVELDB_TEST = $(TEST_DIR)/leveldb_test
PROTOBUF_TEST = $(TEST_DIR)/protobuf_test
CXXOPTS_TEST = $(TEST_DIR)/cxxopts_test

#-----------------------------------------------
#src/util
LOGGER_TEST = $(TEST_DIR)/logger_test
TIMESTAMP_TEST = $(TEST_DIR)/timestamp_test
UTIL_TEST = $(TEST_DIR)/util_test
CONFIG_TEST = $(TEST_DIR)/config_test
CONFIG2_TEST = $(TEST_DIR)/config2_test

#-----------------------------------------------
#src/core
CORE_TEST = $(TEST_DIR)/core_test
ENGINE_TEST = $(TEST_DIR)/engine_test

################################################
# 可执行程序依赖

#-----------------------------------------------
# src/main
$(MAIN): $(MAIN_OBJS) $(CONFIG2_OBJS) $(UTIL_OBJS) $(VDB_PB_OBJS) $(CORE_OBJS) $(LOGGER_OBJS) $(TIMESTAMP_OBJS) $(ENGINE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(LIBS_PROTOBUF) $(LIBS_LEVELDB) $(LIBS_ROCKSDB)

#-----------------------------------------------
#src/common
$(RETNO_TEST): $(RETNO_OBJS) $(RETNO_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

#-----------------------------------------------
#src/misc
$(HNSWLIB_TEST): $(HNSWLIB_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(JSON_TEST): $(JSON_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(ROCKSDB_TEST): $(ROCKSDB_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(LIBS_ROCKSDB)

$(LEVELDB_TEST): $(LEVELDB_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(LIBS_LEVELDB)

$(PROTOBUF_TEST): $(PERSON_PB_OBJS) $(PROTOBUF_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(LIBS_PROTOBUF)

$(CXXOPTS_TEST): $(CXXOPTS_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

#-----------------------------------------------
#src/util
$(LOGGER_TEST): $(LOGGER_OBJS) $(LOGGER_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(TIMESTAMP_TEST): $(TIMESTAMP_OBJS) $(TIMESTAMP_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(UTIL_TEST): $(UTIL_OBJS) $(UTIL_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(CONFIG_TEST): $(CONFIG_OBJS) $(CONFIG_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(CONFIG2_TEST): $(CONFIG2_OBJS) $(CONFIG2_TEST_OBJS) $(UTIL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

#-----------------------------------------------
#src/core
$(CORE_TEST): $(CORE_OBJS) $(CORE_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(ENGINE_TEST): $(ENGINE_TEST_OBJS) $(CONFIG2_OBJS) $(UTIL_OBJS) $(VDB_PB_OBJS) $(CORE_OBJS) $(LOGGER_OBJS) $(TIMESTAMP_OBJS) $(ENGINE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(LIBS_PROTOBUF) $(LIBS_LEVELDB) $(LIBS_ROCKSDB)

################################################
# 可执行程序

#-----------------------------------------------
# src/main
main: prepare $(MAIN)

#-----------------------------------------------
#src/common
retno_test: prepare $(RETNO_TEST)

#-----------------------------------------------
#src/misc
hnswlib_test: prepare $(HNSWLIB_TEST)
json_test: prepare $(JSON_TEST)
rocksdb_test: prepare $(ROCKSDB_TEST)
leveldb_test: prepare $(LEVELDB_TEST)
protobuf_test: prepare $(PROTOBUF_TEST)
cxxopts_test: prepare $(CXXOPTS_TEST)

#-----------------------------------------------
#src/util
logger_test: prepare $(LOGGER_TEST)
timestamp_test: prepare $(TIMESTAMP_TEST)
util_test: prepare $(UTIL_TEST)
config_test: prepare $(CONFIG_TEST)
config2_test: prepare $(CONFIG2_TEST)

#-----------------------------------------------
#src/core
core_test: prepare $(CORE_TEST)
engine_test: prepare $(ENGINE_TEST)

################################################
proto:
	./third_party/protobuf/src/protoc --cpp_out=. src/misc/person.proto
	./third_party/protobuf/src/protoc --cpp_out=. src/misc/vdb.proto

# 编译测试
test: prepare \
      main \
      retno_test \
      hnswlib_test \
      json_test \
      rocksdb_test \
	  leveldb_test \
      protobuf_test \
      cxxopts_test \
      logger_test \
      timestamp_test \
      util_test \
      config_test \
      config2_test \
      core_test \
      engine_test

# 运行测试
run_test: 
	./$(RETNO_TEST)
	./$(HNSWLIB_TEST)
	./$(JSON_TEST)
	./$(ROCKSDB_TEST)
	./$(PROTOBUF_TEST)
	./$(CXXOPTS_TEST)
	./$(LOGGER_TEST)
	./$(TIMESTAMP_TEST)
	./$(UTIL_TEST)
	./$(CONFIG_TEST)
	./$(CONFIG2_TEST)
	./$(CORE_TEST)
	./$(ENGINE_TEST)

# 清理
clean:
	rm -rf $(OBJ_DIR)/* $(TEST_DIR)/* $(MAIN_DIR)/*

.PHONY: all prepare test clean run_test

format:
	clang-format --style=Google -i `find ./src -type f \( -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cpp" \) | grep -v "\.pb\."`