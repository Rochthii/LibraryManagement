#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <sstream>

// Simple test framework - tu viet de khong phai dung thu vien ngoai
class TestFramework {
private:
    static int totalTests;
    static int passedTests;
    static int failedTests;
    static std::string currentTestName;

public:
    static void startTest(const std::string& name) {
        currentTestName = name;
        std::cout << "\n[TEST] " << name << std::endl;
    }

    static void assert_true(bool condition, const std::string& message) {
        totalTests++;
        if (condition) {
            passedTests++;
            std::cout << "  ✓ PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "  ✗ FAIL: " << message << std::endl;
        }
    }

    static void assert_equal(int expected, int actual, const std::string& message) {
        totalTests++;
        if (expected == actual) {
            passedTests++;
            std::cout << "  ✓ PASS: " << message << " (expected: " << expected << ", got: " << actual << ")" << std::endl;
        } else {
            failedTests++;
            std::cout << "  ✗ FAIL: " << message << " (expected: " << expected << ", got: " << actual << ")" << std::endl;
        }
    }

    static void assert_equal(const std::string& expected, const std::string& actual, const std::string& message) {
        totalTests++;
        if (expected == actual) {
            passedTests++;
            std::cout << "  ✓ PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "  ✗ FAIL: " << message << std::endl;
            std::cout << "    Expected: \"" << expected << "\"" << std::endl;
            std::cout << "    Got:      \"" << actual << "\"" << std::endl;
        }
    }

    static void assert_not_null(void* ptr, const std::string& message) {
        totalTests++;
        if (ptr != nullptr) {
            passedTests++;
            std::cout << "  ✓ PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "  ✗ FAIL: " << message << " (pointer is null)" << std::endl;
        }
    }

    static void assert_null(void* ptr, const std::string& message) {
        totalTests++;
        if (ptr == nullptr) {
            passedTests++;
            std::cout << "  ✓ PASS: " << message << std::endl;
        } else {
            failedTests++;
            std::cout << "  ✗ FAIL: " << message << " (pointer is not null)" << std::endl;
        }
    }

    static void printSummary() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Total:  " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << " (" << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%)" << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "========================================" << std::endl;
        
        if (failedTests == 0) {
            std::cout << "✅ ALL TESTS PASSED!" << std::endl;
        } else {
            std::cout << "❌ SOME TESTS FAILED!" << std::endl;
        }
    }

    static void reset() {
        totalTests = 0;
        passedTests = 0;
        failedTests = 0;
        currentTestName = "";
    }

    static int getFailedCount() {
        return failedTests;
    }
};

// Initialize static members
int TestFramework::totalTests = 0;
int TestFramework::passedTests = 0;
int TestFramework::failedTests = 0;
std::string TestFramework::currentTestName = "";

#endif // TEST_FRAMEWORK_H
