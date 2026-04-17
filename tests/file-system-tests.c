#include "test-framework.c"

static void TestFileOperations(void) {
  TEST_BEGIN("FileOperations");
  {
    TEST_ASSERT(Mkdir(S("file-system-dir")) == SUCCESS, "Should create test directory");
    TEST_ASSERT(SetCwd("file-system-dir") == SUCCESS, "Should set cwd to test directory");

    TEST_ASSERT(Mkdir(S("test-dir")) == SUCCESS, "Should create directory");

    Arena *arena = ArenaCreate(1024);
    ListDirResult files = ListDir(arena, S("."));
    TEST_ASSERT(files.error == SUCCESS, "ListDir should succeed");
    TEST_ASSERT(files.data.length > 0, "Should list at least one entry (test-dir)");

    String content = S("Test content");
    TEST_ASSERT(FileWrite(S("test-file.txt"), content) == SUCCESS, "Should write file successfully");

    FileStatsResult stats = FileStats(S("test-file.txt"));
    TEST_ASSERT(stats.error == SUCCESS, "Should get file stats");
    TEST_ASSERT(stats.data.size == (int64_t)content.length, "File size should match written content size");

    FileReadResult readResult = FileRead(arena, S("test-file.txt"), stats.data.size);
    TEST_ASSERT(readResult.error == SUCCESS, "Should read file successfully");
    TEST_ASSERT(readResult.data.length == content.length, "Read content size should match written content size");

    bool contentMatch = true;
    for (size_t i = 0; i < content.length; i++) {
      if (content.data[i] != readResult.data.data[i]) {
        contentMatch = false;
        break;
      }
    }
    TEST_ASSERT(contentMatch, "Read content should match written content");

    String additional = S("Additional content");
    TEST_ASSERT(FileAdd(S("test-file.txt"), additional) == SUCCESS, "Should add content to file");

    FileStatsResult updatedStats = FileStats(S("test-file.txt"));
    TEST_ASSERT(updatedStats.error == SUCCESS, "Should get updated file stats");
    FileReadResult updatedRead = FileRead(arena, S("test-file.txt"), updatedStats.data.size);
    TEST_ASSERT(updatedRead.error == SUCCESS, "Should read updated file");
    TEST_ASSERT(updatedRead.data.length > content.length, "Updated content should be larger");

    TEST_ASSERT(FileWrite(S("test-file.txt"), S("")) == SUCCESS, "Should reset file");

    FileReadResult resetRead = FileRead(arena, S("test-file.txt"), 0);
    TEST_ASSERT(resetRead.error == SUCCESS, "Should read reset file");
    TEST_ASSERT(resetRead.data.length == 0, "Reset content should be empty");

    TEST_ASSERT(FileRename(S("test-file.txt"), S("renamed-file.txt")) == SUCCESS, "Should rename file");

    FileStatsResult originalStats = FileStats(S("test-file.txt"));
    TEST_ASSERT(originalStats.error == FILE_NOT_FOUND, "Original file should not exist after rename");

    FileStatsResult renamedStats = FileStats(S("renamed-file.txt"));
    TEST_ASSERT(renamedStats.error == SUCCESS, "Renamed file should exist");

    TEST_ASSERT(FileDelete(S("renamed-file.txt")) == SUCCESS, "Should delete file");

    FileStatsResult deletedStats = FileStats(S("renamed-file.txt"));
    TEST_ASSERT(deletedStats.error == FILE_NOT_FOUND, "File should not exist after deletion");

    VecFree(files.data);
    ArenaFree(arena);
  }
  TEST_END();
}

static void TestFileErrorCases(void) {
  TEST_BEGIN("FileErrorCases");
  {
    Arena *arena = ArenaCreate(1024);

    FileReadResult readResult = FileRead(arena, S("non-existent.txt"), 0);
    TEST_ASSERT(readResult.error == FILE_NOT_FOUND, "Should return FILE_NOT_FOUND for non-existent file");

    FileStatsResult statsResult = FileStats(S("non-existent.txt"));
    TEST_ASSERT(statsResult.error == FILE_NOT_FOUND, "Should return FILE_NOT_FOUND for non-existent file stats");

    TEST_ASSERT(FileDelete(S("non-existent.txt")) == FILE_NOT_FOUND, "Should return FILE_NOT_FOUND for deleting non-existent file");

    TEST_ASSERT(FileRename(S("non-existent.txt"), S("new-name.txt")) == FILE_NOT_FOUND, "Should return FILE_NOT_FOUND for renaming non-existent file");

    TEST_ASSERT(FileWrite(S("existing.txt"), S("Existing content")) == SUCCESS, "Should write existing file");
    TEST_ASSERT(FileWrite(S("conflict.txt"), S("Conflict content")) == SUCCESS, "Should write conflict file");

    TEST_ASSERT(FileRename(S("existing.txt"), S("conflict.txt")) == SUCCESS, "Should successfully overwrite when target exists");

    TEST_ASSERT(FileDelete(S("existing.txt")) == FILE_NOT_FOUND, "Should return FILE_NOT_FOUND when deleting moved file");
    TEST_ASSERT(FileDelete(S("conflict.txt")) == SUCCESS, "Should delete conflict file");

    ArenaFree(arena);
  }
  TEST_END();
}

