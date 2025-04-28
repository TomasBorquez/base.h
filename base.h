/* MIT License

  base.h - Better cross-platform std
  Version - 2025-04-28 (0.1.5):
  https://github.com/TomasBorquez/base.h

  Usage:
    #define BASE_IMPLEMENTATION
    #include "base.h"

  More on the the `README.md`
*/
#define BASE_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

/* --- Platform MACROS and includes --- */
#if defined(__clang__)
#define COMPILER_CLANG
#elif defined(_MSC_VER)
#define COMPILER_MSVC
#elif defined(__GNUC__)
#define COMPILER_GCC
#else
#error "The codebase only supports Clang, MSVC and GCC, TCC soon"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WIN
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX
#else
#error "The codebase only supports windows and linux, macos soon"
#endif

#ifdef COMPILER_CLANG
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#ifdef PLATFORM_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(PLATFORM_LINUX)
#include <unistd.h>
#include <linux/limits.h>
#endif

#if defined(__STDC_VERSION__)
#if (__STDC_VERSION__ >= 202311L)
#define C_STANDARD_C23
#define C_STANDARD "C23"
#elif (__STDC_VERSION__ >= 201710L)
#define C_STANDARD_C17
#define C_STANDARD "C17"
#elif (__STDC_VERSION__ >= 201112L)
#define C_STANDARD_C11
#define C_STANDARD "C11"
#elif (__STDC_VERSION__ >= 199901L)
#define C_STANDARD_C99
#define C_STANDARD "C99"
#else
#error "you are cooked" // ???
#endif
#endif

#if defined(COMPILER_MSVC)
#if _MSVC_LANG >= 202000L
#define C_STANDARD_C23
#define C_STANDARD "C23"
#elif _MSVC_LANG >= 201704L
#define C_STANDARD_C17
#define C_STANDARD "C17"
#elif _MSVC_LANG >= 201103L
#define C_STANDARD_C11
#define C_STANDARD "C11"
#else
#error "you are cooked" // ???
#endif
#endif

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* --- Platform Specific --- */
#ifdef PLATFORM_WIN
/* Process functions */
#define popen _popen
#define pclose _pclose

/* File I/O functions */
#define fileno _fileno
#define fdopen _fdopen
#define access _access
#define unlink _unlink
#define isatty _isatty
#define dup _dup
#define dup2 _dup2
#define ftruncate _chsize
#define fsync _commit

/* Directory functions */
#define mkdir(path, mode) _mkdir(path)
#define rmdir _rmdir
#define getcwd _getcwd
#define chdir _chdir

/* Process/Threading */
#define getpid _getpid
#define execvp _execvp
#define execve _execve
#define sleep(x) Sleep((x) * 1000)
#define usleep(x) Sleep((x) / 1000)

/* String functions */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define strdup _strdup

/* File modes */
#define R_OK 4
#define W_OK 2
#define X_OK 0 /* Windows doesn't have explicit X_OK */
#define F_OK 0

/* File descriptors */
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* Some functions need complete replacements */
#ifdef COMPILER_MSVC
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#endif

/* --- Types and MACRO types --- */
// Unsigned int types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed int types
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// Regular int types
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Floating point types
typedef float f32;
typedef double f64;

typedef struct {
  size_t length; // Does not include null terminator
  char *data;
} String;

// Maximum values for integer types
#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define S8_MAX INT8_MAX
#define S8_MIN INT8_MIN
#define S16_MAX INT16_MAX
#define S16_MIN INT16_MIN
#define S32_MAX INT32_MAX
#define S32_MIN INT32_MIN
#define S64_MAX INT64_MAX
#define S64_MIN INT64_MIN

#define I8_MAX INT8_MAX
#define I8_MIN INT8_MIN
#define I16_MAX INT16_MAX
#define I16_MIN INT16_MIN
#define I32_MAX INT32_MAX
#define I32_MIN INT32_MIN
#define I64_MAX INT64_MAX
#define I64_MIN INT64_MIN

#define TYPE_INIT(type) (type)

/* --- Vector Macros --- */
// TODO: Add MSVC like vector macros
#define VEC_TYPE(typeName, valueType) \
  typedef struct { \
    valueType *data; \
    i32 length; \
    i32 capacity; \
  } typeName;

#define VecTypeof(vector) __typeof__(*vector.data)

