#include <math.h>

#include "test.c"

static void TestIniParseBasic(void) {
  TEST_BEGIN("Ini Parse Basic");
  {
    String testPath = S("./resources/test-config.ini");
    IniFile iniFile = {0};
    errno_t err = IniParse(testPath, &iniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(iniFile.data.length == 7, "IniParse found 7 key-value pairs");
    IniFree(&iniFile);
  }
  TEST_END();
}

static void TestIniGetString(void) {
  TEST_BEGIN("Ini Get String Values");
  {
    String testPath = S("./resources/test-config.ini");
    IniFile iniFile = {0};
    errno_t err = IniParse(testPath, &iniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");

    String key1 = S("key1");
    String value1 = IniGet(&iniFile, key1);
    TEST_ASSERT(StrEq(value1, S("value1")), "IniGet key1");

    String key2 = S("key2");
    String value2 = IniGet(&iniFile, key2);
    TEST_ASSERT(StrEq(value2, S("value2")), "IniGet key2");

    String keyWithSpaces = S("key with spaces");
    String valueWithSpaces = IniGet(&iniFile, keyWithSpaces);
    TEST_ASSERT(StrEq(valueWithSpaces, S("value with spaces")), "IniGet with spaces");

    IniFree(&iniFile);
  }
  TEST_END();
}

static void TestIniGetTypedValues(void) {
  TEST_BEGIN("Ini Get Typed Values");
  {
    String testPath = S("./resources/test-config.ini");
    IniFile iniFile = {0};
    errno_t err = IniParse(testPath, &iniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");

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

    IniFree(&iniFile);
  }
  TEST_END();
}

static void TestIniNonExistentKey(void) {
  TEST_BEGIN("Ini Non-Existent Key");
  {
    String testPath = S("./resources/test-config.ini");
    IniFile iniFile = {0};
    errno_t err = IniParse(testPath, &iniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");

    String nonExistentKey = S("non_existent_key");
    String emptyValue = IniGet(&iniFile, nonExistentKey);
    TEST_ASSERT(emptyValue.length == 0 && emptyValue.data == NULL, "IniGet non-existent key");

    IniFree(&iniFile);
  }
  TEST_END();
}

static void TestIniSetValues(void) {
  TEST_BEGIN("Ini Set Values");
  {
    String testPath = S("./resources/test-config.ini");
    IniFile iniFile = {0};
    errno_t err = IniParse(testPath, &iniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");

    String key1 = S("key1");
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

    IniFree(&iniFile);
  }
  TEST_END();
}

static void TestIniWriteAndRead(void) {
  TEST_BEGIN("Ini Write and Read");
  {
    String testPath = S("./resources/test-config.ini");
    IniFile iniFile = {0};
    errno_t err = IniParse(testPath, &iniFile);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");

    String key1 = S("key1");
    IniSet(&iniFile, key1, S("new_value1"));

    String newKey = S("new_key");
    String newValue = S("new_value");
    IniSet(&iniFile, newKey, newValue);

    String newIniPath = S("./resources/new_config.ini");
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

    IniFree(&iniFile);
    IniFree(&newIniFile);
  }
  TEST_END();
}

static void TestIniSpecialCases(void) {
  TEST_BEGIN("Ini Special Cases");
  {
    String emptyFilePath = S("./resources/empty.ini");
    IniFile emptyIni = {0};
    errno_t err = IniParse(emptyFilePath, &emptyIni);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(emptyIni.data.length == 0, "Empty INI file");

    String commentsFilePath = S("./resources/comments.ini");
    IniFile commentsIni = {0};
    err = IniParse(commentsFilePath, &commentsIni);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(commentsIni.data.length == 0, "Comments-only INI file");

    String malformedFilePath = S("./resources/malformed.ini");
    IniFile malformedIni = {0};
    err = IniParse(malformedFilePath, &malformedIni);
    TEST_ASSERT(err == SUCCESS, "IniParse failed");
    TEST_ASSERT(malformedIni.data.length == 2, "Malformed INI file");

    IniFree(&emptyIni);
    IniFree(&commentsIni);
    IniFree(&malformedIni);
  }
  TEST_END();
}

i32 main(void) {
  StartTest();
  {
    TestIniParseBasic();
    TestIniGetString();
    TestIniGetTypedValues();
    TestIniNonExistentKey();
    TestIniSetValues();
    TestIniWriteAndRead();
    TestIniSpecialCases();
  }
  EndTest();
}
