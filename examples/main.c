#include <stdio.h>
#include "json_builder.h"
#include "json_parser.h"

int main() {
    // 设置控制台为UTF-8编码
    set_console_utf8();

    // 创建一个JSON字符串用于测试
    JsonBuilder* builder = json_builder_create(1024);
    json_builder_start_object(builder);
    json_builder_add_string(builder, "name", "张三");
    json_builder_add_number(builder, "age", 25);
    json_builder_add_bool(builder, "is_student", true);
    json_builder_add_null(builder, "address");
    
    // 添加一个数组
    json_builder_append(builder, "\"hobbies\":");
    json_builder_start_array(builder);
    json_builder_append(builder, "\"读书\",");
    json_builder_append(builder, "\"音乐\",");
    json_builder_append(builder, "\"编程\"");
    json_builder_end_array(builder);
    json_builder_append(builder, ",");

    // 添加一个嵌套对象
    json_builder_append(builder, "\"contact\":");
    json_builder_start_object(builder);
    json_builder_add_string(builder, "email", "zhangsan@example.com");
    json_builder_add_string(builder, "phone", "123456789");
    json_builder_end_object(builder);

    json_builder_end_object(builder);

    // 获取构建的JSON字符串
    const char* json_str = json_builder_get_string(builder);
    printf("构建的JSON字符串:\n%s\n\n", json_str);

    // 解析JSON字符串
    JsonValue* value = json_parse(json_str);
    if (!value) {
        printf("解析错误: %s\n", json_get_error());
        json_builder_free(builder);
        return 1;
    }

    // 访问解析后的值
    JsonObject* obj = json_value_get_object(value);
    if (obj) {
        for (size_t i = 0; i < obj->size; i++) {
            printf("键: %s, ", obj->pairs[i].key);
            JsonValue* v = obj->pairs[i].value;
            switch (v->type) {
                case JSON_STRING:
                    printf("值(字符串): %s\n", v->value.string);
                    break;
                case JSON_NUMBER:
                    printf("值(数字): %f\n", v->value.number);
                    break;
                case JSON_BOOL:
                    printf("值(布尔): %s\n", v->value.boolean ? "true" : "false");
                    break;
                case JSON_NULL:
                    printf("值: null\n");
                    break;
                case JSON_ARRAY: {
                    printf("值(数组): [");
                    JsonArray* arr = v->value.array;
                    for (size_t j = 0; j < arr->size; j++) {
                        JsonValue* elem = arr->elements[j];
                        if (elem->type == JSON_STRING) {
                            printf("\"%s\"", elem->value.string);
                        }
                        if (j < arr->size - 1) printf(", ");
                    }
                    printf("]\n");
                    break;
                }
                case JSON_OBJECT: {
                    printf("值(对象): {\n");
                    JsonObject* nested = v->value.object;
                    for (size_t j = 0; j < nested->size; j++) {
                        printf("    %s: ", nested->pairs[j].key);
                        JsonValue* nested_v = nested->pairs[j].value;
                        if (nested_v->type == JSON_STRING) {
                            printf("\"%s\"\n", nested_v->value.string);
                        }
                    }
                    printf("}\n");
                    break;
                }
            }
        }
    }

    // 测试错误处理
    printf("\n测试错误处理:\n");
    const char* invalid_json = "{\"key\":}";
    JsonValue* invalid_value = json_parse(invalid_json);
    if (!invalid_value) {
        printf("预期的错误: %s\n", json_get_error());
    }

    // 释放资源
    json_value_free(value);
    json_builder_free(builder);

    return 0;
}