#define VecPush(vec, value) \
  ({ \
    if (vec.length >= vec.capacity) { \
      if (vec.capacity == 0) vec.capacity = 128; \
      else vec.capacity *= 2; \
      vec.data = (VecTypeof(vec)*)realloc(vec.data, vec.capacity * sizeof(*vec.data)); \
    } \
    vec.data[vec.length++] = value; \
    &vec.data[vec.length - 1]; \
  })

#define VecPop(vector) \
  ({ \
    assert(vector.length > 0 && "Cannot pop from empty vector"); \
    VecTypeof(vector) value = vector.data[vector.length - 1]; \
    vector.length--; \
    &value; \
  })

#define VecShift(vector) \
  ({ \
    assert(vector.length != 0 && "Length should at least be >= 1"); \
    VecTypeof(vector) value = vector.data[0]; \
    memmove(&vector.data[0], &vector.data[1], (vector.length - 1) * sizeof(*vector.data)); \
    vector.length--; \
    &value; \
  })

#define VecShift(vector) \
  ({ \
    assert(vector.length != 0 && "Length should at least be >= 1"); \
    VecTypeof(vector) value = vector.data[0]; \
    memmove(&vector.data[0], &vector.data[1], (vector.length - 1) * sizeof(*vector.data)); \
    vector.length--; \
    &value; \
  })

#define VecForEach(vec, it) \
  for (VecTypeof(vec)* it = vec.data; vec.data && (it != vec.data + vec.length); ++it)

#define VecUnshift(vector, value) \
  ({ \
    if (vector.length >= vector.capacity) { \
      if (vector.capacity == 0) vector.capacity = 2; \
      else vector.capacity *= 2; \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data)); \
    } \
    \
    if (vector.length > 0) { \
      memmove(&vector.data[1], &vector.data[0], vector.length * sizeof(*vector.data)); \
    } \
    \
    vector.data[0] = value; \
    vector.length++; \
    &value; \
  })

#define VecInsert(vector, value, index) \
  ({ \
    assert(index <= vector.length && "Index out of bounds for insertion"); \
    if (vector.length >= vector.capacity) { \
      if (vector.capacity == 0) vector.capacity = 2; \
      else vector.capacity *= 2; \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data)); \
    } \
    memmove(&vector.data[index + 1], &vector.data[index], (vector.length - index) * sizeof(*vector.data)); \
    vector.data[index] = value; \
    vector.length++; \
    &value; \
  })

#define VecAt(vector, index) \
  ({ \
    assert(index >= 0 && index < vector.length && "Index out of bounds"); \
    &vector.data[index]; \
  })

#define VecFree(vector) \
  ({ \
    assert(vector.data != NULL && "Vector data should never be NULL"); \
    free(vector.data); \
    vector.data = NULL; \
  })

/* --- Time and Platforms --- */
i64 TimeNow();
void WaitTime(i64 ms);
String GetCompiler();
String GetPlatform();

/* --- Errors --- */
typedef i32 errno_t;

enum GeneralError {
  SUCCESS,
  MEMORY_ALLOCATION_FAILED,
};

/* --- Arena --- */
typedef struct {
  int8_t *buffer;
  size_t capacity;
  size_t offset;
} Arena;

// This makes sure right alignment on 86/64 bits
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

void *ArenaAllocAligned(Arena *arena, size_t size, size_t align);
void *ArenaAlloc(Arena *arena, size_t size);
bool ArenaReserve(Arena *arena, size_t cap);
char *ArenaAllocChars(Arena *arena, size_t count);
void ArenaFree(Arena *arena);
void ArenaReset(Arena *arena);

/* --- String and Macros --- */
#define STRING_LENGTH(s) ((sizeof(s) / sizeof((s)[0])) - sizeof((s)[0])) // NOTE: Inspired from clay.h
#define ENSURE_STRING_LITERAL(x) ("" x "")

// NOTE: If an error led you here, it's because `S` can only be used with string literals, i.e. `S("SomeString")` and not `S(yourString)` - for that use `s()`
#define S(string) (TYPE_INIT(String){.length = STRING_LENGTH(ENSURE_STRING_LITERAL(string)), .data = (string)})
String s(char *msg);

#ifdef COMPILER_CLANG
#define FORMAT_CHECK(fmt_pos, args_pos) __attribute__((format(printf, fmt_pos, args_pos))) // NOTE: Printf like warnings on format
#else
#define FORMAT_CHECK(fmt_pos, args_pos)
#endif

String F(Arena *arena, const char *format, ...) FORMAT_CHECK(2, 3);

