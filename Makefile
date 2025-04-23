CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Itest
LDFLAGS =

# 目标文件
LIB_NAME = libjson
MAIN_TARGET = json_example
TEST_TARGET = json_test

# 目录结构
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test
EXAMPLES_DIR = examples
BUILD_DIR = build

# 源文件
LIB_SOURCES = $(wildcard $(SRC_DIR)/*.c)
EXAMPLE_SOURCES = $(wildcard $(EXAMPLES_DIR)/*.c)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)

# 目标文件
LIB_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_SOURCES))
EXAMPLE_OBJECTS = $(patsubst $(EXAMPLES_DIR)/%.c,$(BUILD_DIR)/%.o,$(EXAMPLE_SOURCES))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

# 创建build目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 默认目标
all: $(BUILD_DIR) $(LIB_NAME) $(MAIN_TARGET) $(TEST_TARGET)

# 静态库
$(LIB_NAME): $(LIB_OBJECTS)
	ar rcs $(BUILD_DIR)/$@.a $^

# 示例程序
$(MAIN_TARGET): $(EXAMPLE_OBJECTS) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $(EXAMPLE_OBJECTS) -L$(BUILD_DIR) -ljson $(LDFLAGS)

# 测试程序
$(TEST_TARGET): $(TEST_OBJECTS) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $(TEST_OBJECTS) -L$(BUILD_DIR) -ljson $(LDFLAGS)

# 编译规则 - 库源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 编译规则 - 示例源文件
$(BUILD_DIR)/%.o: $(EXAMPLES_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 编译规则 - 测试源文件
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 运行测试
test: $(TEST_TARGET)
	$(BUILD_DIR)/$(TEST_TARGET)

# 运行示例
run: $(MAIN_TARGET)
	$(BUILD_DIR)/$(MAIN_TARGET)

# 清理
clean:
	rm -rf $(BUILD_DIR)

# 安装
install: $(LIB_NAME)
	mkdir -p /usr/local/include/json
	mkdir -p /usr/local/lib
	cp $(INCLUDE_DIR)/*.h /usr/local/include/json/
	cp $(BUILD_DIR)/$(LIB_NAME).a /usr/local/lib/

# 卸载
uninstall:
	rm -rf /usr/local/include/json
	rm -f /usr/local/lib/$(LIB_NAME).a

.PHONY: all test run clean install uninstall
