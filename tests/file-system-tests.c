#include "test.c"

void TestFileOperations() {
  TEST_BEGIN("FileOperations");
  {
    bool mkdirResult = Mkdir(S("file-system-tests"));
    TEST_ASSERT(mkdirResult == true, "Should create test directory");

    SetCwd("file-system-tests");

    bool dirResult = Mkdir(S("test_dir"));
    TEST_ASSERT(dirResult == true, "Should create directory");

    Arena *arena = ArenaCreate(1024);
    StringVector files = ListDir(arena, S("."));
    TEST_ASSERT(files.length > 0, "Should list at least one entry (test_dir)");

    String content = S("Test content");
    errno_t writeResult = FileWrite(S("test_file.txt"), content);
    TEST_ASSERT(writeResult == SUCCESS, "Should write file successfully");

    File fileStats;
    errno_t statsResult = FileStats(S("test_file.txt"), &fileStats);
    TEST_ASSERT(statsResult == SUCCESS, "Should get file stats");
    TEST_ASSERT(fileStats.size == (i64)content.length, "File size should match written content size");

    String readContent;
    errno_t readResult = FileRead(arena, S("test_file.txt"), &readContent);
    TEST_ASSERT(readResult == SUCCESS, "Should read file successfully");
    TEST_ASSERT(readContent.length == content.length, "Read content size should match written content size");

    bool contentMatch = true;
    for (size_t i = 0; i < content.length; i++) {
      if (content.data[i] != readContent.data[i]) {
        contentMatch = false;
        break;
      }
    }
    TEST_ASSERT(contentMatch, "Read content should match written content");

    String additional = S("Additional content");
    errno_t addResult = FileAdd(S("test_file.txt"), additional);
    TEST_ASSERT(addResult == SUCCESS, "Should add content to file");

    String updatedContent;
    errno_t updatedReadResult = FileRead(arena, S("test_file.txt"), &updatedContent);
    TEST_ASSERT(updatedReadResult == SUCCESS, "Should read updated file");
    TEST_ASSERT(updatedContent.length > content.length, "Updated content should be larger");

    errno_t resetResult = FileReset(S("test_file.txt"));
    TEST_ASSERT(resetResult == SUCCESS, "Should reset file");

    String resetContent;
    errno_t resetReadResult = FileRead(arena, S("test_file.txt"), &resetContent);
    TEST_ASSERT(resetReadResult == SUCCESS, "Should read reset file");
    TEST_ASSERT(resetContent.length == 0, "Reset content should be empty");

    errno_t renameResult = FileRename(S("test_file.txt"), S("renamed_file.txt"));
    TEST_ASSERT(renameResult == SUCCESS, "Should rename file");

    File originalStats;
    errno_t originalStatsResult = FileStats(S("test_file.txt"), &originalStats);
    TEST_ASSERT(originalStatsResult == FILE_STATS_FILE_NOT_EXIST, "Original file should not exist after rename");

    File renamedStats;
    errno_t renamedStatsResult = FileStats(S("renamed_file.txt"), &renamedStats);
    TEST_ASSERT(renamedStatsResult == SUCCESS, "Renamed file should exist");

    errno_t deleteResult = FileDelete(S("renamed_file.txt"));
    TEST_ASSERT(deleteResult == SUCCESS, "Should delete file");

    File deletedStats;
    errno_t deletedStatsResult = FileStats(S("renamed_file.txt"), &deletedStats);
    TEST_ASSERT(deletedStatsResult == FILE_STATS_FILE_NOT_EXIST, "File should not exist after deletion");

    ArenaFree(arena);
  }
  TEST_END();
}

void TestFileErrorCases() {
  TEST_BEGIN("FileErrorCases");
  {
    Arena *arena = ArenaCreate(1024);

    String content;
    errno_t readResult = FileRead(arena, S("non_existent.txt"), &content);
    TEST_ASSERT(readResult == FILE_NOT_EXIST, "Should return FILE_NOT_EXIST for non-existent file");

    File fileStats;
    errno_t statsResult = FileStats(S("non_existent.txt"), &fileStats);
    TEST_ASSERT(statsResult == FILE_STATS_FILE_NOT_EXIST, "Should return FILE_STATS_FILE_NOT_EXIST for non-existent file");

    errno_t deleteResult = FileDelete(S("non_existent.txt"));
    TEST_ASSERT(deleteResult == FILE_DELETE_NOT_FOUND, "Should return FILE_DELETE_NOT_FOUND for non-existent file");

    errno_t renameResult = FileRename(S("non_existent.txt"), S("new_name.txt"));
    TEST_ASSERT(renameResult == FILE_RENAME_NOT_FOUND, "Should return FILE_RENAME_NOT_FOUND for non-existent file");

    errno_t writeExist = FileWrite(S("existing.txt"), S("Existing content"));
    TEST_ASSERT(writeExist == SUCCESS, "Should write existing file");

    errno_t writeConflict = FileWrite(S("conflict.txt"), S("Conflict content"));
    TEST_ASSERT(writeConflict == SUCCESS, "Should write conflict file");

    errno_t renameConflict = FileRename(S("existing.txt"), S("conflict.txt"));
    TEST_ASSERT(renameConflict == SUCCESS, "Should successfully overwrite when target exists");

    errno_t deleteExist = FileDelete(S("existing.txt"));
    TEST_ASSERT(deleteExist == FILE_DELETE_NOT_FOUND, "Should return not found when deleting moved file");

    errno_t deleteConflict = FileDelete(S("conflict.txt"));
    TEST_ASSERT(deleteConflict == SUCCESS, "Should delete conflict file");

    ArenaFree(arena);
  }
  TEST_END();
}