VEC_TYPE(StringVector, String);
#define StringVectorPushMany(vector, ...) \
  ({ \
    char *values[] = {__VA_ARGS__}; \
    size_t count = sizeof(values) / sizeof(values[0]); \
    for (size_t i = 0; i < count; i++) { \
      VecPush(vector, s(values[i])); \
    } \
  })

void SetMaxStrSize(size_t size);
String StrNew(Arena *arena, char *str);
String StrNewSize(Arena *arena, char *str, size_t len); // Without null terminator
void StrCopy(String *destination, String *source);
StringVector StrSplit(Arena *arena, String *string, String *delimiter);
bool StrEqual(String *string1, String *string2);
String StrConcat(Arena *arena, String *string1, String *string2);
void StrToUpper(String *string1);
void StrToLower(String *string1);
bool StrIsNull(String *string);
void StrTrim(String *string);
String StrSlice(Arena *arena, String *str, i32 start, i32 end);
String ConvertExe(Arena *arena, String path);
String ConvertPath(Arena *arena, String path);

/* --- Random --- */
void RandomInit(); // NOTE: Must init before using
u64 RandomGetSeed();
void RandomSetSeed(u64 newSeed);
i32 RandomInteger(i32 min, i32 max);
f32 RandomFloat(f32 min, f32 max);

/* --- File --- */
#define MAX_FILES 200

typedef struct {
  char *name;
  char *extension;
  int64_t size;
  int64_t createTime;
  int64_t modifyTime;
} File;

typedef struct {
  char *name;
} Folder;

typedef struct {
  Folder *folders;
  size_t folderCount;

  File *files;
  size_t fileCount;

  size_t totalCount;
} FileData;

char *GetCwd();
void SetCwd(char *destination);
FileData *GetDirFiles();
FileData *NewFileData();

enum FileStatsError { FILE_GET_ATTRIBUTES_FAILED = 1 };
errno_t FileStats(String *path, File *file);

enum FileReadError { FILE_NOT_EXIST = 1, FILE_OPEN_FAILED, FILE_GET_SIZE_FAILED, FILE_READ_FAILED };
errno_t FileRead(Arena *arena, String *path, String *result);

// TODO: enum FileWriteError {};
errno_t FileWrite(String *path, String *data);

// TODO: enum FileDeleteError {};
errno_t FileDelete(String *path);

// TODO: enum FileRenameError {};
errno_t FileRename(String *oldPath, String *newPath);

bool Mkdir(String path); // NOTE: Mkdir if not exist

/* --- Logger --- */
#define RESET "\x1b[0m"
#define GRAY "\x1b[38;2;192;192;192m"
#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define ORANGE "\x1b[0;33m"

void LogInfo(const char *format, ...) FORMAT_CHECK(1, 2);
void LogWarn(const char *format, ...) FORMAT_CHECK(1, 2);
void LogError(const char *format, ...) FORMAT_CHECK(1, 2);
void LogSuccess(const char *format, ...) FORMAT_CHECK(1, 2);
void LogInit();

/* --- Defer Macros --- */
#ifdef DEFER_MACRO // NOTE: Optional since not all compilers support it and not all C versions do either

/* - GCC implementation -
  NOTE: Must use C23 (depending on the platform)
*/
#ifdef COMPILER_GCC
#define defer __DEFER(__COUNTER__)
#define __DEFER(N) __DEFER_(N)
#define __DEFER_(N) __DEFER__(__DEFER_FUNCTION_##N, __DEFER_VARIABLE_##N)
#define __DEFER__(F, V) \
  auto void F(int *); \
  [[gnu::cleanup(F)]] int V; \
  auto void F(int *)

/* - Clang implementation -
  NOTE: Must compile with flag `-fblocks`
*/
#elif defined(COMPILER_CLANG)
typedef void (^const __df_t)(void);

[[maybe_unused]]
static inline void __df_cb(__df_t *__fp) {
  (*__fp)();
}

#define defer __DEFER(__COUNTER__)
#define __DEFER(N) __DEFER_(N)
#define __DEFER_(N) __DEFER__(__DEFER_VARIABLE_##N)
#define __DEFER__(V) [[gnu::cleanup(__df_cb)]] __df_t V = ^void(void)

/* -- MSVC implementation --
  NOTE: Not available yet in MSVC, use `_try/_finally`
*/
#elif defined(COMPILER_MSVC)
#error "Not available yet in MSVC, use `_try/_finally`"
#endif

