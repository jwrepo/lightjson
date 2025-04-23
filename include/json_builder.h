#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char* buffer;
    size_t capacity;
    size_t length;
} JsonBuilder;

// 创建和销毁
JsonBuilder* json_builder_create(size_t initial_capacity);
void json_builder_free(JsonBuilder* builder);

// 基本操作
bool json_builder_ensure_capacity(JsonBuilder* builder, size_t additional);
bool json_builder_append(JsonBuilder* builder, const char* str);
const char* json_builder_get_string(JsonBuilder* builder);

// JSON结构操作
bool json_builder_start_object(JsonBuilder* builder);
bool json_builder_end_object(JsonBuilder* builder);
bool json_builder_start_array(JsonBuilder* builder);
bool json_builder_end_array(JsonBuilder* builder);

// 添加各种类型的键值对
bool json_builder_add_string(JsonBuilder* builder, const char* key, const char* value);
bool json_builder_add_number(JsonBuilder* builder, const char* key, double value);
bool json_builder_add_bool(JsonBuilder* builder, const char* key, bool value);
bool json_builder_add_null(JsonBuilder* builder, const char* key);

// 控制台编码设置
void set_console_utf8();

#endif // JSON_BUILDER_H