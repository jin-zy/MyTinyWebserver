# 定义编译器
CXX = g++

# 编译选项
CFLAGS = -g -Wall -std=c++14 -O2 -pthread

# 源文件目录
SRC_DIR = src
# 目标文件目录
BUILD_DIR = build
# 可执行文件输出目录
BIN_DIR = $(BUILD_DIR)/bin

# 源文件
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
# 目标文件
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
# 可执行文件名
TARGET = $(BIN_DIR)/server

# 创建目录
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# 自动创建目标文件目录
$(BUILD_DIR)/%:
	mkdir -p $(@D)

# 编译目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BIN_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

# 链接目标文件
$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(TARGET) -pthread -lmysqlclient

all: $(TARGET)

# 清理
clean:
	rm -rf $(BUILD_DIR)

# 运行
run: $(TARGET)
	$(TARGET)

.PHONY: all clean run