#endif

/*
  Implementation of base.h
  Version - 2025-04-28 (0.1.5):
  https://github.com/TomasBorquez/base.h
*/
#ifdef BASE_IMPLEMENTATION

// --- Platform specific functions ---
#if !defined(PLATFORM_WIN) && !defined(C_STANDARD_C11)
#ifndef EINVAL
#define EINVAL 22 // NOTE: Invalid argument
#endif
#ifndef ERANGE
#define ERANGE 34 // NOTE: Result too large
#endif

errno_t memcpy_s(void *dest, size_t destSize, const void *src, size_t count) {
  if (dest == NULL) {
    return EINVAL;
  }

  if (src == NULL || destSize < count) {
    memset(dest, 0, destSize);
    return EINVAL;
  }

  memcpy(dest, src, count);
  return 0;
}

inline errno_t fopen_s(FILE **streamptr, const char *filename, const char *mode) {
  if (streamptr == NULL) {
    return EINVAL;
  }

  *streamptr = NULL;
  if (filename == NULL || mode == NULL) {
    return EINVAL;
  }

  if (*filename == '\0') {
    return EINVAL;
  }

  const char *valid_modes = "rwa+btx";
  size_t mode_len = strlen(mode);

  if (mode_len == 0) {
    return EINVAL;
  }

  for (size_t i = 0; i < mode_len; i++) {
    if (strchr(valid_modes, mode[i]) == NULL) {
      return EINVAL;
    }
  }

  *streamptr = fopen(filename, mode);
  if (*streamptr == NULL) {
    return errno;
  }

  return 0;
}
#endif

String GetCompiler() {
#if defined(COMPILER_CLANG)
  return S("clang");
#elif defined(COMPILER_GCC)
  return S("gcc");
#elif defined(COMPILER_MSVC)
  return S("MSVC");
#endif
}

String GetPlatform() {
#if defined(PLATFORM_WIN)
  return S("windows");
#elif defined(PLATFORM_LINUX)
  return S("linux");
#endif
}