static void TestDirectoryOperations(void) {
  TEST_BEGIN("DirectoryOperations");
  {
    TEST_ASSERT(Mkdir(S("nested")) == SUCCESS, "Should create nested directory");
    TEST_ASSERT(Mkdir(S("nested/dir")) == SUCCESS, "Should create dir in nested directory");
    TEST_ASSERT(Mkdir(S("nested/dir/structure")) == SUCCESS, "Should create structure in nested/dir");

    Arena *arena = ArenaCreate(1024);
    ListDirResult nestedFiles = ListDir(arena, S("nested"));
    TEST_ASSERT(nestedFiles.error == SUCCESS, "ListDir should succeed on nested directory");
    TEST_ASSERT(nestedFiles.data.length > 0, "Should list nested directory contents");

    TEST_ASSERT(FileWrite(S("nested/test-file.txt"), S("Nested file content")) == SUCCESS, "Should write file in nested directory");

    FileStatsResult nestedStats = FileStats(S("nested/test-file.txt"));
    TEST_ASSERT(nestedStats.error == SUCCESS, "Should stat nested file");
    FileReadResult readResult = FileRead(arena, S("nested/test-file.txt"), nestedStats.data.size);
    TEST_ASSERT(readResult.error == SUCCESS, "Should read file from nested directory");

    TEST_ASSERT(FileDelete(S("nested/test-file.txt")) == SUCCESS, "Should delete file in nested directory");

    VecFree(nestedFiles.data);
    ArenaFree(arena);
  }
  TEST_END();
}

static void TestPathHandling(void) {
  TEST_BEGIN("PathHandling");
  {
    TEST_ASSERT(FileWrite(S("./relative-path.txt"), S("Relative path content")) == SUCCESS, "Should handle relative paths with dot");

    TEST_ASSERT(Mkdir(S("backslash")) == SUCCESS, "Should create backslash directory");
    TEST_ASSERT(FileWrite(S("backslash/path.txt"), S("Backslash path content")) == SUCCESS, "Should handle forward slash in path");

    TEST_ASSERT(FileWrite(S(""), S("Empty path content")) != SUCCESS, "Should handle empty path gracefully");

    TEST_ASSERT(FileDelete(S("./relative-path.txt")) == SUCCESS, "Should delete file with relative path");
    TEST_ASSERT(FileDelete(S("backslash/path.txt")) == SUCCESS, "Should delete file with forward slash path");
  }
  TEST_END();
}

static void TestFileSystemEdgeCases(void) {
  TEST_BEGIN("FileSystemEdgeCases");
  {
    TEST_ASSERT(FileWrite(S("empty-file.txt"), S("")) == SUCCESS, "Should write empty file");

    Arena *arena = ArenaCreate(1024);
    FileReadResult emptyRead = FileRead(arena, S("empty-file.txt"), 0);
    TEST_ASSERT(emptyRead.error == SUCCESS, "Should read empty file");
    TEST_ASSERT(emptyRead.data.length == 0, "Should read empty content");

    const size_t largeSize = 1024 * 1024; // 1MB
    char *largeBuffer = (char *)Malloc(largeSize);
    if (largeBuffer) {
      memset(largeBuffer, 'A', largeSize);
      String largeContent = {.length = largeSize, .data = largeBuffer};

      TEST_ASSERT(FileWrite(S("large-file.txt"), largeContent) == SUCCESS, "Should write large file");

      FileStatsResult largeStats = FileStats(S("large-file.txt"));
      TEST_ASSERT(largeStats.error == SUCCESS, "Should get large file stats");
      TEST_ASSERT(largeStats.data.size == largeSize, "Large file size should match");

      TEST_ASSERT(FileDelete(S("large-file.txt")) == SUCCESS, "Should delete large file");
      Free(largeBuffer);
    }

    TEST_ASSERT(FileDelete(S("empty-file.txt")) == SUCCESS, "Should delete empty file");

    ArenaFree(arena);
  }
  TEST_END();
}

