#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdbool.h>
#include <stddef.h>

// JSON值类型枚举
typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JsonValueType;

// 前向声明
struct JsonValue;
struct JsonObject;
struct JsonArray;

// JSON值结构体
typedef struct JsonValue {
    JsonValueType type;
    union {
        bool boolean;
        double number;
        char* string;
        struct JsonArray* array;
        struct JsonObject* object;
    } value;
} JsonValue;

// JSON对象键值对
typedef struct JsonKeyValue {
    char* key;
    JsonValue* value;
} JsonKeyValue;

// JSON对象结构体
typedef struct JsonObject {
    JsonKeyValue* pairs;
    size_t size;
    size_t capacity;
} JsonObject;

// JSON数组结构体
typedef struct JsonArray {
    JsonValue** elements;
    size_t size;
    size_t capacity;
} JsonArray;

// 解析器结构体
typedef struct {
    const char* json;
    size_t pos;
    size_t len;
} JsonParser;

// 创建和销毁函数
JsonParser* json_parser_create(const char* json);
void json_parser_free(JsonParser* parser);

// 解析函数
JsonValue* json_parse(const char* json);
JsonValue* json_parse_value(JsonParser* parser);
JsonObject* json_parse_object(JsonParser* parser);
JsonArray* json_parse_array(JsonParser* parser);
char* json_parse_string(JsonParser* parser);
double json_parse_number(JsonParser* parser);
bool json_parse_bool(JsonParser* parser);
void json_parse_null(JsonParser* parser);

// 值操作函数
void json_value_free(JsonValue* value);
const char* json_value_get_string(JsonValue* value);
double json_value_get_number(JsonValue* value);
bool json_value_get_bool(JsonValue* value);
JsonObject* json_value_get_object(JsonValue* value);
JsonArray* json_value_get_array(JsonValue* value);

// 错误处理
const char* json_get_error(void);

#endif // JSON_PARSER_H