i64 TimeNow() {
#ifdef PLATFORM_WIN
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  // Convert Windows FILETIME (100-nanosecond intervals since January 1, 1601)
  // to UNIX timestamp in milliseconds
  i64 currentTime = (li.QuadPart - 116444736000000000LL) / 10000;
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  i64 currentTime = (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
#endif
  assert(currentTime != -1 && "currentTime should never be -1");
  return currentTime;
}

void WaitTime(i64 ms) {
#ifdef PLATFORM_WIN
  Sleep(ms);
#else
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  nanosleep(&ts, NULL);
#endif
}

// align must be a power of 2
void *ArenaAllocAligned(Arena *arena, size_t size, size_t align) {
  size_t tail = arena->offset & (align - 1);
  size_t begin = tail ? arena->offset + align - tail : arena->offset;
  size_t end = begin + size;
  if (!ArenaReserve(arena, end)) {
    return NULL;
  }
  arena->offset = end;
  void* ptr = arena->buffer + begin;
  memset(ptr, 0, size);
  return ptr;
}

void* ArenaAlloc(Arena *arena, size_t size) {
  return ArenaAllocAligned(arena, size, DEFAULT_ALIGNMENT);
}

bool ArenaReserve(Arena* arena, size_t cap) {
  if (cap >= arena->capacity) {
    // power of 2 growth policy
    size_t newCapacity = arena->capacity << 1;
    newCapacity = newCapacity > cap ? newCapacity : cap;
    void* newBuffer = malloc(newCapacity);
    if (!newBuffer) {
      return false;
    }
    if (arena->buffer) {
      memcpy(newBuffer, arena->buffer, arena->offset);
      free(arena->buffer);
    }
    arena->buffer = (int8_t*)newBuffer;
    arena->capacity = newCapacity;
  }
  return true;
}

char* ArenaAllocChars(Arena* arena, size_t count) {
  return (char*)ArenaAllocAligned(arena, count, 1);
}

void ArenaFree(Arena *arena) {
  if (arena->buffer) {
    free(arena->buffer);
  }
}

void ArenaReset(Arena *arena) {
  arena->offset = 0;
}

/* String Implementation */
static size_t maxStringSize = 10000;

static size_t strLength(char *str, size_t maxSize) {
  if (str == NULL) {
    return 0;
  }

  size_t len = 0;
  while (len < maxSize && str[len] != '\0') {
    len++;
  }

  return len;
}

static void addNullTerminator(char *str, size_t len) {
  str[len] = '\0';
}

bool StrIsNull(String *str) {
  return str == NULL || str->data == NULL;
}

void SetMaxStrSize(size_t size) {
  maxStringSize = size;
}

String StrNewSize(Arena *arena, char *str, size_t len) {
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = ArenaAllocChars(arena, memorySize);

  memcpy(allocatedString, str, memorySize);
  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
}

String StrNew(Arena *arena, char *str) {
  const size_t len = strLength(str, maxStringSize);
  if (len == 0) {
    return (String){0, NULL};
  }
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = ArenaAllocChars(arena, memorySize);

  memcpy(allocatedString, str, memorySize);
  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
}

String s(char *msg) {
  return (String){
      .length = strlen(msg),
      .data = msg,
  };
}

String StrConcat(Arena *arena, String *string1, String *string2) {
  assert(!StrIsNull(string1) && "string1 should never be NULL");
  assert(!StrIsNull(string2) && "string2 should never be NULL");

  const size_t len = string1->length + string2->length;
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = ArenaAllocChars(arena, memorySize);

  memcpy_s(allocatedString, memorySize, string1->data, string1->length);
  memcpy_s(allocatedString + string1->length, memorySize, string2->data, string2->length);
  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
};

void StrCopy(String *destination, String *source) {
  assert(!StrIsNull(destination) && "destination should never be NULL");
  assert(!StrIsNull(source) && "source should never be NULL");
  assert(destination->length >= source->length && "destination length should never smaller than source length");

  const errno_t result = memcpy_s(destination->data, destination->length, source->data, source->length);

  assert(result == 0 && "result should never be anything but 0");
  destination->length = source->length;
  addNullTerminator(destination->data, destination->length);
}

bool StrEqual(String *string1, String *string2) {
  if (string1->length != string2->length) {
    return false;
  }

  if (memcmp(string1->data, string2->data, string1->length) != 0) {
    return false;
  }
  return true;
}

StringVector StrSplit(Arena *arena, String *str, String *delimiter) {
  assert(!StrIsNull(str) && "str should never be NULL");
  assert(!StrIsNull(delimiter) && "delimiter should never be NULL");

  char *start = str->data;
  const char *end = str->data + str->length;
  char *curr = start;
  StringVector result = {0};
  if (delimiter->length == 0) {
    for (size_t i = 0; i < str->length; i++) {
      String currString = StrNewSize(arena, str->data + i, 1);
      VecPush(result, currString);
    }
    return result;
  }

  while (curr < end) {
    char *match = NULL;
    for (char *search = curr; search <= end - delimiter->length; search++) {
      if (memcmp(search, delimiter->data, delimiter->length) == 0) {
        match = search;
        break;
      }
    }

    if (!match) {
      String currString = StrNewSize(arena, curr, end - curr);
      VecPush(result, currString);
      break;
    }

    size_t len = match - curr;
    String currString = StrNewSize(arena, curr, len);
    VecPush(result, currString);

    curr = match + delimiter->length;
  }

  return result;
}

void StringToUpper(String *str) {
  for (int i = 0; i < str->length; ++i) {
    char currChar = str->data[i];
    str->data[i] = toupper(currChar);
  }
}

void StrToLower(String *str) {
  for (int i = 0; i < str->length; ++i) {
    char currChar = str->data[i];
    str->data[i] = tolower(currChar);
  }
}

bool isSpace(char character) {
  return character == ' ' || character == '\n' || character == '\t' || character == '\r';
}

void StrTrim(String *str) {
  char *firstChar = NULL;
  char *lastChar = NULL;
  if (str->length == 0) {
    return;
  }

  if (str->length == 1) {
    if (isSpace(str->data[0])) {
      str->data[0] = '\0';
      str->length = 0;
    }
    return;
  }

  for (int i = 0; i < str->length; ++i) {
    char *currChar = &str->data[i];
    if (isSpace(*currChar)) {
      continue;
    }

    if (firstChar == NULL) {
      firstChar = currChar;
    }
    lastChar = currChar;
  }

  if (firstChar == NULL || lastChar == NULL) {
    str->data[0] = '\0';
    str->length = 0;
    addNullTerminator(str->data, 0);
    return;
  }

  size_t len = (lastChar - firstChar) + 1;
  memcpy_s(str->data, str->length, firstChar, len);
  str->length = len;
  addNullTerminator(str->data, len);
}

String StrSlice(Arena *arena, String *str, i32 start, i32 end) {
  assert(start >= 0 && "start index must be non-negative");
  assert(start <= str->length && "start index out of bounds");

  if (end < 0) {
    end = str->length + end;
  }

  assert(end >= start && "end must be greater than or equal to start");
  assert(end <= str->length && "end index out of bounds");

  size_t len = end - start;
  return StrNewSize(arena, str->data + start, len);
}

String F(Arena *arena, const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t size = vsnprintf(NULL, 0, format, args) + 1; // +1 for null terminator
  va_end(args);

  char *buffer = (char *)ArenaAllocChars(arena, size);
  va_start(args, format);
  vsnprintf(buffer, size, format, args);
  va_end(args);

  return (String){.length = size - 1, .data = buffer};
}

String ConvertPath(Arena *arena, String path) {
  String platform = GetPlatform();
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
    memcpy(result.data, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  if (StrEqual(&platform, &S("linux")) || StrEqual(&platform, &S("macos"))) {
    return result;
  }

  for (size_t i = 0; i < result.length; i++) {
    if (result.data[i] == '/') {
      result.data[i] = '\\';
    }
  }

  return result;
}

String ParsePath(Arena *arena, String path) {
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
    memcpy(result.data, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  return result;
}

String ConvertExe(Arena *arena, String path) {
  String platform = GetPlatform();
  String exeExtension = S(".exe");

  bool hasExe = false;
  if (path.length >= exeExtension.length) {
    String pathEnd = StrSlice(arena, &path, path.length - exeExtension.length, path.length);
    if (StrEqual(&pathEnd, &exeExtension)) {
      hasExe = true;
    }
  }

  if (StrEqual(&platform, &S("windows"))) {
    if (hasExe) {
      return path;
    }
    return StrConcat(arena, &path, &exeExtension);
  }

  if (StrEqual(&platform, &S("linux")) || StrEqual(&platform, &S("macos"))) {
    if (hasExe) {
      return StrSlice(arena, &path, 0, path.length - exeExtension.length);
    }
    return path;
  }

  return path;
}

/* Random Implemenation */
static u64 seed = 0;
void RandomInit() {
  seed = TimeNow();
  srand(seed);
}

u64 RandomGetSeed() {
  return seed;
}

void RandomSetSeed(u64 newSeed) {
  seed = newSeed;
}

i32 RandomInteger(i32 min, i32 max) {
  assert(min <= max && "min should always be less than or equal to max");
  assert(max - min <= INT32_MAX - 1 && "range too large");

  i32 range = max - min + 1;

  // Calculate scaling factor to avoid modulo bias
  u32 buckets = RAND_MAX / range;
  u32 limit = buckets * range;

  // Reject numbers that would create bias
  u32 r;
  do {
    r = rand();
  } while (r >= limit);

  return min + (r / buckets);
}

f32 RandomFloat(f32 min, f32 max) {
  assert(min <= max && "min must be less than or equal to max");
  f32 normalized = (f32)rand() / (f32)RAND_MAX;
  return min + normalized * (max - min);
}

/* File Implementation */
#ifdef PLATFORM_WIN
char *GetCwd() {
  static _Thread_local char currentPath[PATH_MAX];
  DWORD length = GetCurrentDirectory(MAX_PATH, currentPath);
  if (length == 0) {
    printf("Error getting current directory: %lu\n", GetLastError());
    currentPath[0] = '\0';
  }
  return currentPath;
}
void SetCwd(char *destination) {
  bool result = SetCurrentDirectory(destination);
  if (!result) {
    printf("Error setting cwd: %lu\n", GetLastError());
  }
  GetCwd();
}

FileData *GetDirFiles() {
  WIN32_FIND_DATA findData;
  HANDLE hFind;
  char searchPath[MAX_PATH];
  FileData *fileData = NewFileData();
  i32 result = snprintf(searchPath, MAX_PATH - 2, "%s\\*", GetCwd());
  assert(result >= 0 && "sprint should not return error");

  hFind = FindFirstFile(searchPath, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    printf("Error finding files: %lu\n", GetLastError());
    return NULL;
  }

  do {
    if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
      continue;
    }

    if (fileData->totalCount >= MAX_FILES) {
      printf("Warning: Maximum file count reached (%d). Some files might be skipped.\n", MAX_FILES);
      break;
    }

    bool isDirectory = findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    File *currFile = &fileData->files[fileData->fileCount];
    Folder *currFolder = &fileData->folders[fileData->folderCount];

    if (isDirectory) {
      currFolder->name = strdup(findData.cFileName);
      fileData->folderCount++;
    }

    if (!isDirectory) {
      char *dot = strrchr(findData.cFileName, '.');
      if (dot != NULL) {
        currFile->extension = strdup(dot + 1);

        size_t baseNameLength = dot - findData.cFileName;
        char *baseName = (char *)malloc(baseNameLength + 1);
        memcpy(baseName, findData.cFileName, baseNameLength);
        baseName[baseNameLength] = '\0';
        currFile->name = baseName;
      }

      if (dot == NULL) {
        currFile->extension = strdup("");
        currFile->name = strdup(findData.cFileName);
      }

      LARGE_INTEGER createTime, modifyTime;
      createTime.LowPart = findData.ftCreationTime.dwLowDateTime;
      createTime.HighPart = findData.ftCreationTime.dwHighDateTime;
      modifyTime.LowPart = findData.ftLastWriteTime.dwLowDateTime;
      modifyTime.HighPart = findData.ftLastWriteTime.dwHighDateTime;

      const i64 WINDOWS_TICK = 10000000;
      const i64 SEC_TO_UNIX_EPOCH = 11644473600LL;

      currFile->createTime = createTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;
      currFile->modifyTime = modifyTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;
      currFile->size = (((i64)findData.nFileSizeHigh) << 32) | findData.nFileSizeLow;
      fileData->fileCount++;
    }

    fileData->totalCount++;
  } while (FindNextFile(hFind, &findData) != 0);

  DWORD dwError = GetLastError();
  if (dwError != ERROR_NO_MORE_FILES) {
    printf("Error searching for files: %lu\n", dwError);
  }

  FindClose(hFind);
  return fileData;
}

FileData *NewFileData() {
  FileData *fileData = (FileData *)malloc(sizeof(FileData));
  fileData->files = (File *)malloc(MAX_FILES * sizeof(File));
  fileData->fileCount = 0;
  fileData->folders = (Folder *)malloc(MAX_FILES * sizeof(Folder));
  fileData->folderCount = 0;
  fileData->totalCount = 0;
  return fileData;
};

void FreeFileData(FileData *fileData) {
  if (fileData->files == NULL && fileData->folders == NULL) return;

  for (size_t i = 0; i < fileData->fileCount; i++) {
    File currentFile = fileData->files[i];
    free(currentFile.name);
    free(currentFile.extension);
  }

  free(fileData->files);

  for (size_t i = 0; i < fileData->folderCount; i++) {
    Folder currentFolder = fileData->folders[i];
    free(currentFolder.name);
  }

  free(fileData->folders);
  free(fileData);
}

errno_t FileStats(String *path, File *result) {
  WIN32_FILE_ATTRIBUTE_DATA fileAttr = {0};

  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return MEMORY_ALLOCATION_FAILED;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';

  if (!GetFileAttributesExA(pathStr, GetFileExInfoStandard, &fileAttr)) {
    LogError("Failed to get file attributes: %lu", GetLastError());
    free(pathStr);
    return FILE_GET_ATTRIBUTES_FAILED;
  }

  char *nameStart = strrchr(pathStr, '\\');
  if (!nameStart) {
    nameStart = strrchr(pathStr, '/');
  }

  if (nameStart) {
    nameStart++;
  } else {
    nameStart = pathStr;
  }

  result->name = strdup(nameStart);

  char *extStart = strrchr(nameStart, '.');
  if (extStart) {
    result->extension = strdup(extStart + 1);
  } else {
    result->extension = strdup("");
  }

  LARGE_INTEGER fileSize;
  fileSize.HighPart = fileAttr.nFileSizeHigh;
  fileSize.LowPart = fileAttr.nFileSizeLow;
  result->size = fileSize.QuadPart;

  LARGE_INTEGER createTime, modifyTime;
  createTime.LowPart = fileAttr.ftCreationTime.dwLowDateTime;
  createTime.HighPart = fileAttr.ftCreationTime.dwHighDateTime;
  modifyTime.LowPart = fileAttr.ftLastWriteTime.dwLowDateTime;
  modifyTime.HighPart = fileAttr.ftLastWriteTime.dwHighDateTime;

  const i64 WINDOWS_TICK = 10000000;
  const i64 SEC_TO_UNIX_EPOCH = 11644473600LL;

  result->createTime = createTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;
  result->modifyTime = modifyTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;

  free(pathStr);
  return SUCCESS;
}

errno_t FileRead(Arena *arena, String *path, String *result) {
  HANDLE hFile = INVALID_HANDLE_VALUE;

  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return MEMORY_ALLOCATION_FAILED;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';
  hFile = CreateFileA(pathStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    free(pathStr);

    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      return FILE_NOT_EXIST;
    }

    LogError("File open failed, err: %lu", error);
    return FILE_OPEN_FAILED;
  }

  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(hFile, &fileSize)) {
    LogError("Failed to get file size: %lu\n", GetLastError());
    CloseHandle(hFile);
    free(pathStr);
    return FILE_GET_SIZE_FAILED;
  }

  char *buffer = (char *)ArenaAllocChars(arena, fileSize.QuadPart);
  if (!buffer) {
    LogError("Memory allocation failed");
    CloseHandle(hFile);
    free(pathStr);
    return MEMORY_ALLOCATION_FAILED;
  }

  DWORD bytesRead;
  if (!ReadFile(hFile, buffer, (DWORD)fileSize.QuadPart, &bytesRead, NULL) || bytesRead != fileSize.QuadPart) {
    LogError("Failed to read file: %lu", GetLastError());
    CloseHandle(hFile);
    free(pathStr);
    return FILE_READ_FAILED;
  }

  *result = StrNewSize(arena, buffer, (size_t)bytesRead);

  CloseHandle(hFile);
  free(pathStr);
  return SUCCESS;
}

