#include <stdio.h>
#include <string.h>
#include <math.h>
#include "unity.h"
#include "json_builder.h"
#include "json_parser.h"

// 测试JSON构建器的基本功能
void test_json_builder_basic() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    // 测试创建简单对象
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_add_string(builder, "name", "张三"));
    TEST_ASSERT(json_builder_add_number(builder, "age", 25));
    TEST_ASSERT(json_builder_add_bool(builder, "is_student", true));
    TEST_ASSERT(json_builder_add_null(builder, "address"));
    TEST_ASSERT(json_builder_end_object(builder));

    const char* json_str = json_builder_get_string(builder);
    TEST_ASSERT_NOT_NULL(json_str);

    // 验证生成的JSON字符串
    const char* expected = "{\"name\":\"张三\",\"age\":25,\"is_student\":true,\"address\":null}";
    TEST_ASSERT_EQUAL_STRING(expected, json_str);

    json_builder_free(builder);
}

// 测试嵌套对象和数组
void test_json_complex_structure() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    // 创建复杂的嵌套结构
    TEST_ASSERT(json_builder_start_object(builder));
    
    // 添加嵌套对象
    TEST_ASSERT(json_builder_append(builder, "\"info\":{\"city\":\"北京\",\"district\":\"海淀\"},"));
    
    // 添加嵌套数组
    TEST_ASSERT(json_builder_append(builder, "\"scores\":[100,98.5,95.8],"));
    
    // 添加混合嵌套
    TEST_ASSERT(json_builder_append(builder, "\"details\":{\"subjects\":[\"数学\",\"物理\"],\"grades\":{\"term1\":95,\"term2\":97}}"));
    
    TEST_ASSERT(json_builder_end_object(builder));

    const char* json_str = json_builder_get_string(builder);
    TEST_ASSERT_NOT_NULL(json_str);

    const char* expected = "{\"info\":{\"city\":\"北京\",\"district\":\"海淀\"},\"scores\":[100,98.5,95.8],\"details\":{\"subjects\":[\"数学\",\"物理\"],\"grades\":{\"term1\":95,\"term2\":97}}}";
    TEST_ASSERT_EQUAL_STRING(expected, json_str);

    json_builder_free(builder);
}

// 测试特殊字符和转义
void test_json_special_chars() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_add_string(builder, "special", "\"引号\\反斜杠\n换行\r回车\t制表符"));
    TEST_ASSERT(json_builder_add_string(builder, "unicode", "\u4F60\u597D"));
    TEST_ASSERT(json_builder_end_object(builder));

    const char* json_str = json_builder_get_string(builder);
    TEST_ASSERT_NOT_NULL(json_str);

    const char* expected = "{\"special\":\"\\\"引号\\\\反斜杠\\n换行\\r回车\\t制表符\",\"unicode\":\"你好\"}";
    TEST_ASSERT_EQUAL_STRING(expected, json_str);

    json_builder_free(builder);
}

// 测试数字格式
void test_json_numbers() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_add_number(builder, "integer", 42));
    TEST_ASSERT(json_builder_add_number(builder, "float", 3.14159));
    TEST_ASSERT(json_builder_add_number(builder, "negative", -123.456));
    TEST_ASSERT(json_builder_add_number(builder, "scientific", 1.23e-4));
    TEST_ASSERT(json_builder_add_number(builder, "zero", 0));
    TEST_ASSERT(json_builder_end_object(builder));

    const char* json_str = json_builder_get_string(builder);
    TEST_ASSERT_NOT_NULL(json_str);

    // 注意：由于浮点数精度问题，这里使用近似比较
    JsonValue* value = json_parse(json_str);
    TEST_ASSERT_NOT_NULL(value);
    JsonObject* obj = json_value_get_object(value);
    TEST_ASSERT_NOT_NULL(obj);

    TEST_ASSERT_EQUAL_INT(42, (int)obj->pairs[0].value->value.number);
    TEST_ASSERT_FLOAT_WITHIN(0.00001, 3.14159, obj->pairs[1].value->value.number);
    TEST_ASSERT_FLOAT_WITHIN(0.00001, -123.456, obj->pairs[2].value->value.number);
    TEST_ASSERT_FLOAT_WITHIN(0.00001, 1.23e-4, obj->pairs[3].value->value.number);
    TEST_ASSERT_EQUAL_INT(0, (int)obj->pairs[4].value->value.number);

    json_value_free(value);
    json_builder_free(builder);
}

