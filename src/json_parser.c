#include "json_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 错误消息缓冲区
static char error_message[256] = {0};

// 设置错误消息
static void set_error(const char* msg) {
    strncpy(error_message, msg, sizeof(error_message) - 1);
}

// 获取错误消息
const char* json_get_error(void) {
    return error_message;
}

// 创建解析器
JsonParser* json_parser_create(const char* json) {
    JsonParser* parser = (JsonParser*)malloc(sizeof(JsonParser));
    if (!parser) {
        set_error("内存分配失败");
        return NULL;
    }
    parser->json = json;
    parser->pos = 0;
    parser->len = strlen(json);
    return parser;
}

// 跳过空白字符
static void skip_whitespace(JsonParser* parser) {
    while (parser->pos < parser->len && 
           (parser->json[parser->pos] == ' ' || 
            parser->json[parser->pos] == '\n' || 
            parser->json[parser->pos] == '\r' || 
            parser->json[parser->pos] == '\t')) {
        parser->pos++;
    }
}

// 解析字符串
char* json_parse_string(JsonParser* parser) {
    if (parser->json[parser->pos] != '"') {
        set_error("预期字符串应以引号开始");
        return NULL;
    }
    parser->pos++;

    size_t start = parser->pos;
    size_t len = 0;
    bool escaped = false;

    while (parser->pos < parser->len) {
        char c = parser->json[parser->pos];
        if (escaped) {
            escaped = false;
            len++;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '"') {
            break;
        } else {
            len++;
        }
        parser->pos++;
    }

    if (parser->pos >= parser->len) {
        set_error("字符串未正确结束");
        return NULL;
    }

    char* str = (char*)malloc(len + 1);
    if (!str) {
        set_error("内存分配失败");
        return NULL;
    }

    size_t j = 0;
    escaped = false;
    for (size_t i = start; i < parser->pos; i++) {
        if (escaped) {
            switch (parser->json[i]) {
                case 'n': str[j++] = '\n'; break;
                case 'r': str[j++] = '\r'; break;
                case 't': str[j++] = '\t'; break;
                case '\\': str[j++] = '\\'; break;
                case '"': str[j++] = '"'; break;
                default: str[j++] = parser->json[i];
            }
            escaped = false;
        } else if (parser->json[i] == '\\') {
            escaped = true;
        } else {
            str[j++] = parser->json[i];
        }
    }
    str[j] = '\0';
    parser->pos++;
    return str;
}

// 解析数字
double json_parse_number(JsonParser* parser) {
    char* endptr;
    double num = strtod(parser->json + parser->pos, &endptr);
    if (endptr == parser->json + parser->pos) {
        set_error("无效的数字格式");
        return 0;
    }
    parser->pos = endptr - parser->json;
    return num;
}

// 解析布尔值
bool json_parse_bool(JsonParser* parser) {
    if (strncmp(parser->json + parser->pos, "true", 4) == 0) {
        parser->pos += 4;
        return true;
    } else if (strncmp(parser->json + parser->pos, "false", 5) == 0) {
        parser->pos += 5;
        return false;
    }
    set_error("无效的布尔值");
    return false;
}

// 解析null
void json_parse_null(JsonParser* parser) {
    if (strncmp(parser->json + parser->pos, "null", 4) == 0) {
        parser->pos += 4;
        return;
    }
    set_error("无效的null值");
}

// 前向声明
JsonValue* json_parse_value(JsonParser* parser);

// 解析数组
JsonArray* json_parse_array(JsonParser* parser) {
    if (parser->json[parser->pos] != '[') {
        set_error("预期数组应以'['开始");
        return NULL;
    }
    parser->pos++;

    JsonArray* array = (JsonArray*)malloc(sizeof(JsonArray));
    if (!array) {
        set_error("内存分配失败");
        return NULL;
    }
    array->capacity = 8;
    array->size = 0;
    array->elements = (JsonValue**)malloc(sizeof(JsonValue*) * array->capacity);
    if (!array->elements) {
        free(array);
        set_error("内存分配失败");
        return NULL;
    }

    skip_whitespace(parser);
    if (parser->json[parser->pos] == ']') {
        parser->pos++;
        return array;
    }

    while (parser->pos < parser->len) {
        JsonValue* value = json_parse_value(parser);
        if (!value) {
            // 错误信息已在parse_value中设置
            return NULL;
        }

        if (array->size >= array->capacity) {
            size_t new_capacity = array->capacity * 2;
            JsonValue** new_elements = (JsonValue**)realloc(array->elements, 
                                                          sizeof(JsonValue*) * new_capacity);
            if (!new_elements) {
                set_error("内存分配失败");
                return NULL;
            }
            array->elements = new_elements;
            array->capacity = new_capacity;
        }

        array->elements[array->size++] = value;

        skip_whitespace(parser);
        if (parser->json[parser->pos] == ']') {
            parser->pos++;
            return array;
        }

        if (parser->json[parser->pos] != ',') {
            set_error("预期','或']'");
            return NULL;
        }
        parser->pos++;
        skip_whitespace(parser);
    }

    set_error("数组未正确结束");
    return NULL;
}

