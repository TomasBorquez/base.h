#include "test.c"

void TestStringCreation() {
  TEST_BEGIN("String Creation");
  {
    String str1 = S("Hello");
    TEST_ASSERT(str1.length == 5, "S macro length incorrect");
    TEST_ASSERT(str1.data[0] == 'H', "S macro data incorrect");

    char *cstr = "World";
    String str2 = s(cstr);
    TEST_ASSERT(str2.length == 5, "s function length incorrect");
    TEST_ASSERT(str2.data[0] == 'W', "s function data incorrect");

    String empty = S("");
    TEST_ASSERT(empty.length == 0, "Empty string length incorrect");
  }
  TEST_END();
}

void TestStringComparison() {
  TEST_BEGIN("String Comparison");
  {
    String str1 = S("Hello");
    String str2 = S("Hello");
    String str3 = S("World");
    String empty1 = S("");
    String empty2 = S("");

    TEST_ASSERT(StrEq(str1, str2), "Equal strings should return true");
    TEST_ASSERT(!StrEq(str1, str3), "Unequal strings should return false");
    TEST_ASSERT(StrEq(empty1, empty2), "Empty strings should be equal");
    TEST_ASSERT(!StrEq(str1, empty1), "String and empty string should not be equal");
  }
  TEST_END();
}

void TestStringManipulation() {
  TEST_BEGIN("String Manipulation");
  {
    Arena *arena = ArenaCreate(128);

    String str1 = StrNew(arena, "Hello");
    String str2 = StrNew(arena, " World");
    String concatenated = StrConcat(arena, str1, str2);
    TEST_ASSERT(concatenated.length == 11, "Concatenated string length incorrect");
    TEST_ASSERT(concatenated.data[5] == ' ', "Concatenated string data incorrect");
    TEST_ASSERT(concatenated.data[6] == 'W', "Concatenated string data incorrect");

    String destination = StrNewSize(arena, "", 5);
    StrCopy(destination, str1);
    TEST_ASSERT(destination.length == 5, "Copied string length incorrect");
    TEST_ASSERT(StrEq(destination, str1), "Copied string should equal original");

    String uppercase = StrNew(arena, "HELLO");
    StrToLower(uppercase);
    TEST_ASSERT(uppercase.data[0] == 'h', "StrToLower failed to convert first character");
    TEST_ASSERT(uppercase.data[4] == 'o', "StrToLower failed to convert last character");

    String lowercase = StrNew(arena, "hello");
    StrToUpper(lowercase);
    TEST_ASSERT(lowercase.data[0] == 'H', "StrToUpper failed to convert first character");
    TEST_ASSERT(lowercase.data[4] == 'O', "StrToUpper failed to convert last character");
  }
  TEST_END();
}

void TestStringSplitting() {
  TEST_BEGIN("String Splitting");
  {
    Arena *arena = ArenaCreate(128);

    String str = S("hello,world,test");
    String delimiter = S(",");
    StringVector parts = StrSplit(arena, str, delimiter);

    TEST_ASSERT(parts.length == 3, "Split should result in 3 parts");
    TEST_ASSERT(StrEq(parts.data[0], S("hello")), "First part incorrect");
    TEST_ASSERT(StrEq(parts.data[1], S("world")), "Second part incorrect");
    TEST_ASSERT(StrEq(parts.data[2], S("test")), "Third part incorrect");

    String multiline = S("line1\nline2\nline3");
    StringVector lines = StrSplitNewLine(arena, multiline);

    TEST_ASSERT(lines.length == 3, "Split by newline should result in 3 lines");
    TEST_ASSERT(StrEq(lines.data[0], S("line1")), "First line incorrect");
    TEST_ASSERT(StrEq(lines.data[1], S("line2")), "Second line incorrect");
    TEST_ASSERT(StrEq(lines.data[2], S("line3")), "Third line incorrect");
  }
  TEST_END();
}

void TestStringBuilderFunctionality() {
  TEST_BEGIN("StringBuilder Functionality");
  {
    Arena *arena = ArenaCreate(128);

    StringBuilder builder = StringBuilderCreate(arena);
    TEST_ASSERT(builder.capacity > 0, "Builder should have non-zero capacity");
    TEST_ASSERT(builder.buffer.length == 0, "New builder buffer should be empty");

    String str1 = S("Hello");
    StringBuilderAppend(arena, &builder, &str1);
    TEST_ASSERT(builder.buffer.length >= str1.length, "Builder buffer length incorrect after append");

    String str2 = S(" World");
    StringBuilderAppend(arena, &builder, &str2);
    TEST_ASSERT(builder.buffer.length >= str1.length + str2.length, "Builder buffer length incorrect after second append");

    String finalStr = builder.buffer;
    TEST_ASSERT(finalStr.length == str1.length + str2.length, "Final builder string length incorrect");
    TEST_ASSERT(finalStr.data[0] == 'H', "Final builder string data incorrect");
    TEST_ASSERT(finalStr.data[5] == ' ', "Final builder string data incorrect");
    TEST_ASSERT(finalStr.data[6] == 'W', "Final builder string data incorrect");
  }
  TEST_END();
}

