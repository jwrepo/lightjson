#include "unity.h"

// 测试计数器
int total_tests = 0;
int failed_tests = 0;

// 测试初始化
void unity_begin(void) {
    total_tests = 0;
    failed_tests = 0;
    printf("\n开始运行测试...");
}

// 测试清理和报告
void unity_end(void) {
    printf("\n测试完成。总共运行 %d 个测试，%d 个失败。\n", 
           total_tests, failed_tests);
}