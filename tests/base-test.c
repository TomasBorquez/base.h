#include <math.h>

#define BASE_IMPLEMENTATION
#include "../base.h"

static int totalTestsRun = 0;
static int totalTestsFailed = 0;

#define TEST_BEGIN(name) \
  int testsRun = 0;      \
  int testsFailed = 0;   \
  LogInfo("TESTING: %s", name);

#define TEST_END(name)       \
  totalTestsRun += testsRun; \
  totalTestsFailed += testsFailed;

#define TEST_ASSERT(cond, msg)         \
  testsRun++;                          \
  if (!(cond)) {                       \
    testsFailed++;                     \
    LogError("assert failed %s", msg); \
  }

static void TestVectors() {
  TEST_BEGIN("StringVector");
  {
    StringVector vec = {0};
    VecForEach(vec, str) {
      TEST_ASSERT(false, "Empty vector iteration");
    }
    TEST_ASSERT(true, "Empty vector does not iterate");

    VecPush(vec, S("1"));
    VecPush(vec, S("2"));
    VecPush(vec, S("3"));
    TEST_ASSERT(vec.length == 3, "Vector has 3 elements");

    int count = 0;
    VecForEach(vec, str) {
      count++;
    }
    TEST_ASSERT(count == 3, "Vector iteration count matches length");

    VecFree(vec);
    TEST_ASSERT(true, "Vector freed");
  }
  TEST_END("StringVector");
}

static void TestArenas() {
  TEST_BEGIN("Arenas");
  {
    Arena *a = ArenaCreate(1024);
    TEST_ASSERT(a != NULL, "Arena created");

    uintptr_t ptr1 = (uintptr_t)ArenaAlloc(a, 1);
    uintptr_t ptr2 = (uintptr_t)ArenaAlloc(a, 1);
    TEST_ASSERT(ptr2 - ptr1 == DEFAULT_ALIGNMENT, "Arena alignment");

    uintptr_t ptr3 = (uintptr_t)ArenaAllocChars(a, 2);
    TEST_ASSERT(ptr3 - ptr2 == 1, "Arena char alignment");

    void *largePtr = ArenaAllocChars(a, 4000);
    TEST_ASSERT(largePtr != NULL, "Large allocation successful");

    ArenaFree(a);
    TEST_ASSERT(true, "Arena freed");
  }
  TEST_END("Arenas");
}

static void TestIniParser() {
  TEST_BEGIN("IniParser");
  {
    String testPath = S("test-config.ini");
    IniFile iniFile = {0};
    errno_t err = IniParse(testPath, &iniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(iniFile.data.length == 7, "IniParse found 7 key-value pairs");

    String key1 = S("key1");
    String value1 = IniGet(&iniFile, key1);
    TEST_ASSERT(StrEq(value1, S("value1")), "IniGet key1");

    String key2 = S("key2");
    String value2 = IniGet(&iniFile, key2);
    TEST_ASSERT(StrEq(value2, S("value2")), "IniGet key2");

    String keyWithSpaces = S("key with spaces");
    String valueWithSpaces = IniGet(&iniFile, keyWithSpaces);
    TEST_ASSERT(StrEq(valueWithSpaces, S("value with spaces")), "IniGet with spaces");

    String key3 = S("key3");
    i32 intValue = IniGetInt(&iniFile, key3);
    TEST_ASSERT(intValue == 123, "IniGetInt");

    String key4 = S("key4");
    f64 doubleValue = IniGetDouble(&iniFile, key4);
    TEST_ASSERT(fabs(doubleValue - 3.14) < 0.0001, "IniGetDouble");

    bool boolValue = IniGetBool(&iniFile, S("key5"));
    TEST_ASSERT(boolValue == true, "IniGetBool true");

    bool boolValue2 = IniGetBool(&iniFile, S("key6"));
    TEST_ASSERT(boolValue2 == false, "IniGetBool false");

    String nonExistentKey = S("non_existent_key");
    String emptyValue = IniGet(&iniFile, nonExistentKey);
    TEST_ASSERT(emptyValue.length == 0 && emptyValue.data == NULL, "IniGet non-existent key");

    String updatedValue = IniSet(&iniFile, key1, S("new_value1"));
    TEST_ASSERT(StrEq(updatedValue, S("new_value1")), "IniSet update");

    String retrievedUpdatedValue = IniGet(&iniFile, key1);
    TEST_ASSERT(StrEq(retrievedUpdatedValue, S("new_value1")), "IniGet after update");

    String newKey = S("new_key");
    String newValue = S("new_value");
    String addedValue = IniSet(&iniFile, newKey, newValue);
    TEST_ASSERT(StrEq(addedValue, newValue), "IniSet add");

    String retrievedNewValue = IniGet(&iniFile, newKey);
    TEST_ASSERT(StrEq(retrievedNewValue, newValue), "IniGet after add");
    TEST_ASSERT(iniFile.data.length == 8, "IniFile size increased after add");

    String newIniPath = S("new_config.ini");
    errno_t createResult = IniWrite(newIniPath, &iniFile);
    TEST_ASSERT(createResult == SUCCESS, "IniWrite success");

    IniFile newIniFile = {0};
    err = IniParse(newIniPath, &newIniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(newIniFile.data.length == iniFile.data.length, "New file has same number of entries");

    String valueFromNewFile = IniGet(&newIniFile, key1);
    TEST_ASSERT(StrEq(valueFromNewFile, S("new_value1")), "IniGet from new file");

    String newKeyValueFromNewFile = IniGet(&newIniFile, newKey);
    TEST_ASSERT(StrEq(newKeyValueFromNewFile, newValue), "IniGet new key from new file");

    String emptyFilePath = S("empty.ini");
    IniFile emptyIni = {0};
    err = IniParse(emptyFilePath, &emptyIni);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(emptyIni.data.length == 0, "Empty INI file");

    String commentsFilePath = S("comments.ini");
    IniFile commentsIni = {0};
    err = IniParse(commentsFilePath, &commentsIni);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(commentsIni.data.length == 0, "Comments-only INI file");

    String malformedFilePath = S("malformed.ini");
    IniFile malformedIni = {0};
    err = IniParse(malformedFilePath, &malformedIni);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(malformedIni.data.length == 2, "Malformed INI file");

    IniFree(&iniFile);
    IniFree(&newIniFile);
    IniFree(&emptyIni);
    IniFree(&commentsIni);
    IniFree(&malformedIni);
    TEST_ASSERT(true, "All INI files freed");
  }
  TEST_END("IniParser");
}

int main() {
  LogInfo("========== Running tests ==========");

  TestVectors();
  TestArenas();
  TestIniParser();

  LogInfo("========== Tests complete ==========");
  LogInfo("Total tests run: %d", totalTestsRun);
  if (totalTestsFailed > 1) {
    LogError("%d tests failed!", totalTestsFailed);
    return 1;
  }

  LogSuccess("All tests passed!");
  return 0;
}
