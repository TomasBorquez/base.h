#define BASE_IMPLEMENTATION
#include "../base.h"

static size_t total_tests_ran = 0;
static size_t total_tests_failed = 0;

#define TEST_BEGIN(name)  \
  size_t tests_ran = 0;    \
  size_t tests_failed = 0; \
  LogInfo("TESTING: %s", name);

#define TEST_END()           \
  total_tests_ran += tests_ran; \
  total_tests_failed += tests_failed;

#define TEST_ASSERT(cond, msg)         \
  tests_ran++;                          \
  if (!(cond)) {                       \
    tests_failed++;                     \
    LogError("assert failed %s", msg); \
  }

#define TEST_ASSERT_NOT(cond, msg)         \
  tests_ran++;                          \
  if (cond) {                       \
    tests_failed++;                     \
    LogError("assert failed %s", msg); \
  }

static void StartTest(void) {
  LogInfo("========== Running tests ==========");
}

static void EndTest(void) {
  LogInfo("========== Tests complete ==========");
  LogInfo("Total tests run: %zu", total_tests_ran);
  if (total_tests_failed > 1) {
    LogError("%zu tests failed!", total_tests_failed);
    exit(1);
  }

  LogSuccess("All tests passed!");
  exit(0);
}
