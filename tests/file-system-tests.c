#include "test-framework.c"

static void TestFileOperations(void) {
  TEST_BEGIN("FileOperations");
  {
    TEST_ASSERT(Mkdir(S("file-system-dir")) == SUCCESS, "should create test directory");
    TEST_ASSERT(SetCwd("file-system-dir") == SUCCESS, "should set cwd to test directory");

    TEST_ASSERT(Mkdir(S("test-dir")) == SUCCESS, "should create directory");

    Arena *arena = ArenaCreate(1024);
    ListDirResult files = ListDir(arena, S("."));
    TEST_ASSERT(files.error == SUCCESS, "ListDir should succeed");
    TEST_ASSERT(files.data.length > 0, "should list at least one entry (test-dir)");

    String content = S("Test content");
    TEST_ASSERT(FileWrite(S("test-file.txt"), content) == SUCCESS, "should write file successfully");

    FileStatsResult stats = FileStats(S("test-file.txt"));
    TEST_ASSERT(stats.error == SUCCESS, "should get file stats");
    TEST_ASSERT(stats.data.size == (int64_t)content.length, "file size should match written content size");

    FileReadResult readResult = FileRead(arena, S("test-file.txt"), stats.data.size);
    TEST_ASSERT(readResult.error == SUCCESS, "should read file successfully");
    TEST_ASSERT(readResult.data.length == content.length, "read content size should match written content size");

    bool contentMatch = true;
    for (size_t i = 0; i < content.length; i++) {
      if (content.data[i] != readResult.data.data[i]) {
        contentMatch = false;
        break;
      }
    }
    TEST_ASSERT(contentMatch, "read content should match written content");

    String additional = S("Additional content");
    TEST_ASSERT(FileAdd(S("test-file.txt"), additional) == SUCCESS, "should add content to file");

    FileStatsResult updatedStats = FileStats(S("test-file.txt"));
    TEST_ASSERT(updatedStats.error == SUCCESS, "should get updated file stats");
    FileReadResult updatedRead = FileRead(arena, S("test-file.txt"), updatedStats.data.size);
    TEST_ASSERT(updatedRead.error == SUCCESS, "should read updated file");
    TEST_ASSERT(updatedRead.data.length > content.length, "updated content should be larger");

    TEST_ASSERT(FileWrite(S("test-file.txt"), S("")) == SUCCESS, "should reset file");

    FileReadResult resetRead = FileRead(arena, S("test-file.txt"), 0);
    TEST_ASSERT(resetRead.error == SUCCESS, "should read reset file");
    TEST_ASSERT(resetRead.data.length == 0, "reset content should be empty");

    TEST_ASSERT(FileRename(S("test-file.txt"), S("renamed-file.txt")) == SUCCESS, "should rename file");

    FileStatsResult originalStats = FileStats(S("test-file.txt"));
    TEST_ASSERT(originalStats.error == FILE_NOT_FOUND, "original file should not exist after rename");

    FileStatsResult renamedStats = FileStats(S("renamed-file.txt"));
    TEST_ASSERT(renamedStats.error == SUCCESS, "renamed file should exist");

    TEST_ASSERT(FileDelete(S("renamed-file.txt")) == SUCCESS, "should delete file");

    FileStatsResult deletedStats = FileStats(S("renamed-file.txt"));
    TEST_ASSERT(deletedStats.error == FILE_NOT_FOUND, "file should not exist after deletion");

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
    TEST_ASSERT(readResult.error == FILE_NOT_FOUND, "should return FILE_NOT_FOUND for non-existent file");

    FileStatsResult statsResult = FileStats(S("non-existent.txt"));
    TEST_ASSERT(statsResult.error == FILE_NOT_FOUND, "should return FILE_NOT_FOUND for non-existent file stats");

    TEST_ASSERT(FileDelete(S("non-existent.txt")) == FILE_NOT_FOUND, "should return FILE_NOT_FOUND for deleting non-existent file");

    TEST_ASSERT(FileRename(S("non-existent.txt"), S("new-name.txt")) == FILE_NOT_FOUND, "should return FILE_NOT_FOUND for renaming non-existent file");

    TEST_ASSERT(FileWrite(S("existing.txt"), S("Existing content")) == SUCCESS, "should write existing file");
    TEST_ASSERT(FileWrite(S("conflict.txt"), S("Conflict content")) == SUCCESS, "should write conflict file");

    TEST_ASSERT(FileRename(S("existing.txt"), S("conflict.txt")) == SUCCESS, "should successfully overwrite when target exists");

    TEST_ASSERT(FileDelete(S("existing.txt")) == FILE_NOT_FOUND, "should return FILE_NOT_FOUND when deleting moved file");
    TEST_ASSERT(FileDelete(S("conflict.txt")) == SUCCESS, "should delete conflict file");

    ArenaFree(arena);
  }
  TEST_END();
}