errno_t FileWrite(String *path, String *result) {
  HANDLE hFile = INVALID_HANDLE_VALUE;

  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return ENOMEM;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';

  hFile = CreateFileA(pathStr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    free(pathStr);

    switch (error) {
    case ERROR_ACCESS_DENIED:
      return EACCES;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
    case ERROR_FILE_NOT_FOUND:
      return ENOENT;
    default:
      return EIO;
    }
  }

  DWORD bytesWritten;
  if (!WriteFile(hFile, result->data, (DWORD)result->length, &bytesWritten, NULL) || bytesWritten != result->length) {
    DWORD error = GetLastError();
    CloseHandle(hFile);
    free(pathStr);

    switch (error) {
    case ERROR_DISK_FULL:
      return ENOSPC;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
    default:
      return EIO;
    }
  }

  CloseHandle(hFile);
  free(pathStr);

  return SUCCESS;
}

errno_t FileDelete(String *path) {
  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return ENOMEM;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';

  if (!DeleteFileA(pathStr)) {
    DWORD error = GetLastError();
    free(pathStr);
    switch (error) {
    case ERROR_ACCESS_DENIED:
      return EACCES;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
    case ERROR_FILE_NOT_FOUND:
      return ENOENT;
    default:
      return EIO;
    }
  }

  free(pathStr);
  return SUCCESS;
}

