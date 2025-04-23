#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char* buffer;
    size_t capacity;
    size_t length;
} JsonBuilder;

// 初始化JSON构建器
JsonBuilder* json_builder_create(size_t initial_capacity) {
    JsonBuilder* builder = (JsonBuilder*)malloc(sizeof(JsonBuilder));
    if (!builder) return NULL;
    
    builder->buffer = (char*)malloc(initial_capacity);
    if (!builder->buffer) {
        free(builder);
        return NULL;
    }
    
    builder->capacity = initial_capacity;
    builder->length = 0;
    builder->buffer[0] = '\0';
    
    return builder;
}

// 确保缓冲区有足够空间
bool json_builder_ensure_capacity(JsonBuilder* builder, size_t additional) {
    if (builder->length + additional + 1 > builder->capacity) {
        size_t new_capacity = builder->capacity * 2;
        if (new_capacity < builder->length + additional + 1)
            new_capacity = builder->length + additional + 1;
        
        char* new_buffer = (char*)realloc(builder->buffer, new_capacity);
        if (!new_buffer) return false;
        
        builder->buffer = new_buffer;
        builder->capacity = new_capacity;
    }
    return true;
}

// 添加字符串到构建器
bool json_builder_append(JsonBuilder* builder, const char* str) {
    size_t len = strlen(str);
    if (!json_builder_ensure_capacity(builder, len)) return false;
    
    strcpy(builder->buffer + builder->length, str);
    builder->length += len;
    
    return true;
}

// 开始一个新的JSON对象
bool json_builder_start_object(JsonBuilder* builder) {
    return json_builder_append(builder, "{");
}

// 结束当前JSON对象
bool json_builder_end_object(JsonBuilder* builder) {
    // 移除最后一个逗号（如果有）
    if (builder->length > 0 && builder->buffer[builder->length - 1] == ',') {
        builder->buffer[builder->length - 1] = '}';
    } else {
        if (!json_builder_append(builder, "}")) return false;
    }
    return true;
}

// 开始一个新的JSON数组
bool json_builder_start_array(JsonBuilder* builder) {
    return json_builder_append(builder, "[");
}

// 结束当前JSON数组
bool json_builder_end_array(JsonBuilder* builder) {
    // 移除最后一个逗号（如果有）
    if (builder->length > 0 && builder->buffer[builder->length - 1] == ',') {
        builder->buffer[builder->length - 1] = ']';
    } else {
        if (!json_builder_append(builder, "]")) return false;
    }
    return true;
}

// 添加字符串键值对
bool json_builder_add_string(JsonBuilder* builder, const char* key, const char* value) {
    if (!json_builder_ensure_capacity(builder, strlen(key) + strlen(value) + 10)) 
        return false;
    
    char* escaped_value = NULL;
    size_t escaped_len = 0;
    
    // 简单的字符串转义处理
    for (const char* p = value; *p; p++) {
        if (*p == '\"' || *p == '\\' || *p == '\n' || *p == '\r' || *p == '\t')
            escaped_len += 2;
        else
            escaped_len++;
    }
    
    escaped_value = (char*)malloc(escaped_len + 1);
    if (!escaped_value) return false;
    
    char* q = escaped_value;
    for (const char* p = value; *p; p++) {
        if (*p == '\"') { *q++ = '\\'; *q++ = '\"'; }
        else if (*p == '\\') { *q++ = '\\'; *q++ = '\\'; }
        else if (*p == '\n') { *q++ = '\\'; *q++ = 'n'; }
        else if (*p == '\r') { *q++ = '\\'; *q++ = 'r'; }
        else if (*p == '\t') { *q++ = '\\'; *q++ = 't'; }
        else *q++ = *p;
    }
    *q = '\0';
    
    char temp[1024];
    sprintf(temp, "\"%s\":\"%s\",", key, escaped_value);
    free(escaped_value);
    
    return json_builder_append(builder, temp);
}

// 添加数字键值对
bool json_builder_add_number(JsonBuilder* builder, const char* key, double value) {
    if (!json_builder_ensure_capacity(builder, strlen(key) + 32)) 
        return false;
    
    char temp[1024];
    sprintf(temp, "\"%s\":%.10g,", key, value);
    
    return json_builder_append(builder, temp);
}

// 添加布尔键值对
bool json_builder_add_bool(JsonBuilder* builder, const char* key, bool value) {
    if (!json_builder_ensure_capacity(builder, strlen(key) + 10)) 
        return false;
    
    char temp[1024];
    sprintf(temp, "\"%s\":%s,", key, value ? "true" : "false");
    
    return json_builder_append(builder, temp);
}

// 添加null键值对
bool json_builder_add_null(JsonBuilder* builder, const char* key) {
    if (!json_builder_ensure_capacity(builder, strlen(key) + 10)) 
        return false;
    
    char temp[1024];
    sprintf(temp, "\"%s\":null,", key);
    
    return json_builder_append(builder, temp);
}

// 获取构建的JSON字符串
const char* json_builder_get_string(JsonBuilder* builder) {
    return builder->buffer;
}

// 释放JSON构建器
void json_builder_free(JsonBuilder* builder) {
    if (builder) {
        free(builder->buffer);
        free(builder);
    }
}

// 新增函数：设置控制台编码
void set_console_utf8() {
#ifdef _WIN32
    // 设置控制台输出代码页为UTF-8
    system("chcp 65001 > nul");
#endif
}