// 测试JSON构建器的数组功能
void test_json_builder_array() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    // 测试创建数组
    TEST_ASSERT(json_builder_start_array(builder));
    TEST_ASSERT(json_builder_append(builder, "\"读书\","));
    TEST_ASSERT(json_builder_append(builder, "\"音乐\","));
    TEST_ASSERT(json_builder_append(builder, "\"编程\""));
    TEST_ASSERT(json_builder_end_array(builder));

    const char* json_str = json_builder_get_string(builder);
    TEST_ASSERT_NOT_NULL(json_str);

    // 验证生成的JSON数组
    const char* expected = "[\"读书\",\"音乐\",\"编程\"]";
    TEST_ASSERT_EQUAL_STRING(expected, json_str);

    json_builder_free(builder);
}

// 测试JSON解析器的基本功能
void test_json_parser_basic() {
    const char* json_str = "{\"name\":\"张三\",\"age\":25,\"is_student\":true,\"address\":null}";
    JsonValue* value = json_parse(json_str);
    TEST_ASSERT_NOT_NULL(value);

    // 测试解析对象
    JsonObject* obj = json_value_get_object(value);
    TEST_ASSERT_NOT_NULL(obj);
    TEST_ASSERT_EQUAL_INT(4, obj->size);

    // 测试字符串值
    JsonValue* name = obj->pairs[0].value;
    TEST_ASSERT_EQUAL_STRING("张三", name->value.string);

    // 测试数字值
    JsonValue* age = obj->pairs[1].value;
    TEST_ASSERT_EQUAL_DOUBLE(25, age->value.number, 0.0001);

    // 测试布尔值
    JsonValue* is_student = obj->pairs[2].value;
    TEST_ASSERT(is_student->value.boolean);

    // 测试null值
    JsonValue* address = obj->pairs[3].value;
    TEST_ASSERT_EQUAL_INT(JSON_NULL, address->type);

    json_value_free(value);
}

// 测试JSON解析器的错误处理
void test_json_parser_error() {
    // 测试无效的JSON
    const char* invalid_json = "{\"key\":}";
    JsonValue* value = json_parse(invalid_json);
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_NOT_NULL(json_get_error());

    // 测试不完整的JSON
    const char* incomplete_json = "{\"key\":\"value";
    value = json_parse(incomplete_json);
    TEST_ASSERT_NULL(value);
    TEST_ASSERT_NOT_NULL(json_get_error());
}

// 测试空对象和空数组
void test_json_empty_structures() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    // 测试空对象
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_end_object(builder));
    const char* empty_obj = json_builder_get_string(builder);
    TEST_ASSERT_EQUAL_STRING("{}", empty_obj);

    // 重置构建器
    json_builder_free(builder);
    builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    // 测试空数组
    TEST_ASSERT(json_builder_start_array(builder));
    TEST_ASSERT(json_builder_end_array(builder));
    const char* empty_array = json_builder_get_string(builder);
    TEST_ASSERT_EQUAL_STRING("[]", empty_array);

    json_builder_free(builder);
}

// 测试大数字和边界值
void test_json_number_limits() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_add_number(builder, "max_int", 2147483647));
    TEST_ASSERT(json_builder_add_number(builder, "min_int", -2147483648));
    TEST_ASSERT(json_builder_add_number(builder, "large_float", 1.7976931348623157e+100));
    TEST_ASSERT(json_builder_add_number(builder, "small_float", 2.2250738585072014e-100));
    TEST_ASSERT(json_builder_end_object(builder));

    const char* json_str = json_builder_get_string(builder);
    TEST_ASSERT_NOT_NULL(json_str);

    JsonValue* value = json_parse(json_str);
    TEST_ASSERT_NOT_NULL(value);
    JsonObject* obj = json_value_get_object(value);
    TEST_ASSERT_NOT_NULL(obj);

    TEST_ASSERT_EQUAL_INT(2147483647, (int)obj->pairs[0].value->value.number);
    TEST_ASSERT_EQUAL_INT(-2147483648, (int)obj->pairs[1].value->value.number);
    TEST_ASSERT_FLOAT_WITHIN(1e+100, 1.7976931348623157e+100, obj->pairs[2].value->value.number);
    TEST_ASSERT_FLOAT_WITHIN(1e-100, 2.2250738585072014e-100, obj->pairs[3].value->value.number);

    json_value_free(value);
    json_builder_free(builder);
}