errno_t FileRename(String *oldPath, String *newPath) {
  if (!MoveFileEx(oldPath->data, newPath->data, MOVEFILE_REPLACE_EXISTING)) {
    return 1;
  }

  return SUCCESS;
}

bool Mkdir(String path) {
  bool result = CreateDirectory(path.data, NULL);
  if (result != false) {
    return true;
  }

  u64 error = GetLastError();
  if (error == ERROR_ALREADY_EXISTS) {
    return true;
  }

  LogError("Error meanwhile Mkdir() %llu", error);
  return false;
}

#elif defined(PLATFORM_LINUX)

char *GetCwd() {
  static _Thread_local char currentPath[PATH_MAX];
  if (getcwd(currentPath, PATH_MAX) == NULL) {
    printf("Error getting current directory: %s\n", strerror(errno));
    currentPath[0] = '\0';
  }
  return currentPath;
}
void SetCwd(char *destination) {
  if (chdir(destination) != 0) {
    printf("Error setting cwd: %s\n", strerror(errno));
  }
  GetCwd();
}
#endif

/* Logger Implemenation */
void LogInfo(const char *format, ...) {
  printf("%s[INFO]: ", GRAY);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogWarn(const char *format, ...) {
  printf("%s[WARN]: ", ORANGE);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogError(const char *format, ...) {
  printf("%s[ERROR]: ", RED);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogSuccess(const char *format, ...) {
  printf("%s[SUCCESS]: ", GREEN);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogInit() {
#ifdef PLATFORM_WIN
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
}
#endif

#ifdef __cplusplus
}
#endif
