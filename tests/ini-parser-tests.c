#include "test-framework.c"

#include <math.h>

static void TestIniParseBasic(void) {
  TEST_BEGIN("Ini Parse Basic");
  {
    IniParseResult result = IniParse(S("./resources/test-config.ini"));
    IniFile ini_file = result.data;
    TEST_ASSERT(result.error == SUCCESS, "IniParse failed");
    TEST_ASSERT(ini_file.entries.length == 7, "IniParse found 7 key-value pairs");
    IniFree(&ini_file);
  }
  TEST_END();
}

static void TestIniGetString(void) {
  TEST_BEGIN("Ini Get String Values");
  {
    IniParseResult result = IniParse(S("./resources/test-config.ini"));
    IniFile ini_file = result.data;
    TEST_ASSERT(result.error == SUCCESS, "IniParse failed");

    String value1 = IniGet(&ini_file, S("key1"));
    TEST_ASSERT(StrEq(value1, S("value1")), "IniGet key1");

    String value2 = IniGet(&ini_file, S("key2"));
    TEST_ASSERT(StrEq(value2, S("value2")), "IniGet key2");

    String valueWithSpaces = IniGet(&ini_file, S("key with spaces"));
    TEST_ASSERT(StrEq(valueWithSpaces, S("value with spaces")), "IniGet with spaces");

    IniFree(&ini_file);
  }
  TEST_END();
}

static void TestIniGetTypedValues(void) {
  TEST_BEGIN("Ini Get Typed Values");
  {
    IniParseResult result = IniParse(S("./resources/test-config.ini"));
    IniFile ini_file = result.data;
    TEST_ASSERT(result.error == SUCCESS, "IniParse failed");

    int32_t intValue = IniGetInt(&ini_file, S("key3"));
    TEST_ASSERT(intValue == 123, "IniGetInt");

    float64_t doubleValue = IniGetDouble(&ini_file, S("key4"));
    TEST_ASSERT(fabs(doubleValue - 3.14) < 0.0001, "IniGetDouble");

    bool boolValue = IniGetBool(&ini_file, S("key5"));
    TEST_ASSERT(boolValue == true, "IniGetBool true");

    bool boolValue2 = IniGetBool(&ini_file, S("key6"));
    TEST_ASSERT(boolValue2 == false, "IniGetBool false");

    IniFree(&ini_file);
  }
  TEST_END();
}

static void TestIniNonExistentKey(void) {
  TEST_BEGIN("Ini Non-Existent Key");
  {
    IniParseResult result = IniParse(S("./resources/test-config.ini"));
    IniFile ini_file = result.data;
    TEST_ASSERT(result.error == SUCCESS, "IniParse failed");

    String emptyValue = IniGet(&ini_file, S("non_existent_key"));
    TEST_ASSERT(emptyValue.length == 0 && emptyValue.data == NULL, "IniGet non-existent key");

    IniFree(&ini_file);
  }
  TEST_END();
}

static void TestIniSetValues(void) {
  TEST_BEGIN("Ini Set Values");
  {
    IniParseResult result = IniParse(S("./resources/test-config.ini"));
    IniFile ini_file = result.data;
    TEST_ASSERT(result.error == SUCCESS, "IniParse failed");

    String updatedValue = IniSet(&ini_file, S("key1"), S("new_value1"));
    TEST_ASSERT(StrEq(updatedValue, S("new_value1")), "IniSet update");

    String retrievedUpdatedValue = IniGet(&ini_file, S("key1"));
    TEST_ASSERT(StrEq(retrievedUpdatedValue, S("new_value1")), "IniGet after update");

    String addedValue = IniSet(&ini_file, S("new_key"), S("new_value"));
    TEST_ASSERT(StrEq(addedValue, S("new_value")), "IniSet add");

    String retrievedNewValue = IniGet(&ini_file, S("new_key"));
    TEST_ASSERT(StrEq(retrievedNewValue, S("new_value")), "IniGet after add");
    TEST_ASSERT(ini_file.entries.length == 8, "IniFile size increased after add");

    IniFree(&ini_file);
  }
  TEST_END();
}

static void TestIniWriteAndRead(void) {
  TEST_BEGIN("Ini Write and Read");
  {
    IniParseResult ini_result = IniParse(S("./resources/test-config.ini"));
    IniFile ini_file = ini_result.data;
    TEST_ASSERT(ini_result.error == SUCCESS, "IniParse failed");

    IniSet(&ini_file, S("key1"), S("new_value1"));
    IniSet(&ini_file, S("new_key"), S("new_value"));

    Error create_result = IniWrite(S("./resources/new_config.ini"), &ini_file);
    TEST_ASSERT(create_result == SUCCESS, "IniWrite success");

    IniParseResult new_ini_result = IniParse(S("./resources/new_config.ini"));
    IniFile new_ini_file = new_ini_result.data;
    TEST_ASSERT(new_ini_result.error == SUCCESS, "IniParse failed");
    TEST_ASSERT(new_ini_file.entries.length == ini_file.entries.length, "new file has same number of entries");

    String value_from_new_file = IniGet(&new_ini_file, S("key1"));
    TEST_ASSERT(StrEq(value_from_new_file, S("new_value1")), "IniGet from new file");

    String new_key_value_from_new_file = IniGet(&new_ini_file, S("new_key"));
    TEST_ASSERT(StrEq(new_key_value_from_new_file, S("new_value")), "IniGet new key from new file");

    IniFree(&ini_file);
    IniFree(&new_ini_file);
  }
  TEST_END();
}

static void TestIniSpecialCases(void) {
  TEST_BEGIN("Ini Special Cases");
  {
    IniParseResult ini_result = IniParse(S("./resources/empty.ini"));
    IniFile empty_ini = ini_result.data;
    TEST_ASSERT(ini_result.error == SUCCESS, "IniParse failed");
    TEST_ASSERT(empty_ini.entries.length == 0, "empty INI file");

    ini_result = IniParse(S("./resources/comments.ini"));
    IniFile comments_ini = ini_result.data;
    TEST_ASSERT(ini_result.error == SUCCESS, "IniParse failed");
    TEST_ASSERT(comments_ini.entries.length == 0, "comments only INI file");

    ini_result = IniParse(S("./resources/malformed.ini"));
    IniFile malformed_ini = ini_result.data;
    TEST_ASSERT(ini_result.error == SUCCESS, "IniParse failed");
    TEST_ASSERT(malformed_ini.entries.length == 2, "malformed INI file");

    IniFree(&empty_ini);
    IniFree(&comments_ini);
    IniFree(&malformed_ini);
  }
  TEST_END();
}

int main(void) {
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