static void TestDirectoryOperations(void) {
  TEST_BEGIN("DirectoryOperations");
  {
    TEST_ASSERT(Mkdir(S("nested")) == SUCCESS, "should create nested directory");
    TEST_ASSERT(Mkdir(S("nested/dir")) == SUCCESS, "should create dir in nested directory");
    TEST_ASSERT(Mkdir(S("nested/dir/structure")) == SUCCESS, "should create structure in nested/dir");

    Arena *arena = ArenaCreate(1024);
    ListDirResult nestedFiles = ListDir(arena, S("nested"));
    TEST_ASSERT(nestedFiles.error == SUCCESS, "ListDir should succeed on nested directory");
    TEST_ASSERT(nestedFiles.data.length > 0, "should list nested directory contents");

    TEST_ASSERT(FileWrite(S("nested/test-file.txt"), S("Nested file content")) == SUCCESS, "should write file in nested directory");

    FileStatsResult nestedStats = FileStats(S("nested/test-file.txt"));
    TEST_ASSERT(nestedStats.error == SUCCESS, "should stat nested file");
    FileReadResult readResult = FileRead(arena, S("nested/test-file.txt"), nestedStats.data.size);
    TEST_ASSERT(readResult.error == SUCCESS, "should read file from nested directory");

    TEST_ASSERT(FileDelete(S("nested/test-file.txt")) == SUCCESS, "should delete file in nested directory");

    VecFree(nestedFiles.data);
    ArenaFree(arena);
  }
  TEST_END();
}

static void TestPathHandling(void) {
  TEST_BEGIN("PathHandling");
  {
    TEST_ASSERT(FileWrite(S("./relative-path.txt"), S("Relative path content")) == SUCCESS, "should handle relative paths with dot");

    TEST_ASSERT(Mkdir(S("backslash")) == SUCCESS, "should create backslash directory");
    TEST_ASSERT(FileWrite(S("backslash/path.txt"), S("Backslash path content")) == SUCCESS, "should handle forward slash in path");

    TEST_ASSERT(FileWrite(S(""), S("Empty path content")) != SUCCESS, "should handle empty path gracefully");

    TEST_ASSERT(FileDelete(S("./relative-path.txt")) == SUCCESS, "should delete file with relative path");
    TEST_ASSERT(FileDelete(S("backslash/path.txt")) == SUCCESS, "should delete file with forward slash path");
  }
  TEST_END();
}

static void TestFileSystemEdgeCases(void) {
  TEST_BEGIN("FileSystemEdgeCases");
  {
    TEST_ASSERT(FileWrite(S("empty-file.txt"), S("")) == SUCCESS, "should write empty file");

    Arena *arena = ArenaCreate(1024);
    FileReadResult emptyRead = FileRead(arena, S("empty-file.txt"), 0);
    TEST_ASSERT(emptyRead.error == SUCCESS, "should read empty file");
    TEST_ASSERT(emptyRead.data.length == 0, "should read empty content");

    const size_t largeSize = 1024 * 1024; // 1MB
    char *largeBuffer = (char *)Malloc(largeSize);
    if (largeBuffer) {
      memset(largeBuffer, 'A', largeSize);
      String largeContent = {.length = largeSize, .data = largeBuffer};

      TEST_ASSERT(FileWrite(S("large-file.txt"), largeContent) == SUCCESS, "should write large file");

      FileStatsResult largeStats = FileStats(S("large-file.txt"));
      TEST_ASSERT(largeStats.error == SUCCESS, "should get large file stats");
      TEST_ASSERT(largeStats.data.size == largeSize, "large file size should match");

      TEST_ASSERT(FileDelete(S("large-file.txt")) == SUCCESS, "should delete large file");
      Free(largeBuffer);
    }

    TEST_ASSERT(FileDelete(S("empty-file.txt")) == SUCCESS, "should delete empty file");

    ArenaFree(arena);
  }
  TEST_END();
}

