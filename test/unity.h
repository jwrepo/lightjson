#ifndef UNITY_H
#define UNITY_H

#include <stdio.h>
#include <stdbool.h>

// 测试计数器
extern int total_tests;
extern int failed_tests;

// 测试宏
#define TEST_ASSERT(condition) do { \
    total_tests++; \
    if (!(condition)) { \
        failed_tests++; \
        printf("❌ 断言失败: %s (行 %d)\n", #condition, __LINE__); \
    } else { \
        printf("✓ 测试通过: %s\n", #condition); \
    } \
} while (0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) do { \
    total_tests++; \
    if (strcmp(expected, actual) != 0) { \
        failed_tests++; \
        printf("❌ 字符串不匹配:\n  期望: %s\n  实际: %s\n  (行 %d)\n", expected, actual, __LINE__); \
    } else { \
        printf("✓ 字符串匹配: %s\n", expected); \
    } \
} while (0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) do { \
    total_tests++; \
    if (expected != actual) { \
        failed_tests++; \
        printf("❌ 整数不匹配:\n  期望: %d\n  实际: %d\n  (行 %d)\n", expected, actual, __LINE__); \
    } else { \
        printf("✓ 整数匹配: %d\n", expected); \
    } \
} while (0)

#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual, epsilon) do { \
    total_tests++; \
    if (fabs(expected - actual) > epsilon) { \
        failed_tests++; \
        printf("❌ 浮点数不匹配:\n  期望: %f\n  实际: %f\n  (行 %d)\n", expected, actual, __LINE__); \
    } else { \
        printf("✓ 浮点数匹配: %f\n", expected); \
    } \
} while (0)

#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) do { \
    total_tests++; \
    if (fabs(expected - actual) > delta) { \
        failed_tests++; \
        printf("❌ 浮点数超出允许范围:\n  期望: %f\n  实际: %f\n  允许误差: %f\n  (行 %d)\n", expected, actual, delta, __LINE__); \
    } else { \
        printf("✓ 浮点数在允许范围内: %f (误差: %f)\n", actual, fabs(expected - actual)); \
    } \
} while (0)

#define TEST_ASSERT_NULL(pointer) do { \
    total_tests++; \
    if (pointer != NULL) { \
        failed_tests++; \
        printf("❌ 指针不为NULL (行 %d)\n", __LINE__); \
    } else { \
        printf("✓ 指针为NULL\n"); \
    } \
} while (0)

#define TEST_ASSERT_NOT_NULL(pointer) do { \
    total_tests++; \
    if (pointer == NULL) { \
        failed_tests++; \
        printf("❌ 指针为NULL (行 %d)\n", __LINE__); \
    } else { \
        printf("✓ 指针不为NULL\n"); \
    } \
} while (0)

// 测试运行器
#define RUN_TEST(test_func) do { \
    printf("\n运行测试: %s\n", #test_func); \
    test_func(); \
} while (0)

// 测试初始化和清理
void unity_begin(void);
void unity_end(void);

#endif // UNITY_H