void TestDirectoryOperations() {
  TEST_BEGIN("DirectoryOperations");
  {
    bool mkdirNested = Mkdir(S("nested"));
    TEST_ASSERT(mkdirNested == true, "Should create nested directory");

    bool mkdirDir = Mkdir(S("nested/dir"));
    TEST_ASSERT(mkdirDir == true, "Should create dir in nested directory");

    bool mkdirStructure = Mkdir(S("nested/dir/structure"));
    TEST_ASSERT(mkdirStructure == true, "Should create structure in nested/dir");

    Arena *arena = ArenaCreate(1024);
    StringVector nestedFiles = ListDir(arena, S("nested"));
    TEST_ASSERT(nestedFiles.length > 0, "Should list nested directory contents");

    errno_t writeResult = FileWrite(S("nested/test_file.txt"), S("Nested file content"));
    TEST_ASSERT(writeResult == SUCCESS, "Should write file in nested directory");

    String readContent;
    errno_t readResult = FileRead(arena, S("nested/test_file.txt"), &readContent);
    TEST_ASSERT(readResult == SUCCESS, "Should read file from nested directory");

    errno_t deleteResult = FileDelete(S("nested/test_file.txt"));
    TEST_ASSERT(deleteResult == SUCCESS, "Should delete file in nested directory");

    ArenaFree(arena);
  }
  TEST_END();
}

void TestPathHandling() {
  TEST_BEGIN("PathHandling");
  {
    errno_t writeResult = FileWrite(S("./relative_path.txt"), S("Relative path content"));
    TEST_ASSERT(writeResult == SUCCESS, "Should handle relative paths with dot");

    bool mkdirBackslash = Mkdir(S("backslash"));
    TEST_ASSERT(mkdirBackslash == true, "Should create backslash directory");

    errno_t writeBackslash = FileWrite(S("backslash/path.txt"), S("Backslash path content"));
    TEST_ASSERT(writeBackslash == SUCCESS, "Should handle forward slash in path");

    errno_t writeEmpty = FileWrite(S(""), S("Empty path content"));
    TEST_ASSERT(writeEmpty != SUCCESS, "Should handle empty path gracefully");

    errno_t deleteRelative = FileDelete(S("./relative_path.txt"));
    TEST_ASSERT(deleteRelative == SUCCESS, "Should delete file with relative path");

    errno_t deleteBackslash = FileDelete(S("backslash/path.txt"));
    TEST_ASSERT(deleteBackslash == SUCCESS, "Should delete file with forward slash path");
  }
  TEST_END();
}

void TestFileSystemEdgeCases() {
  TEST_BEGIN("FileSystemEdgeCases");
  {
    errno_t writeEmpty = FileWrite(S("empty_file.txt"), S(""));
    TEST_ASSERT(writeEmpty == SUCCESS, "Should write empty file");

    Arena *arena = ArenaCreate(1024);
    String content;
    errno_t readEmpty = FileRead(arena, S("empty_file.txt"), &content);
    TEST_ASSERT(readEmpty == SUCCESS, "Should read empty file");
    TEST_ASSERT(content.length == 0, "Should read empty content");

    const size_t largeSize = 1024 * 1024; // 1MB
    char *largeBuffer = (char *)Malloc(largeSize);
    if (largeBuffer) {
      memset(largeBuffer, 'A', largeSize);
      String largeContent = {.length = largeSize, .data = largeBuffer};

      errno_t writeLarge = FileWrite(S("large_file.txt"), largeContent);
      TEST_ASSERT(writeLarge == SUCCESS, "Should write large file");

      File largeStats;
      errno_t largeStatsResult = FileStats(S("large_file.txt"), &largeStats);
      TEST_ASSERT(largeStatsResult == SUCCESS, "Should get large file stats");
      TEST_ASSERT(largeStats.size == (i64)largeSize, "Large file size should match");

      errno_t deleteLarge = FileDelete(S("large_file.txt"));
      TEST_ASSERT(deleteLarge == SUCCESS, "Should delete large file");
      Free(largeBuffer);
    }

    errno_t deleteEmpty = FileDelete(S("empty_file.txt"));
    TEST_ASSERT(deleteEmpty == SUCCESS, "Should delete empty file");

    ArenaFree(arena);
  }
  TEST_END();
}

i32 main() {
  StartTest();
  {
    TestFileOperations();
    TestFileErrorCases();
    TestDirectoryOperations();
    TestPathHandling();
    TestFileSystemEdgeCases();
  }
  EndTest();
  return 0;
}