// 解析对象
JsonObject* json_parse_object(JsonParser* parser) {
    if (parser->json[parser->pos] != '{') {
        set_error("预期对象应以'{'开始");
        return NULL;
    }
    parser->pos++;

    JsonObject* object = (JsonObject*)malloc(sizeof(JsonObject));
    if (!object) {
        set_error("内存分配失败");
        return NULL;
    }
    memset(object, 0, sizeof(JsonObject));
    object->capacity = 8;
    object->pairs = (JsonKeyValue*)malloc(sizeof(JsonKeyValue) * object->capacity);
    if (!object->pairs) {
        free(object);
        set_error("内存分配失败");
        return NULL;
    }
    memset(object->pairs, 0, sizeof(JsonKeyValue) * object->capacity);

    skip_whitespace(parser);
    if (parser->json[parser->pos] == '}') {
        parser->pos++;
        return object;
    }

    while (parser->pos < parser->len) {
        skip_whitespace(parser);
        char* key = json_parse_string(parser);
        if (!key) return NULL;

        skip_whitespace(parser);
        if (parser->json[parser->pos] != ':') {
            set_error("预期':'");
            free(key);
            return NULL;
        }
        parser->pos++;

        skip_whitespace(parser);
        JsonValue* value = json_parse_value(parser);
        if (!value) {
            free(key);
            return NULL;
        }

        if (object->size >= object->capacity) {
            size_t new_capacity = object->capacity * 2;
            JsonKeyValue* new_pairs = (JsonKeyValue*)realloc(object->pairs, 
                                                           sizeof(JsonKeyValue) * new_capacity);
            if (!new_pairs) {
                free(key);
                json_value_free(value);
                set_error("内存分配失败");
                return NULL;
            }
            memset(new_pairs + object->capacity, 0, sizeof(JsonKeyValue) * (new_capacity - object->capacity));
            object->pairs = new_pairs;
            object->capacity = new_capacity;
        }

        // 确保键值对正确存储
        object->pairs[object->size].key = strdup(key);
        object->pairs[object->size].value = value;
        object->size++;
        free(key);  // 释放原始key，因为我们已经复制了它

        skip_whitespace(parser);
        if (parser->json[parser->pos] == '}') {
            parser->pos++;
            return object;
        }

        if (parser->json[parser->pos] != ',') {
            set_error("预期','或'}'");
            return NULL;
        }
        parser->pos++;
    }

    set_error("对象未正确结束");
    return NULL;
}

// 解析JSON值
JsonValue* json_parse_value(JsonParser* parser) {
    skip_whitespace(parser);

    JsonValue* value = (JsonValue*)malloc(sizeof(JsonValue));
    if (!value) {
        set_error("内存分配失败");
        return NULL;
    }

    char c = parser->json[parser->pos];
    switch (c) {
        case '"':
            value->type = JSON_STRING;
            value->value.string = json_parse_string(parser);
            if (!value->value.string) {
                free(value);
                return NULL;
            }
            break;

        case '{':
            value->type = JSON_OBJECT;
            value->value.object = json_parse_object(parser);
            if (!value->value.object) {
                free(value);
                return NULL;
            }
            break;

        case '[':
            value->type = JSON_ARRAY;
            value->value.array = json_parse_array(parser);
            if (!value->value.array) {
                free(value);
                return NULL;
            }
            break;

        case 't':
        case 'f':
            value->type = JSON_BOOL;
            value->value.boolean = json_parse_bool(parser);
            if (parser->pos >= parser->len) {
                free(value);
                return NULL;
            }
            break;

        case 'n':
            value->type = JSON_NULL;
            json_parse_null(parser);
            if (parser->pos >= parser->len) {
                free(value);
                return NULL;
            }
            break;

        default:
            if (c == '-' || isdigit(c)) {
                value->type = JSON_NUMBER;
                value->value.number = json_parse_number(parser);
                if (parser->pos >= parser->len) {
                    free(value);
                    return NULL;
                }
            } else {
                set_error("无效的JSON值");
                free(value);
                return NULL;
            }
    }

    return value;
}

// 解析JSON字符串
JsonValue* json_parse(const char* json) {
    JsonParser* parser = json_parser_create(json);
    if (!parser) return NULL;

    JsonValue* value = json_parse_value(parser);
    skip_whitespace(parser);

    if (parser->pos < parser->len) {
        set_error("JSON字符串后存在额外字符");
        json_value_free(value);
        value = NULL;
    }

    json_parser_free(parser);
    return value;
}

// 释放解析器
void json_parser_free(JsonParser* parser) {
    free(parser);
}

// 释放JSON值
void json_value_free(JsonValue* value) {
    if (!value) return;

    switch (value->type) {
        case JSON_STRING:
            free(value->value.string);
            break;

        case JSON_ARRAY:
            if (value->value.array) {
                for (size_t i = 0; i < value->value.array->size; i++) {
                    json_value_free(value->value.array->elements[i]);
                }
                free(value->value.array->elements);
                free(value->value.array);
            }
            break;

        case JSON_OBJECT:
            if (value->value.object) {
                for (size_t i = 0; i < value->value.object->size; i++) {
                    free(value->value.object->pairs[i].key);
                    json_value_free(value->value.object->pairs[i].value);
                }
                free(value->value.object->pairs);
                free(value->value.object);
            }
            break;

        default:
            break;
    }

    free(value);
}

// 获取值操作函数实现
const char* json_value_get_string(JsonValue* value) {
    return (value && value->type == JSON_STRING) ? value->value.string : NULL;
}

double json_value_get_number(JsonValue* value) {
    return (value && value->type == JSON_NUMBER) ? value->value.number : 0.0;
}

bool json_value_get_bool(JsonValue* value) {
    return (value && value->type == JSON_BOOL) ? value->value.boolean : false;
}

JsonObject* json_value_get_object(JsonValue* value) {
    return (value && value->type == JSON_OBJECT) ? value->value.object : NULL;
}

JsonArray* json_value_get_array(JsonValue* value) {
    return (value && value->type == JSON_ARRAY) ? value->value.array : NULL;
}