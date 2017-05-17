#pragma once

#include <iostream>
#include <sstream>

static long __success = 0;
static long __suite_success = 0;

#define TEST_MAIN(X) \
int main(int argc, char**argv) {\
    (void)argc; (void)argv;\
    try {\
        X;\
    }\
    catch (std::string e) {\
        std::cout << "-- FAIL: " << e << "\n";\
        return 1;\
    }\
    std::cout << "++ " << __success << " tests PASSED\n"; \
    return 0;\
}
#define TEST_SUITE(X, Y) \
    std::cout << "== SUITE: " << #X << "\n";\
    __suite_success = __success;\
    {\
        Y;\
    }\
    std::cout << "== " << (__success - __suite_success) << " tests PASSED in suite\n";
#define TEST_CASE(X, Y) {\
    std::cout << "   TEST " << #X << "\n"; \
    Y;\
}
#define TEST_ONE_ASSERT bool only_one_assert_per_test_case; (void)(only_one_assert_per_test_case);
#define TEST_ASSERT_TRUE(X) TEST_ONE_ASSERT do { if ((X)) { __success++; } else { std::stringstream s; s << #X; throw s.str(); } } while (0);
#define TEST_ASSERT_FALSE(X) TEST_ASSERT_TRUE(!X)
#define TEST_ASSERT_EQUALS(X, Y) TEST_ONE_ASSERT do { if ((X == Y)) { __success++; } else { std::stringstream s; s << #X << " != " << #Y << ", got " << X; throw s.str(); } } while (0);