static void TestFileCopy(void) {
  TEST_BEGIN("FileCopy");
  {
    Arena *arena = ArenaCreate(1024);

    String sourceContent = S("Source file content for copy test");
    TEST_ASSERT(FileWrite(S("source.txt"), sourceContent) == SUCCESS, "Should write source file successfully");

    TEST_ASSERT(FileCopy(S("source.txt"), S("destination.txt")) == SUCCESS, "Should copy file successfully");

    FileStatsResult destStats = FileStats(S("destination.txt"));
    TEST_ASSERT(destStats.error == SUCCESS, "Should stat destination file");
    FileReadResult destRead = FileRead(arena, S("destination.txt"), destStats.data.size);
    TEST_ASSERT(destRead.error == SUCCESS, "Should read destination file successfully");
    TEST_ASSERT(destRead.data.length == sourceContent.length, "Destination content size should match source");

    bool contentMatch = true;
    for (size_t i = 0; i < sourceContent.length; i++) {
      if (sourceContent.data[i] != destRead.data.data[i]) {
        contentMatch = false;
        break;
      }
    }
    TEST_ASSERT(contentMatch, "Destination content should match source content");

    TEST_ASSERT(FileWrite(S("destination.txt"), S("New content to overwrite")) == SUCCESS, "Should write new content to destination");
    TEST_ASSERT(FileCopy(S("source.txt"), S("destination.txt")) == SUCCESS, "Should overwrite existing file");

    FileStatsResult overwrittenStats = FileStats(S("destination.txt"));
    TEST_ASSERT(overwrittenStats.error == SUCCESS, "Should stat overwritten file");
    FileReadResult overwrittenRead = FileRead(arena, S("destination.txt"), overwrittenStats.data.size);
    TEST_ASSERT(overwrittenRead.error == SUCCESS, "Should read overwritten file");
    TEST_ASSERT(overwrittenRead.data.length == sourceContent.length, "Overwritten content size should match source");

    TEST_ASSERT(Mkdir(S("copy-test-dir")) == SUCCESS, "Should create test subdirectory");
    TEST_ASSERT(FileCopy(S("source.txt"), S("copy-test-dir/copied.txt")) == SUCCESS, "Should copy to subdirectory");

    FileStatsResult subdirStats = FileStats(S("copy-test-dir/copied.txt"));
    TEST_ASSERT(subdirStats.error == SUCCESS, "Should stat subdirectory copy");
    FileReadResult subdirRead = FileRead(arena, S("copy-test-dir/copied.txt"), subdirStats.data.size);
    TEST_ASSERT(subdirRead.error == SUCCESS, "Should read file from subdirectory");
    TEST_ASSERT(subdirRead.data.length == sourceContent.length, "Subdirectory copy content size should match source");

    TEST_ASSERT(FileWrite(S("empty-source.txt"), S("")) == SUCCESS, "Should write empty source file");
    TEST_ASSERT(FileCopy(S("empty-source.txt"), S("empty-dest.txt")) == SUCCESS, "Should copy empty file");

    FileReadResult emptyRead = FileRead(arena, S("empty-dest.txt"), 0);
    TEST_ASSERT(emptyRead.error == SUCCESS, "Should read empty destination file");
    TEST_ASSERT(emptyRead.data.length == 0, "Empty destination content should be empty");

    TEST_ASSERT(FileCopy(S("non-existent.txt"), S("dest.txt")) == FILE_NOT_FOUND, "Should return FILE_NOT_FOUND for non-existent source");
    TEST_ASSERT(FileCopy(S("source.txt"), S("")) != SUCCESS, "Should fail when copying to empty path");

    String files_to_delete[5] = {0};
    files_to_delete[0] = S("source.txt");
    files_to_delete[1] = S("destination.txt");
    files_to_delete[2] = S("copy-test-dir/copied.txt");
    files_to_delete[3] = S("empty-source.txt");
    files_to_delete[4] = S("empty-dest.txt");

    for (size_t i = 0; i < ARR_LEN(files_to_delete); i++) {
      TEST_ASSERT(FileDelete(files_to_delete[i]) == SUCCESS, "Should not fail when deleting files");
    }

    ArenaFree(arena);
  }
  TEST_END();
}

int main(void) {
  StartTest();
  {
    TestFileOperations();
    TestFileErrorCases();
    TestDirectoryOperations();
    TestPathHandling();
    TestFileSystemEdgeCases();
    TestFileCopy();
  }
  EndTest();
}
