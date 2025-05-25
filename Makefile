# 编译器配置
CC := g++
CFLAGS := -std=c++14 -O2 -Wall -Wextra -I./src
LDFLAGS := -pthread -lmysqlclient
DEBUG_FLAGS := -g -DDEBUG
RELEASE_FLAGS := -O3
MYSQL_INCLUDE := $(shell mysql_config --include | sed 's/-I//') 
MYSQL_LIB := $(shell mysql_config --libs | sed 's/-L//' | awk '{print $1}')

# 目录配置
SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR := build/bin
TARGET := $(BIN_DIR)/server

# 自动收集源文件
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

# 添加MySQL头文件路径
CFLAGS += -I$(MYSQL_INCLUDE)
LDFLAGS += -L$(MYSQL_LIB)

.PHONY: all clean debug release

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "清理完成"

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

release: CFLAGS += $(RELEASE_FLAGS)
release: all

# 调试用目标
print-%:
	@echo $* = $($*)