static void TestFileCopy(void) {
  TEST_BEGIN("FileCopy");
  {
    Arena *arena = ArenaCreate(1024);

    String sourceContent = S("Source file content for copy test");
    TEST_ASSERT(FileWrite(S("source.txt"), sourceContent) == SUCCESS, "should write source file successfully");

    TEST_ASSERT(FileCopy(S("source.txt"), S("destination.txt")) == SUCCESS, "should copy file successfully");

    FileStatsResult destStats = FileStats(S("destination.txt"));
    TEST_ASSERT(destStats.error == SUCCESS, "should stat destination file");
    FileReadResult destRead = FileRead(arena, S("destination.txt"), destStats.data.size);
    TEST_ASSERT(destRead.error == SUCCESS, "should read destination file successfully");
    TEST_ASSERT(destRead.data.length == sourceContent.length, "destination content size should match source");

    bool contentMatch = true;
    for (size_t i = 0; i < sourceContent.length; i++) {
      if (sourceContent.data[i] != destRead.data.data[i]) {
        contentMatch = false;
        break;
      }
    }
    TEST_ASSERT(contentMatch, "Destination content should match source content");

    TEST_ASSERT(FileWrite(S("destination.txt"), S("New content to overwrite")) == SUCCESS, "should write new content to destination");
    TEST_ASSERT(FileCopy(S("source.txt"), S("destination.txt")) == SUCCESS, "should overwrite existing file");

    FileStatsResult overwrittenStats = FileStats(S("destination.txt"));
    TEST_ASSERT(overwrittenStats.error == SUCCESS, "should stat overwritten file");
    FileReadResult overwrittenRead = FileRead(arena, S("destination.txt"), overwrittenStats.data.size);
    TEST_ASSERT(overwrittenRead.error == SUCCESS, "should read overwritten file");
    TEST_ASSERT(overwrittenRead.data.length == sourceContent.length, "Overwritten content size should match source");

    TEST_ASSERT(Mkdir(S("copy-test-dir")) == SUCCESS, "should create test subdirectory");
    TEST_ASSERT(FileCopy(S("source.txt"), S("copy-test-dir/copied.txt")) == SUCCESS, "should copy to subdirectory");

    FileStatsResult subdirStats = FileStats(S("copy-test-dir/copied.txt"));
    TEST_ASSERT(subdirStats.error == SUCCESS, "should stat subdirectory copy");
    FileReadResult subdirRead = FileRead(arena, S("copy-test-dir/copied.txt"), subdirStats.data.size);
    TEST_ASSERT(subdirRead.error == SUCCESS, "should read file from subdirectory");
    TEST_ASSERT(subdirRead.data.length == sourceContent.length, "Subdirectory copy content size should match source");

    TEST_ASSERT(FileWrite(S("empty-source.txt"), S("")) == SUCCESS, "should write empty source file");
    TEST_ASSERT(FileCopy(S("empty-source.txt"), S("empty-dest.txt")) == SUCCESS, "should copy empty file");

    FileReadResult emptyRead = FileRead(arena, S("empty-dest.txt"), 0);
    TEST_ASSERT(emptyRead.error == SUCCESS, "should read empty destination file");
    TEST_ASSERT(emptyRead.data.length == 0, "Empty destination content should be empty");

    TEST_ASSERT(FileCopy(S("non-existent.txt"), S("dest.txt")) == FILE_NOT_FOUND, "should return FILE_NOT_FOUND for non-existent source");
    TEST_ASSERT(FileCopy(S("source.txt"), S("")) != SUCCESS, "should fail when copying to empty path");

    String files_to_delete[5] = {0};
    files_to_delete[0] = S("source.txt");
    files_to_delete[1] = S("destination.txt");
    files_to_delete[2] = S("copy-test-dir/copied.txt");
    files_to_delete[3] = S("empty-source.txt");
    files_to_delete[4] = S("empty-dest.txt");

    for (size_t i = 0; i < ARR_LEN(files_to_delete); i++) {
      TEST_ASSERT(FileDelete(files_to_delete[i]) == SUCCESS, "should not fail when deleting files");
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