void TestStringSlicing() {
  TEST_BEGIN("String Slicing");
  {
    Arena *arena = ArenaCreate(128);

    String str = S("Hello World");

    String slice1 = StrSlice(arena, str, 0, 5); // "Hello"
    TEST_ASSERT(slice1.length == 5, "Slice length incorrect");
    TEST_ASSERT(StrEq(slice1, S("Hello")), "Slice content incorrect");

    String slice2 = StrSlice(arena, str, 6, 11); // "World"
    TEST_ASSERT(slice2.length == 5, "Slice length incorrect");
    TEST_ASSERT(StrEq(slice2, S("World")), "Slice content incorrect");

    String slice3 = StrSlice(arena, str, 0, str.length);
    TEST_ASSERT(slice3.length == str.length, "Full slice length incorrect");
    TEST_ASSERT(StrEq(slice3, str), "Full slice should equal original string");

    String slice4 = StrSlice(arena, str, 5, 5);
    TEST_ASSERT(slice4.length == 0, "Empty slice length should be 0");
    TEST_ASSERT(StrEq(slice4, S("")), "Empty slice should equal empty string");
  }
  TEST_END();
}

void TestPathNormalization() {
  TEST_BEGIN("Path Normalization");
  {
    Arena *arena = ArenaCreate(128);

#if defined(PLATFORM_WIN)
    String path1 = S("./dir1/dir2/dir3");
    String norm1 = NormalizePath(arena, path1);
    TEST_ASSERT(StrEq(norm1, S("dir1\\dir2\\dir3")), "Path separator should be normalized");

    String path2 = S("./dir1/dir2/executable.exe");
    String norm2 = NormalizeExePath(arena, path2);
    TEST_ASSERT(StrEq(norm2, S("dir1\\dir2\\executable.exe")), "Path separator should be normalized");
#else
    String path1 = S("./dir1/dir2/dir3");
    String norm1 = NormalizePath(arena, path1);
    TEST_ASSERT(StrEq(norm1, S("dir1/dir2/dir3")), "Path separator should be normalized");

    String path2 = S("./dir1/dir2/executable.exe");
    String norm2 = NormalizeExePath(arena, path2);
    TEST_ASSERT(StrEq(norm2, S("dir1/dir2/executable")), "Path separator should be normalized");
#endif

    String path3 = S("./dir1/dir2");
    String norm3 = NormalizePathStart(arena, path3);
    TEST_ASSERT(StrEq(norm3, S("dir1/dir2")), "Path separator should be normalized");
  }
  TEST_END();
}

void TestStringVectorFunctionality() {
  TEST_BEGIN("StringVector Functionality");
  {
    StringVector vector = {0};
    StringVectorPushMany(vector, "item1", "item2", "item3");

    TEST_ASSERT(vector.length == 3, "Vector should have 3 items");
    TEST_ASSERT(StrEq(vector.data[0], S("item1")), "First vector item incorrect");
    TEST_ASSERT(StrEq(vector.data[1], S("item2")), "Second vector item incorrect");
    TEST_ASSERT(StrEq(vector.data[2], S("item3")), "Third vector item incorrect");
  }
  TEST_END();
}

void TestStringEdgeCases() {
  TEST_BEGIN("String Edge Cases");
  {
    Arena *arena = ArenaCreate(128);

    String nullStr = {0};
    TEST_ASSERT(StrIsNull(nullStr), "Null string check failed");

    String empty = S("");
    TEST_ASSERT(!StrIsNull(empty), "Empty string should not be null");
    TEST_ASSERT(empty.length == 0, "Empty string length should be 0");

    String whitespace = StrNew(arena, "  hello  ");
    StrTrim(&whitespace);
    TEST_ASSERT(whitespace.length == 5, "Trimmed string length incorrect");
    TEST_ASSERT(StrEq(whitespace, S("hello")), "Trimmed string content incorrect");
  }
  TEST_END();
}

i32 main() {
  StartTest();
  {
    TestStringCreation();
    TestStringComparison();
    TestStringManipulation();
    TestStringSplitting();
    TestStringBuilderFunctionality();
    TestStringSlicing();
    TestPathNormalization();
    TestStringVectorFunctionality();
    TestStringEdgeCases();
  }
  EndTest();
}
