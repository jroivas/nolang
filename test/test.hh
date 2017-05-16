#pragma once

#include <iostream>

static long __failures = 0;
static long __success = 0;
static long __tests = 0;

#define TEST_SUITE(X) \
int main(int argc, char**argv) {\
    (void)argc; (void)argv;\
    X;\
    if (__failures) std::cout << "** FAIL: " << __failures << ", "; \
    std::cout << " PASS: " << __success << ", "; \
    std::cout << " TOTAL: " << __tests << "\n"; \
    return __failures;\
}
#define TEST_CASE(X, Y) {\
    std::cout << "== TEST " << #X << ": "; \
    Y;\
    std::cout << "\n"; \
}
#define TEST_ASSERT_TRUE(X) do { __tests++; if ((X)) { __success++; std::cout << "SUCCESS"; } else { __failures++; std::cout << "FAIL"; } } while (0);
#define TEST_ASSERT_FALSE(X) TEST_ASSERT_TRUE(!X)
