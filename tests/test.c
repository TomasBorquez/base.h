#define BASE_IMPLEMENTATION
#include "../base.h"

static size_t totalTestsRun = 0;
static size_t totalTestsFailed = 0;

#define TEST_BEGIN(name)  \
  size_t testsRun = 0;    \
  size_t testsFailed = 0; \
  LogInfo("TESTING: %s", name);

#define TEST_END()           \
  totalTestsRun += testsRun; \
  totalTestsFailed += testsFailed;

#define TEST_ASSERT(cond, msg)         \
  testsRun++;                          \
  if (!(cond)) {                       \
    testsFailed++;                     \
    LogError("assert failed %s", msg); \
  }

void StartTest() {
  LogInfo("========== Running tests ==========");
}

void EndTest() {
  LogInfo("========== Tests complete ==========");
  LogInfo("Total tests run: %zu", totalTestsRun);
  if (totalTestsFailed > 1) {
    LogError("%zu tests failed!", totalTestsFailed);
    exit(1);
  }

  LogSuccess("All tests passed!");
  exit(0);
}