// 测试深层嵌套结构
void test_json_deep_nesting() {
    JsonBuilder* builder = json_builder_create(1024);
    TEST_ASSERT_NOT_NULL(builder);

    // 创建深层嵌套的JSON结构
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_append(builder, "\"level1\":"));
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_append(builder, "\"level2\":"));
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_append(builder, "\"level3\":"));
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_append(builder, "\"level4\":"));
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_append(builder, "\"level5\":"));
    TEST_ASSERT(json_builder_start_object(builder));
    TEST_ASSERT(json_builder_add_number(builder, "value", 42));
    TEST_ASSERT(json_builder_end_object(builder));
    TEST_ASSERT(json_builder_append(builder, ","));
    TEST_ASSERT(json_builder_end_object(builder));
    TEST_ASSERT(json_builder_append(builder, ","));
    TEST_ASSERT(json_builder_end_object(builder));
    TEST_ASSERT(json_builder_append(builder, ","));
    TEST_ASSERT(json_builder_end_object(builder));
    TEST_ASSERT(json_builder_append(builder, ","));
    TEST_ASSERT(json_builder_end_object(builder));
    TEST_ASSERT(json_builder_append(builder, ","));
    TEST_ASSERT(json_builder_end_object(builder));

    const char* json_str = json_builder_get_string(builder);
    printf("%s\n", json_str);
    TEST_ASSERT_NOT_NULL(json_str);

    JsonValue* value = json_parse(json_str);
    TEST_ASSERT_NOT_NULL(value);

    // 验证深层嵌套的值
    JsonObject* obj1 = json_value_get_object(value);
    TEST_ASSERT_NOT_NULL(obj1);
    JsonValue* val1 = obj1->pairs[0].value;
    TEST_ASSERT_NOT_NULL(val1);

    JsonObject* obj2 = json_value_get_object(val1);
    TEST_ASSERT_NOT_NULL(obj2);
    JsonValue* val2 = obj2->pairs[0].value;
    TEST_ASSERT_NOT_NULL(val2);

    JsonObject* obj3 = json_value_get_object(val2);
    TEST_ASSERT_NOT_NULL(obj3);
    JsonValue* val3 = obj3->pairs[0].value;
    TEST_ASSERT_NOT_NULL(val3);

    JsonObject* obj4 = json_value_get_object(val3);
    TEST_ASSERT_NOT_NULL(obj4);
    JsonValue* val4 = obj4->pairs[0].value;
    TEST_ASSERT_NOT_NULL(val4);

    JsonObject* obj5 = json_value_get_object(val4);
    TEST_ASSERT_NOT_NULL(obj5);
    JsonValue* val5 = obj5->pairs[0].value;
    TEST_ASSERT_NOT_NULL(val5);

    // The innermost object has a key "value" with value 42
    JsonObject* innerObj = json_value_get_object(val5);
    TEST_ASSERT_NOT_NULL(innerObj);
    TEST_ASSERT_EQUAL_STRING("value", innerObj->pairs[0].key);
    TEST_ASSERT_EQUAL_INT(42, (int)innerObj->pairs[0].value->value.number);

    json_value_free(value);
    json_builder_free(builder);
}

// 主测试函数
int main() {
    // 设置控制台为UTF-8编码
    set_console_utf8();

    // 初始化测试框架
    unity_begin();

    // 运行所有测试用例
    RUN_TEST(test_json_builder_basic);
    RUN_TEST(test_json_builder_array);
    RUN_TEST(test_json_complex_structure);
    RUN_TEST(test_json_special_chars);
    RUN_TEST(test_json_numbers);
    RUN_TEST(test_json_parser_basic);
    RUN_TEST(test_json_parser_error);
    RUN_TEST(test_json_empty_structures);
    RUN_TEST(test_json_number_limits);
    RUN_TEST(test_json_deep_nesting);

    // 完成测试并显示结果
    unity_end();

    return failed_tests;
}