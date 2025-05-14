/* MIT License

  base.h - Better cross-platform STD
  Version - 2025-05-10 (0.1.16):
  https://github.com/TomasBorquez/base.h

  Usage:
    #define BASE_IMPLEMENTATION
    #include "base.h"

  More on the the `README.md`
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* --- Platform MACROS and includes --- */
#if defined(__clang__)
#  define COMPILER_CLANG
#elif defined(_MSC_VER)
#  define COMPILER_MSVC
#elif defined(__GNUC__)
#  define COMPILER_GCC
#elif defined(__TINYC__)
#  define COMPILER_TCC
#else
#  error "The codebase only supports GCC, Clang, TCC and MSVC"
#endif

#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#  define NORETURN __attribute__((noreturn))
#  define UNLIKELY(x) __builtin_expect(!!(x), 0)
#  define ALLOC_ATTR2(sz, al) __attribute__((malloc, alloc_size(sz), alloc_align(al)))
#  define ALLOC_ATTR(sz) __attribute__((malloc, alloc_size(sz)))
#  define FORMAT_CHECK(fmt_pos, args_pos) __attribute__((format(printf, fmt_pos, args_pos)))
#  define WARN_UNUSED __attribute__((warn_unused_result))
#elif defined(COMPILER_MSVC)
#  define NORETURN __declspec(noreturn)
#  define UNLIKELY(x) x
#  define ALLOC_ATTR2(sz, al)
#  define ALLOC_ATTR(sz)
#  define FORMAT_CHECK(fmt_pos, args_pos)
#  define WARN_UNUSED _Check_return_
#else
#  define NORETURN __declspec(noreturn)
#  define UNLIKELY(x) x
#  define ALLOC_ATTR2(sz, al)
#  define ALLOC_ATTR(sz)
#  define FORMAT_CHECK(fmt_pos, args_pos)
#  define WARN_UNUSED
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#  define PLATFORM_WIN
#elif defined(__linux__) || defined(__gnu_linux__)
#  define PLATFORM_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
#  define PLATFORM_MACOS
#else
#  error "The codebase only supports linux, macos and windows"
#endif

#if defined(COMPILER_CLANG)
#  define FILE_NAME __FILE_NAME__
#else
#  define FILE_NAME __FILE__
#endif

#if defined(PLATFORM_WIN)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  define _POSIX_C_SOURCE 200809L
#  define _GNU_SOURCE
#  include <dirent.h>
#  include <errno.h>
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#if defined(__STDC_VERSION__)
#  if (__STDC_VERSION__ >= 202311L)
#    define C_STANDARD_C23
#    define C_STANDARD "C23"
#  elif (__STDC_VERSION__ >= 201710L)
#    define C_STANDARD_C17
#    define C_STANDARD "C17"
#  elif (__STDC_VERSION__ >= 201112L)
#    define C_STANDARD_C11
#    define C_STANDARD "C11"
#  elif (__STDC_VERSION__ >= 199901L)
#    define C_STANDARD_C99
#    define C_STANDARD "C99"
#  else
#    error "Why C89 if you have C99"
#  endif
#else
#  if defined(COMPILER_MSVC)
#    if defined(_MSC_VER) && _MSC_VER >= 1920 // >= Visual Studio 2019
#      define C_STANDARD_C17
#      define C_STANDARD "C17"
#    else
#      define C_STANDARD_C11
#      define C_STANDARD "C11"
#    endif
#  endif
#endif

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* --- Platform Specific --- */
#if defined(PLATFORM_WIN)
/* Process functions */
#  define popen _popen
#  define pclose _pclose

/* File I/O functions */
#  define fileno _fileno
#  define fdopen _fdopen
#  define access _access
#  define unlink _unlink
#  define isatty _isatty
#  define dup _dup
#  define dup2 _dup2
#  define ftruncate _chsize
#  define fsync _commit

/* Directory functions */
#  define mkdir(path, mode) _mkdir(path)
#  define rmdir _rmdir
#  define getcwd _getcwd
#  define chdir _chdir

/* Process/Threading */
#  define getpid _getpid
#  define execvp _execvp
#  define execve _execve
#  define sleep(x) Sleep((x) * 1000)
#  define usleep(x) Sleep((x) / 1000)

/* String functions */
#  define strcasecmp _stricmp
#  define strncasecmp _strnicmp
#  define strdup _strdup

/* File modes */
#  define R_OK 4
#  define W_OK 2
#  define X_OK 0 /* Windows doesn't have explicit X_OK */
#  define F_OK 0

/* File descriptors */
#  define STDIN_FILENO 0
#  define STDOUT_FILENO 1
#  define STDERR_FILENO 2

/* Some functions need complete replacements */
#  if defined(COMPILER_MSVC)
#    define snprintf _snprintf
#    define vsnprintf _vsnprintf
#  endif
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

void _custom_assert(const char *expr, const char *file, unsigned line, const char *format, ...) FORMAT_CHECK(4, 5);
#define Assert(expression, ...) (void)((!!(expression)) || (_custom_assert(#expression, __FILE__, __LINE__, __VA_ARGS__), 0))

/* --- Vector --- */
// TODO: `VecSort` implement some sorting algorithm for sorting the vector
#define VEC_TYPE(typeName, valueType) \
  typedef struct {                    \
    valueType *data;                  \
    size_t length;                    \
    size_t capacity;                  \
  } typeName

#define VecReserve(vector, count)                       \
  do {                                                  \
    vector.capacity = count;                            \
    vector.data = Malloc(count * sizeof(*vector.data)); \
  } while (0)

#define VecPush(vector, value) vecPush((void **)&(vector).data, &(vector).length, &(vector).capacity, sizeof(*vector.data), &(value));

#define VecPop(vector) vecPop((vector).data, &(vector).length, sizeof(*vector.data));

#define VecShift(vector) vecShift((void **)&(vector).data, &(vector).length, sizeof(*vector.data))

#define VecUnshift(vector, value) vecUnshift((void **)&(vector).data, &(vector).length, &(vector).capacity, sizeof(*vector.data), &(value))

#define VecInsert(vector, value, index) vecInsert((void **)&(vector).data, &(vector).length, &(vector).capacity, sizeof(*vector.data), &(value), index)

#define VecAt(vector, index) (*(__typeof__(*vector.data) *)vecAt((void **)&(vector).data, &(vector).length, index, sizeof(*vector.data)))

#define VecAtPtr(vector, index) (vecAt((void **)&(vector).data, &(vector).length, index, sizeof(*vector.data)))

#define VecFree(vector) vecFree((void **)&(vector).data)

#define VecForEach(vector, it) for (__typeof__(*vector.data) *it = vector.data; it < vector.data + vector.length; it++)

/* --- Time and Platforms --- */
i64 TimeNow();
void WaitTime(i64 ms);
String GetCompiler();
String GetPlatform();

/* --- Error --- */
typedef i32 errno_t;

enum GeneralError {
  SUCCESS = 0,
};

String ErrToStr(errno_t err);
/* --- Arena --- */
typedef struct __ArenaChunk {
  struct __ArenaChunk *next;
  size_t cap;
  char buffer[];
} __ArenaChunk;

typedef struct {
  __ArenaChunk *current;
  size_t offset;
  __ArenaChunk *root;
  size_t chunkSize;
} Arena;

// This makes sure right alignment on 86/64 bits
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

Arena *ArenaCreate(size_t chunkSize);
ALLOC_ATTR2(2, 3) void *ArenaAllocAligned(Arena *arena, size_t size, size_t align);
ALLOC_ATTR(2) char *ArenaAllocChars(Arena *arena, size_t count);
ALLOC_ATTR(2) void *ArenaAlloc(Arena *arena, size_t size);
void ArenaFree(Arena *arena);
void ArenaReset(Arena *arena);

/* --- Memory Allocations --- */
void *Realloc(void *block, size_t size);
void *Malloc(size_t size);
void Free(void *address);

/* --- String and Macros --- */
#define STRING_LENGTH(s) ((sizeof(s) / sizeof((s)[0])) - sizeof((s)[0])) // NOTE: Inspired from clay.h
#define ENSURE_STRING_LITERAL(x) ("" x "")

// NOTE: If an error led you here, it's because `S` can only be used with string literals, i.e. `S("SomeString")` and not `S(yourString)` - for that use `s()`
#define S(string) (TYPE_INIT(String){.length = STRING_LENGTH(ENSURE_STRING_LITERAL(string)), .data = (string)})
String s(char *msg);

String F(Arena *arena, const char *format, ...) FORMAT_CHECK(2, 3);

VEC_TYPE(StringVector, String);
#define StringVectorPushMany(vector, ...)              \
  do {                                                 \
    char *values[] = {__VA_ARGS__};                    \
    size_t count = sizeof(values) / sizeof(values[0]); \
    for (size_t i = 0; i < count; i++) {               \
      VecPush(vector, s(values[i]));                   \
    }                                                  \
  } while (0)

void SetMaxStrSize(size_t size);
String StrNew(Arena *arena, char *str);
String StrNewSize(Arena *arena, char *str, size_t len); // Without null terminator

void StrCopy(String destination, String source);
StringVector StrSplit(Arena *arena, String string, String delimiter);
StringVector StrSplitNewLine(Arena *arena, String str);
bool StrEq(String string1, String string2);
String StrConcat(Arena *arena, String string1, String string2);

void StrToLower(String string1);
void StrToUpper(String string1);

bool StrIsNull(String string);
void StrTrim(String *string);

String StrSlice(Arena *arena, String str, size_t start, size_t end);

String NormalizePath(Arena *arena, String path);
String NormalizeExePath(Arena *arena, String path);
String NormalizeStaticLibPath(Arena *arena, String path);
String NormalizePathStart(Arena *arena, String path);

typedef struct {
  size_t capacity;
  String buffer;
} StringBuilder;

StringBuilder StringBuilderCreate(Arena *arena);
StringBuilder StringBuilderReserve(Arena *arena, size_t capacity);
void StringBuilderAppend(Arena *arena, StringBuilder *builder, String *string);

/* --- Random --- */
void RandomInit();
u64 RandomGetSeed();
void RandomSetSeed(u64 newSeed);
i32 RandomInteger(i32 min, i32 max);
f32 RandomFloat(f32 min, f32 max);

/* --- File System --- */
#define MAX_FILES 200

typedef struct {
  char *name;
  char *extension;
  i64 size;
  i64 createTime;
  i64 modifyTime;
} File;

char *GetCwd();
void SetCwd(char *destination);
bool Mkdir(String path); // NOTE: Mkdir if not exist
StringVector ListDir(Arena *arena, String path);

enum FileStatsError { FILE_GET_ATTRIBUTES_FAILED = 100, FILE_STATS_FILE_NOT_EXIST };
WARN_UNUSED errno_t FileStats(String path, File *file);

enum FileReadError { FILE_NOT_EXIST = 200, FILE_OPEN_FAILED, FILE_GET_SIZE_FAILED, FILE_READ_FAILED };
WARN_UNUSED errno_t FileRead(Arena *arena, String path, String *result);

enum FileWriteError { FILE_WRITE_OPEN_FAILED = 300, FILE_WRITE_ACCESS_DENIED, FILE_WRITE_NO_MEMORY, FILE_WRITE_NOT_FOUND, FILE_WRITE_DISK_FULL, FILE_WRITE_IO_ERROR };
WARN_UNUSED errno_t FileWrite(String path, String data);

// enum FileWriteError - Same error enum since it uses `FileWrite("")` under the hood
WARN_UNUSED errno_t FileReset(String path);

enum FileAddError { FILE_ADD_OPEN_FAILED = 400, FILE_ADD_ACCESS_DENIED, FILE_ADD_NO_MEMORY, FILE_ADD_NOT_FOUND, FILE_ADD_DISK_FULL, FILE_ADD_IO_ERROR };
WARN_UNUSED errno_t FileAdd(String path, String data); // NOTE: Adds `\n` at the end always

enum FileDeleteError { FILE_DELETE_ACCESS_DENIED = 500, FILE_DELETE_NOT_FOUND, FILE_DELETE_IO_ERROR };
WARN_UNUSED errno_t FileDelete(String path);

enum FileRenameError { FILE_RENAME_ACCESS_DENIED = 600, FILE_RENAME_NOT_FOUND, FILE_RENAME_IO_ERROR };
WARN_UNUSED errno_t FileRename(String oldPath, String newPath);

/* --- Logger --- */
#define _RESET "\x1b[0m"
#define _GRAY "\x1b[0;36m"
#define _RED "\x1b[0;31m"
#define _GREEN "\x1b[0;32m"
#define _ORANGE "\x1b[0;33m"

void LogInit();
void LogInfo(const char *format, ...) FORMAT_CHECK(1, 2);
void LogWarn(const char *format, ...) FORMAT_CHECK(1, 2);
void LogError(const char *format, ...) FORMAT_CHECK(1, 2);
void LogSuccess(const char *format, ...) FORMAT_CHECK(1, 2);

/* --- Math --- */
#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define Max(a, b) (((a) > (b)) ? (a) : (b))
#define Clamp(a, x, b) (((x) < (a)) ? (a) : ((b) < (x)) ? (b) : (x))
#define Swap(a, b) \
  do {             \
    a ^= b;        \
    b ^= a;        \
    a ^= b;        \
  } while (0);

/* --- Defer Macros --- */
#if defined(DEFER_MACRO) // NOTE: Optional since not all compilers support it and not all C versions do either
/* - GCC implementation -
  NOTE: Must use C23 (depending on the platform)
*/
#  if defined(COMPILER_GCC)
#    define defer __DEFER(__COUNTER__)
#    define __DEFER(N) __DEFER_(N)
#    define __DEFER_(N) __DEFER__(__DEFER_FUNCTION_##N, __DEFER_VARIABLE_##N)
#    define __DEFER__(F, V)      \
      auto void F(int *);        \
      [[gnu::cleanup(F)]] int V; \
      auto void F(int *)

/* - Clang implementation -
  NOTE: Must compile with flag `-fblocks`
*/
#  elif defined(COMPILER_CLANG)
typedef void (^const __df_t)(void);

[[maybe_unused]]
static inline void __df_cb(__df_t *__fp) {
  (*__fp)();
}

#    define defer __DEFER(__COUNTER__)
#    define __DEFER(N) __DEFER_(N)
#    define __DEFER_(N) __DEFER__(__DEFER_VARIABLE_##N)
#    define __DEFER__(V) [[gnu::cleanup(__df_cb)]] __df_t V = ^void(void)

/* -- MSVC implementation --
  NOTE: Not available yet in MSVC, use `_try/_finally`
*/
#  elif defined(COMPILER_MSVC)
#    error "Not available yet in MSVC, use `_try/_finally`"
#  endif
#endif

/* --- INI Parser --- */
typedef struct {
  String key;
  String value;
} IniEntry;

VEC_TYPE(IniEntryVector, IniEntry);

typedef struct {
  IniEntryVector data;
  Arena *arena;
} IniFile;

WARN_UNUSED errno_t IniParse(String path, IniFile *result);
WARN_UNUSED errno_t IniWrite(String path, IniFile *iniFile); // NOTE: Updates/Creates .ini file

void IniFree(IniFile *iniFile);

String IniGet(IniFile *ini, String key);
String IniSet(IniFile *ini, String key, String value);
i32 IniGetInt(IniFile *ini, String key);
i64 IniGetLong(IniFile *ini, String key);
f64 IniGetDouble(IniFile *ini, String key);
bool IniGetBool(IniFile *ini, String key);

/* MIT License
   base.h - Implementation of base.h
   https://github.com/TomasBorquez/base.h
*/

#if defined(BASE_IMPLEMENTATION)
// --- Vector Implementation ---
static void vecPush(void **data, size_t *length, size_t *capacity, size_t element_size, void *value) {
  // WARNING: Vector must always be initialized to zero `Vector vector = {0}`
  Assert(*length <= *capacity, "VecPush: Possible memory corruption or vector not initialized, `Vector vector = {0}`");

  if (*length >= *capacity) {
    if (*capacity == 0) *capacity = 128;
    else *capacity *= 2;

    *data = realloc(*data, *capacity * element_size);
  }

  void *address = (char *)(*data) + (*length * element_size);
  memcpy(address, value, element_size);

  (*length)++;
}

static void *vecPop(void *data, size_t *length, size_t element_size) {
  Assert(*length > 0, "VecPop: Cannot pop from empty vector");
  (*length)--;
  return (char *)data + (*length * element_size);
}

static void vecShift(void **data, size_t *length, size_t element_size) {
  Assert(*length != 0, "VecShift: Length should at least be >= 1");
  memmove(*data, (char *)(*data) + element_size, ((*length) - 1) * element_size);
  (*length)--;
}

static void vecUnshift(void **data, size_t *length, size_t *capacity, size_t element_size, const void *value) {
  if (*length >= *capacity) {
    if (*capacity == 0) *capacity = 2;
    else *capacity *= 2;
    *data = realloc(*data, *capacity * element_size);
  }

  if (*length > 0) {
    memmove((char *)(*data) + element_size, *data, (*length) * element_size);
  }

  memcpy(*data, value, element_size);
  (*length)++;
}

static void vecInsert(void **data, size_t *length, size_t *capacity, size_t element_size, void *value, size_t index) {
  Assert(index <= *length, "VecInsert: Index out of bounds for insertion");

  if (*length >= *capacity) {
    if (*capacity == 0) *capacity = 2;
    else *capacity *= 2;
    *data = realloc(*data, *capacity * element_size);
  }

  if (index < *length) {
    memmove((char *)(*data) + ((index + 1) * element_size), (char *)(*data) + (index * element_size), (*length - index) * element_size);
  }

  memcpy((char *)(*data) + (index * element_size), value, element_size);
  (*length)++;
}

static void *vecAt(void **data, size_t *length, size_t index, size_t elementSize) {
  Assert(index >= 0 && index < *length, "VecAt: Index out of bounds");
  void *address = (char *)(*data) + (index * elementSize);
  return address;
}

static void vecFree(void **data) {
  free(*data);
  *data = NULL;
}

// --- Time and Platforms Implementation ---
#  if !defined(PLATFORM_WIN)

#    if !defined(EINVAL)
#      define EINVAL 22 // Invalid argument
#    endif

#    if !defined(ERANGE)
#      define ERANGE 34 // Result too large
#    endif

WARN_UNUSED errno_t memcpy_s(void *dest, size_t destSize, const void *src, size_t count) {
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

WARN_UNUSED errno_t fopen_s(FILE **streamptr, const char *filename, const char *mode) {
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
#  endif

String GetCompiler() {
#  if defined(COMPILER_CLANG)
  return S("clang");
#  elif defined(COMPILER_GCC)
  return S("gcc");
#  elif defined(COMPILER_TCC)
  return S("tcc");
#  elif defined(COMPILER_MSVC)
  return S("MSVC");
#  endif
}

String GetPlatform() {
#  if defined(PLATFORM_WIN)
  return S("windows");
#  elif defined(PLATFORM_LINUX)
  return S("linux");
#  elif defined(PLATFORM_MACOS)
  return S("macos");
#  endif
}

i64 TimeNow() {
#  if defined(PLATFORM_WIN)
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  // Convert Windows FILETIME (100-nanosecond intervals since January 1, 1601)
  // to UNIX timestamp in milliseconds
  i64 currentTime = (li.QuadPart - 116444736000000000LL) / 10000;
#  else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  i64 currentTime = (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
#  endif
  Assert(currentTime != -1, "TimeNow: currentTime should never be -1");
  return currentTime;
}

void WaitTime(i64 ms) {
#  if defined(PLATFORM_WIN)
  sleep(ms);
#  else
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  nanosleep(&ts, NULL);
#  endif
}

/* --- Error Implementation --- */
String FileErrToStr(errno_t err) {
  if (err < 100) {
#  if defined(PLATFORM_WIN)
    char buffer[256];
    DWORD msgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, sizeof(buffer), NULL);

    // Trim message
    if (msgLen > 0) {
      while (msgLen > 0 && (buffer[msgLen - 1] == '\r' || buffer[msgLen - 1] == '\n')) {
        buffer[--msgLen] = '\0';
      }

      if (msgLen > 0 && buffer[msgLen - 1] == '.') {
        buffer[--msgLen] = '\0';
      }

      return s(buffer);
    }
    return S("Unknown system error");
#  else
    return s(strerror(err));
#  endif
  }

  switch (err) {
  // FileStats errors (100-199)
  case FILE_GET_ATTRIBUTES_FAILED:
    return S("Failed to get file attributes");
  case FILE_STATS_FILE_NOT_EXIST:
    return S("File does not exist");

  // FileRead errors (200-299)
  case FILE_NOT_EXIST:
    return S("File does not exist");
  case FILE_OPEN_FAILED:
    return S("Failed to open file for reading");
  case FILE_GET_SIZE_FAILED:
    return S("Failed to get file size");
  case FILE_READ_FAILED:
    return S("Failed to read file");

  // FileWrite errors (300-399)
  case FILE_WRITE_OPEN_FAILED:
    return S("Failed to open file for writing");
  case FILE_WRITE_ACCESS_DENIED:
    return S("Access denied when writing file");
  case FILE_WRITE_NO_MEMORY:
    return S("Not enough memory to write file");
  case FILE_WRITE_NOT_FOUND:
    return S("File not found for writing");
  case FILE_WRITE_DISK_FULL:
    return S("Disk full when writing file");
  case FILE_WRITE_IO_ERROR:
    return S("I/O error when writing file");

  // FileAdd errors (400-499)
  case FILE_ADD_OPEN_FAILED:
    return S("Failed to open file for appending");
  case FILE_ADD_ACCESS_DENIED:
    return S("Access denied when appending to file");
  case FILE_ADD_NO_MEMORY:
    return S("Not enough memory to append to file");
  case FILE_ADD_NOT_FOUND:
    return S("File not found for appending");
  case FILE_ADD_DISK_FULL:
    return S("Disk full when appending to file");
  case FILE_ADD_IO_ERROR:
    return S("I/O error when appending to file");

  // FileDelete errors (500-599)
  case FILE_DELETE_ACCESS_DENIED:
    return S("Access denied when deleting file");
  case FILE_DELETE_NOT_FOUND:
    return S("File not found for deletion");
  case FILE_DELETE_IO_ERROR:
    return S("I/O error when deleting file");

  // FileRename errors (600-699)
  case FILE_RENAME_ACCESS_DENIED:
    return S("Access denied when renaming file");
  case FILE_RENAME_NOT_FOUND:
    return S("File not found for renaming");
  case FILE_RENAME_IO_ERROR:
    return S("I/O error when renaming file");

  default:
    return S("Unknown file error");
  }
}

void _custom_assert(const char *expr, const char *file, unsigned line, const char *format, ...) {
  printf("%sAssertion failed: %s, file %s, line %u %s\n", _RED, expr, file, line, _RESET);

  if (format) {
    va_list args;
    va_start(args, format);
    LogError(format, args);
    va_start(args, format);
  }

  abort();
}

/* --- Arena Implementation --- */
// Allocate or iterate to next chunk that can fit `bytes`
void __ArenaNextChunk(Arena *arena, size_t bytes) {
  __ArenaChunk *next = arena->current ? arena->current->next : NULL;
  while (next) {
    arena->current = next;
    if (arena->current->cap > bytes) {
      return;
    }
    next = arena->current->next;
  }
  next = (__ArenaChunk *)Malloc(sizeof(__ArenaChunk) + bytes);
  next->cap = bytes;
  next->next = NULL;
  if (arena->current) arena->current->next = next;
  arena->current = next;
}

void *ArenaAllocAligned(Arena *arena, size_t size, size_t al) {
  void *current_pos = arena->current->buffer + arena->offset;
  intptr_t mask = al - 1;
  intptr_t misalignment = ((intptr_t)current_pos & mask);
  intptr_t padding = misalignment ? al - misalignment : 0;

  arena->offset += padding;

  void *result;
  if (arena->offset + size > arena->current->cap) {
    size_t bytes = size > arena->chunkSize ? size : arena->chunkSize;
    __ArenaNextChunk(arena, bytes);

    current_pos = arena->current->buffer;
    misalignment = ((intptr_t)current_pos & mask);
    padding = misalignment ? al - misalignment : 0;
    arena->offset = padding;

    result = arena->current->buffer + arena->offset;
    arena->offset += size;
  } else {
    result = arena->current->buffer + arena->offset;
    arena->offset += size;
  }

  if (size) memset(result, 0, size);
  return result;
}

char *ArenaAllocChars(Arena *arena, size_t count) {
  return (char *)ArenaAllocAligned(arena, count, 1);
}

void *ArenaAlloc(Arena *arena, const size_t size) {
  return ArenaAllocAligned(arena, size, DEFAULT_ALIGNMENT);
}

void ArenaFree(Arena *arena) {
  __ArenaChunk *chunk = arena->root;
  while (chunk) {
    __ArenaChunk *next = chunk->next;
    free(chunk);
    chunk = next;
  }
  free(arena);
}

void ArenaReset(Arena *arena) {
  arena->current = arena->root;
  arena->offset = 0;
}

Arena *ArenaCreate(size_t chunkSize) {
  Arena *res = (Arena *)Malloc(sizeof(Arena));
  memset(res, 0, sizeof(*res));
  res->chunkSize = chunkSize;
  __ArenaNextChunk(res, chunkSize);
  res->root = res->current;
  return res;
}

/* Memory Allocations */
// TODO: Add hashmap that checks for unfreed values only on DEBUG, __FILE__ and __LINE__
void *Malloc(size_t size) {
  Assert(size > 0, "Malloc: size cant be negative");

  void *address = malloc(size);
  Assert(address != NULL, "Malloc: failed, returned address should never be NULL");
  return address;
}

void *Realloc(void *block, size_t size) {
  Assert(size > 0, "Realloc: size cant be negative");

  void *address = realloc(block, size);
  Assert(address != NULL, "Realloc: failed, returned address should never be NULL");
  return address;
}

void Free(void *address) {
  free(address);
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

bool StrIsNull(String str) {
  return str.data == NULL;
}

void SetMaxStrSize(size_t size) {
  maxStringSize = size;
}

String StrNewSize(Arena *arena, char *str, size_t len) {
  Assert(str != NULL, "StrNewSize: failed, can't give a NULL str");

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

String StrConcat(Arena *arena, String string1, String string2) {
  if (StrIsNull(string1)) {
    const size_t len = string2.length;
    const size_t memorySize = sizeof(char) * len;
    char *allocatedString = ArenaAllocChars(arena, memorySize);

    errno_t err = memcpy_s(allocatedString, memorySize, string2.data, string2.length);
    Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

    addNullTerminator(allocatedString, len);
    return (String){len, allocatedString};
  }

  if (StrIsNull(string2)) {
    const size_t len = string1.length;
    const size_t memorySize = sizeof(char) * len;
    char *allocatedString = ArenaAllocChars(arena, memorySize);
    errno_t err = memcpy_s(allocatedString, memorySize, string1.data, string1.length);
    Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

    addNullTerminator(allocatedString, len);
    return (String){len, allocatedString};
  }

  const size_t len = string1.length + string2.length;
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = ArenaAllocChars(arena, memorySize);

  errno_t err = memcpy_s(allocatedString, memorySize, string1.data, string1.length);
  Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

  err = memcpy_s(allocatedString + string1.length, memorySize, string2.data, string2.length);
  Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
}

void StrCopy(String destination, String source) {
  Assert(!StrIsNull(destination), "StrCopy: destination should never be NULL");
  Assert(!StrIsNull(source), "StrCopy: source should never be NULL");
  Assert(destination.length >= source.length, "destination length should never smaller than source length");

  errno_t err = memcpy_s(destination.data, destination.length, source.data, source.length);
  Assert(err == SUCCESS, "StrCopy: memcpy_s failed, err: %d", err);

  destination.length = source.length;
  addNullTerminator(destination.data, destination.length);
}

bool StrEq(String string1, String string2) {
  if (string1.length != string2.length) {
    return false;
  }

  if (memcmp(string1.data, string2.data, string1.length) != 0) {
    return false;
  }

  return true;
}

StringVector StrSplit(Arena *arena, String str, String delimiter) {
  Assert(!StrIsNull(str), "StrSplit: str should never be NULL");
  Assert(!StrIsNull(delimiter), "StrSplit: delimiter should never be NULL");

  char *start = str.data;
  const char *end = str.data + str.length;
  char *curr = start;
  StringVector result = {0};
  if (delimiter.length == 0) {
    for (size_t i = 0; i < str.length; i++) {
      String currString = StrNewSize(arena, str.data + i, 1);
      VecPush(result, currString);
    }
    return result;
  }

  while (curr < end) {
    char *match = NULL;
    for (char *search = curr; search <= end - delimiter.length; search++) {
      if (memcmp(search, delimiter.data, delimiter.length) == 0) {
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

    curr = match + delimiter.length;
  }

  return result;
}

StringVector StrSplitNewLine(Arena *arena, String str) {
  Assert(!StrIsNull(str), "SplitNewLine: str should never be NULL");
  char *start = str.data;
  const char *end = str.data + str.length;
  char *curr = start;
  StringVector result = {0};

  while (curr < end) {
    char *pos = curr;

    while (pos < end && *pos != '\n') {
      pos++;
    }

    size_t len = pos - curr;

    if (pos < end && pos > curr && *(pos - 1) == '\r') {
      len--;
    }

    String currString = StrNewSize(arena, curr, len);
    VecPush(result, currString);

    if (pos < end) {
      curr = pos + 1;
    } else {
      break;
    }
  }

  return result;
}

void StrToUpper(String str) {
  for (size_t i = 0; i < str.length; ++i) {
    char currChar = str.data[i];
    str.data[i] = toupper(currChar);
  }
}

void StrToLower(String str) {
  for (size_t i = 0; i < str.length; ++i) {
    char currChar = str.data[i];
    str.data[i] = tolower(currChar);
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

  for (size_t i = 0; i < str->length; ++i) {
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
  errno_t err = memcpy_s(str->data, str->length, firstChar, len);
  Assert(err == SUCCESS, "str->rim: memcpy_s failed, err: %d", err);

  str->length = len;
  addNullTerminator(str->data, len);
}

String StrSlice(Arena *arena, String str, size_t start, size_t end) {
  Assert(start >= 0, "StrSlice: start index must be non-negative");
  Assert(start <= str.length, "StrSlice: start index out of bounds");

  if (end < 0) {
    end = str.length + end;
  }

  Assert(end >= start, "StrSlice: end must be greater than or equal to start");
  Assert(end <= str.length, "StrSlice: end index out of bounds");

  size_t len = end - start;
  return StrNewSize(arena, str.data + start, len);
}

String F(Arena *arena, const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t size = vsnprintf(NULL, 0, format, args) + 1; // +1 for null terminator
  va_end(args);

  char *buffer = ArenaAllocChars(arena, size);
  va_start(args, format);
  vsnprintf(buffer, size, format, args);
  va_end(args);

  return (String){.length = size - 1, .data = buffer};
}

String normSlashes(String path) {
#  if defined(PLATFORM_WIN)
  for (size_t i = 0; i < path.length; i++) {
    if (path.data[i] == '/') {
      path.data[i] = '\\';
    }
  }
#  else
  for (size_t i = 0; i < path.length; i++) {
    if (path.data[i] == '\\') {
      path.data[i] = '/';
    }
  }
#  endif
  return path;
}

String NormalizePath(Arena *arena, String path) {
  String result;
  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  return normSlashes(result);
}

String NormalizeExePath(Arena *arena, String path) {
  String platform = GetPlatform();
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  bool hasExe = false;
  String exeExtension = S(".exe");
  if (result.length >= exeExtension.length) {
    String resultEnd = StrSlice(arena, result, result.length - exeExtension.length, result.length);
    if (StrEq(resultEnd, exeExtension)) {
      hasExe = true;
    }
  }

  if (StrEq(platform, S("windows"))) {
    if (!hasExe) {
      result = StrConcat(arena, result, exeExtension);
    }

    return normSlashes(result);
  }

  if (hasExe) {
    result = StrSlice(arena, result, 0, result.length - exeExtension.length);
  }

  return normSlashes(result);
}

String NormalizeStaticLibPath(Arena *arena, String path) {
  String platform = GetPlatform();
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  bool hasLibExt = false;
  String libExtension;
  String aExtension = S(".a");
  String libWinExtension = S(".lib");
  if (result.length >= aExtension.length) {
    String resultEnd = StrSlice(arena, result, result.length - aExtension.length, result.length);
    if (StrEq(resultEnd, aExtension)) {
      hasLibExt = true;
      libExtension = aExtension;
    }
  }

  if (!hasLibExt && result.length >= libWinExtension.length) {
    String resultEnd = StrSlice(arena, result, result.length - libWinExtension.length, result.length);
    if (StrEq(resultEnd, libWinExtension)) {
      hasLibExt = true;
      libExtension = libWinExtension;
    }
  }

  if (StrEq(platform, S("windows"))) {
    if (hasLibExt && !StrEq(libExtension, libWinExtension)) {
      result = StrSlice(arena, result, 0, result.length - libExtension.length);
      hasLibExt = false;
    }

    if (!hasLibExt) {
      result = StrConcat(arena, result, libWinExtension);
    }

    return normSlashes(result);
  }

  if (hasLibExt && !StrEq(libExtension, aExtension)) {
    result = StrSlice(arena, result, 0, result.length - libExtension.length);
    hasLibExt = false;
  }

  if (!hasLibExt) {
    result = StrConcat(arena, result, aExtension);
  }

  return normSlashes(result);
}

String NormalizePathStart(Arena *arena, String path) {
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  return result;
}

StringBuilder StringBuilderCreate(Arena *arena) {
  StringBuilder result = {0};
  char *data = ArenaAllocChars(arena, 128);

  result.capacity = 128;
  result.buffer = (String){.data = data, .length = 0};
  return result;
}

StringBuilder StringBuilderReserve(Arena *arena, size_t capacity) {
  StringBuilder result = {0};
  char *data = ArenaAllocChars(arena, capacity);

  result.capacity = capacity;
  result.buffer = (String){.data = data, .length = 0};
  return result;
}

void StringBuilderAppend(Arena *arena, StringBuilder *builder, String *string) {
  size_t newLength = builder->buffer.length + string->length;
  if (newLength + 1 >= builder->capacity) {
    size_t newCapacity = (newLength + 1) * 2;
    char *data = ArenaAllocChars(arena, newCapacity);

    memcpy(data, builder->buffer.data, builder->buffer.length);
    builder->buffer.data = data;
    builder->capacity = newCapacity;
  }

  memcpy(builder->buffer.data + builder->buffer.length, string->data, string->length);
  builder->buffer.length = newLength;
  builder->buffer.data[builder->buffer.length] = '\0';
}

/* Random Implemenation */
static u64 seed = 0;

u64 RandomGetSeed() {
  return seed;
}

void RandomSetSeed(u64 newSeed) {
  seed = newSeed;
  srand(seed);
}

i32 RandomInteger(i32 min, i32 max) {
  Assert(min <= max, "RandomInteger: min should always be less than or equal to max");
  Assert(max - min <= INT32_MAX - 1, "RandomInteger: range too large");

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
  Assert(min <= max, "RandomFloat: min must be less than or equal to max");
  f32 normalized = (f32)rand() / (f32)RAND_MAX;
  return min + normalized * (max - min);
}

/* File System Implementation */
#  if defined(PLATFORM_WIN)
char *GetCwd() {
  static char currentPath[MAX_PATH];
  DWORD length = GetCurrentDirectory(MAX_PATH, currentPath);
  if (length == 0) {
    LogError("GetCwd: failed getting current directory, err: %lu", GetLastError());
    currentPath[0] = '\0';
  }
  return currentPath;
}

void SetCwd(char *destination) {
  bool result = SetCurrentDirectory(destination);
  if (!result) {
    LogError("SetCwd: failed setting cwd for %s, err: %lu", destination, GetLastError());
  }
  GetCwd();
}

errno_t FileStats(String path, File *result) {
  if (GetFileAttributesA(path.data) == INVALID_FILE_ATTRIBUTES) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      return FILE_STATS_FILE_NOT_EXIST;
    }
    return FILE_GET_ATTRIBUTES_FAILED;
  }

  WIN32_FILE_ATTRIBUTE_DATA fileAttr = {0};
  if (!GetFileAttributesExA(path.data, GetFileExInfoStandard, &fileAttr)) {
    return FILE_GET_ATTRIBUTES_FAILED;
  }

  char *nameStart = strrchr(path.data, '\\');
  if (!nameStart) {
    nameStart = strrchr(path.data, '/');
  }
  if (nameStart) {
    nameStart++;
  } else {
    nameStart = path.data;
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

  return SUCCESS;
}

errno_t FileRead(Arena *arena, String path, String *result) {
  HANDLE hFile = INVALID_HANDLE_VALUE;

  hFile = CreateFileA(path.data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();

    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      return FILE_NOT_EXIST;
    }

    return FILE_OPEN_FAILED;
  }

  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(hFile, &fileSize)) {
    CloseHandle(hFile);
    return FILE_GET_SIZE_FAILED;
  }

  DWORD bytesRead;
  char *buffer = ArenaAllocChars(arena, fileSize.QuadPart);
  if (!ReadFile(hFile, buffer, (DWORD)fileSize.QuadPart, &bytesRead, NULL) || bytesRead != fileSize.QuadPart) {
    CloseHandle(hFile);
    return FILE_READ_FAILED;
  }

  *result = (String){.length = bytesRead, .data = buffer};

  CloseHandle(hFile);
  return SUCCESS;
}

errno_t FileWrite(String path, String data) {
  HANDLE hFile = INVALID_HANDLE_VALUE;

  hFile = CreateFileA(path.data, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();

    switch (error) {
    case ERROR_ACCESS_DENIED:
      return FILE_WRITE_ACCESS_DENIED;
    case ERROR_FILE_NOT_FOUND:
      return FILE_WRITE_NOT_FOUND;
    default:
      return FILE_WRITE_OPEN_FAILED;
    }
  }

  DWORD bytesWritten;
  if (!WriteFile(hFile, data.data, (DWORD)data.length, &bytesWritten, NULL) || bytesWritten != data.length) {
    DWORD error = GetLastError();
    CloseHandle(hFile);

    switch (error) {
    case ERROR_DISK_FULL:
      return FILE_WRITE_DISK_FULL;
    default:
      return FILE_WRITE_IO_ERROR;
    }
  }

  CloseHandle(hFile);

  return SUCCESS;
}

errno_t FileAdd(String path, String data) {
  HANDLE hFile = INVALID_HANDLE_VALUE;
  hFile = CreateFileA(path.data, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();

    switch (error) {
    case ERROR_ACCESS_DENIED:
      return FILE_ADD_ACCESS_DENIED;
    case ERROR_FILE_NOT_FOUND:
      return FILE_ADD_NOT_FOUND;
    default:
      return FILE_ADD_OPEN_FAILED;
    }
  }

  if (SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
    CloseHandle(hFile);
    return FILE_ADD_IO_ERROR;
  }

  char *newData = Malloc(data.length + 1); // NOTE: +1 for \n
  memcpy(newData, data.data, data.length);
  newData[data.length] = '\n';
  DWORD newLength = (DWORD)data.length + 1;
  DWORD bytesWritten;
  if (!WriteFile(hFile, newData, newLength, &bytesWritten, NULL) || bytesWritten != newLength) {
    DWORD error = GetLastError();
    CloseHandle(hFile);
    Free(newData);
    switch (error) {
    case ERROR_DISK_FULL:
      return FILE_ADD_DISK_FULL;
    default:
      return FILE_ADD_IO_ERROR;
    }
  }

  CloseHandle(hFile);
  Free(newData);
  return SUCCESS;
}

errno_t FileDelete(String path) {
  if (!DeleteFileA(path.data)) {
    DWORD error = GetLastError();
    switch (error) {
    case ERROR_ACCESS_DENIED:
      return FILE_DELETE_ACCESS_DENIED;
    case ERROR_FILE_NOT_FOUND:
      return FILE_DELETE_NOT_FOUND;
    default:
      return FILE_DELETE_IO_ERROR;
    }
  }

  return SUCCESS;
}

errno_t FileRename(String oldPath, String newPath) {
  if (!MoveFileEx(oldPath.data, newPath.data, MOVEFILE_REPLACE_EXISTING)) {
    DWORD error = GetLastError();
    switch (error) {
    case ERROR_ACCESS_DENIED:
      return FILE_RENAME_ACCESS_DENIED;
    case ERROR_FILE_NOT_FOUND:
      return FILE_RENAME_NOT_FOUND;
    default:
      return FILE_RENAME_IO_ERROR;
    }
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

  LogError("Mkdir: failed for %s, err: %llu", path.data, error);
  return false;
}

StringVector ListDir(Arena *arena, String path) {
  StringVector result = {0};
  WIN32_FIND_DATA findData;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  char searchPath[MAX_PATH];

  snprintf(searchPath, MAX_PATH, "%s\\*", path.data);
  hFind = FindFirstFile(searchPath, &findData);

  if (hFind == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    LogError("ListDir: failed for %s, err: %lu", path.data, error);
    return result;
  }

  do {
    if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
      continue;
    }

    String entry = StrNew(arena, findData.cFileName);
    VecPush(result, entry);

  } while (FindNextFile(hFind, &findData) != 0);

  DWORD error = GetLastError();
  if (error != ERROR_NO_MORE_FILES) {
    LogError("ListDir: failed reading directory %s, err: %lu", path.data, error);
  }

  FindClose(hFind);
  return result;
}
#  else
char *GetCwd() {
  static char currentPath[PATH_MAX];
  if (getcwd(currentPath, PATH_MAX) == NULL) {
    LogError("GetCwd: failed getting current directory, err: %s", strerror(errno));
    currentPath[0] = '\0';
  }
  return currentPath;
}

void SetCwd(char *destination) {
  if (chdir(destination) != 0) {
    LogError("SetCwd: failed setting cwd for %s, err: %s", destination, strerror(errno));
  }
  GetCwd();
}

errno_t FileStats(String path, File *result) {
  struct stat fileStat;
  if (stat(path.data, &fileStat) != 0) {
    if (errno == ENOENT) {
      return FILE_STATS_FILE_NOT_EXIST;
    }
    return FILE_GET_ATTRIBUTES_FAILED;
  }

  char *nameStart = strrchr(path.data, '/');
  if (nameStart) {
    nameStart++;
  } else {
    nameStart = path.data;
  }
  result->name = strdup(nameStart);

  char *extStart = strrchr(nameStart, '.');
  if (extStart) {
    result->extension = strdup(extStart + 1);
  } else {
    result->extension = strdup("");
  }

  result->size = fileStat.st_size;
  result->createTime = fileStat.st_ctime; // Creation time (may be change time on some Unix systems)
  result->modifyTime = fileStat.st_mtime; // Modification time

  return SUCCESS;
}

errno_t FileRead(Arena *arena, String path, String *result) {
  FILE *file = fopen(path.data, "r");
  if (!file) {
    int error = errno;
    if (error == ENOENT) {
      return FILE_NOT_EXIST;
    }
    return FILE_OPEN_FAILED;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return FILE_GET_SIZE_FAILED;
  }

  long fileSize = ftell(file);
  if (fileSize == -1) {
    fclose(file);
    return FILE_GET_SIZE_FAILED;
  }

  rewind(file);

  char *buffer = ArenaAllocChars(arena, fileSize);
  size_t bytesRead = fread(buffer, 1, fileSize, file);
  if (bytesRead != (size_t)fileSize) {
    fclose(file);
    return FILE_READ_FAILED;
  }

  *result = (String){.length = bytesRead, .data = buffer};
  fclose(file);
  return SUCCESS;
}

errno_t FileWrite(String path, String data) {
  i32 fd = -1;

  fd = open(path.data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    i32 error = errno;

    switch (error) {
    case EACCES:
      return FILE_WRITE_ACCESS_DENIED;
    case ENOENT:
      return FILE_WRITE_NOT_FOUND;
    default:
      return FILE_WRITE_OPEN_FAILED;
    }
  }

  ssize_t bytesWritten = write(fd, data.data, data.length);
  if (bytesWritten != data.length) {
    int error = errno;
    close(fd);

    switch (error) {
    case ENOSPC:
      return FILE_WRITE_DISK_FULL;
    default:
      return FILE_WRITE_IO_ERROR;
    }
  }

  close(fd);

  return SUCCESS;
}

errno_t FileAdd(String path, String data) {
  i32 fd = -1;

  fd = open(path.data, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (fd == -1) {
    int error = errno;

    switch (error) {
    case EACCES:
      return FILE_ADD_ACCESS_DENIED;
    case ENOENT:
      return FILE_ADD_NOT_FOUND;
    default:
      return FILE_ADD_OPEN_FAILED;
    }
  }

  char *newData = Malloc(data.length + 1); // +1 for \n
  memcpy(newData, data.data, data.length);
  newData[data.length] = '\n';
  size_t newLength = data.length + 1;

  ssize_t bytesWritten = write(fd, newData, newLength);
  if (bytesWritten != newLength) {
    int error = errno;
    close(fd);
    Free(newData);

    switch (error) {
    case ENOSPC:
      return FILE_ADD_DISK_FULL;
    default:
      return FILE_ADD_IO_ERROR;
    }
  }

  close(fd);
  Free(newData);
  return SUCCESS;
}

errno_t FileDelete(String path) {
  if (unlink(path.data) != 0) {
    int error = errno;

    switch (error) {
    case EACCES:
      return FILE_DELETE_ACCESS_DENIED;
    case ENOENT:
      return FILE_DELETE_NOT_FOUND;
    default:
      return FILE_DELETE_IO_ERROR;
    }
  }

  return SUCCESS;
}

errno_t FileRename(String oldPath, String newPath) {
  if (rename(oldPath.data, newPath.data) != 0) {
    errno_t error = errno;

    switch (error) {
    case EACCES:
      return FILE_RENAME_ACCESS_DENIED;
    case ENOENT:
      return FILE_RENAME_NOT_FOUND;
    default:
      return FILE_RENAME_IO_ERROR;
    }
  }

  return SUCCESS;
}

bool Mkdir(String path) {
  struct stat st;
  if (stat(path.data, &st) == 0 && S_ISDIR(st.st_mode)) {
    return true; // Directory already exists
  }

  if (mkdir(path.data, 0755) != 0) {
    if (errno != EEXIST) {
      LogError("Mkdir: failed to create directory for %s, err: %s", path.data, strerror(errno));
      return false;
    }
  }

  return true;
}

StringVector ListDir(Arena *arena, String path) {
  StringVector result = {0};
  DIR *dir = opendir(path.data);

  if (dir == NULL) {
    errno_t error = errno;
    LogError("ListDir: failed opening directory for %s, err: %d", path.data, error);
    return result;
  }

  errno = 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    String entryStr = StrNew(arena, entry->d_name);
    VecPush(result, entryStr);

    errno = 0;
  }

  if (errno != 0) {
    errno_t error = errno;
    LogError("ListDir: failed reading directory %s, err: %d", path.data, error);
  }

  closedir(dir);
  return result;
}
#  endif

errno_t FileReset(String path) {
  return FileWrite(path, S(""));
}

/* Logger Implemenation */
void LogInfo(const char *format, ...) {
  printf("%s[INFO]: ", _GRAY);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", _RESET);
}

void LogWarn(const char *format, ...) {
  printf("%s[WARN]: ", _ORANGE);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", _RESET);
}

void LogError(const char *format, ...) {
  printf("%s[ERROR]: ", _RED);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", _RESET);
}

void LogSuccess(const char *format, ...) {
  printf("%s[SUCCESS]: ", _GREEN);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", _RESET);
}

void LogInit() {
#  if defined(PLATFORM_WIN)
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#  endif
}

/* --- INI Parser Implementation --- */
errno_t IniParse(String path, IniFile *result) {
  File stats = {0};
  errno_t err = FileStats(path, &stats);
  if (err == FILE_STATS_FILE_NOT_EXIST) {
    LogWarn("IniParse: %s does not exist, creating...", path.data);
    errno_t fileResetErr = FileReset(path);
    Assert(fileResetErr == SUCCESS, "IniParse: Failed creating file for path %s, err: %d", path.data, fileResetErr);

    result->arena = ArenaCreate(sizeof(String) * 10); // Initialize arena
    return SUCCESS;
  }

  if (err != SUCCESS) {
    return err;
  }

  String buffer;
  result->arena = ArenaCreate(stats.size * 4);
  err = FileRead(result->arena, path, &buffer);
  if (err != SUCCESS) {
    return err;
  }

  StringVector iniSplit = StrSplitNewLine(result->arena, buffer);
  VecForEach(iniSplit, currLine) {
    if (currLine->length == 0 || currLine->data[0] == ';') {
      continue;
    }

    size_t equalPos = (size_t)-1;
    for (size_t j = 0; j < currLine->length; j++) {
      if (currLine->data[j] == '=') {
        equalPos = j;
        break;
      }
    }

    if (equalPos == (size_t)-1) {
      continue;
    }

    String key = StrSlice(result->arena, *currLine, 0, equalPos);
    String value = StrSlice(result->arena, *currLine, equalPos + 1, currLine->length);

    IniEntry entry = {.key = key, .value = value};
    VecPush(result->data, entry);
  }

  VecFree(iniSplit);
  return SUCCESS;
}

errno_t IniWrite(String path, IniFile *iniFile) {
  errno_t err = FileReset(path);
  if (err != SUCCESS) {
    return err;
  }

  VecForEach(iniFile->data, entry) {
    String value = F(iniFile->arena, "%s=%s", entry->key.data, entry->value.data);
    err = FileAdd(path, value);
    if (err != SUCCESS) {
      return err;
    }
  }

  return SUCCESS;
}

void IniFree(IniFile *iniFile) {
  ArenaFree(iniFile->arena);
  VecFree(iniFile->data);
}

String IniGet(IniFile *ini, String key) {
  VecForEach(ini->data, entry) {
    if (StrEq(entry->key, key)) {
      return entry->value;
    }
  }

  return (String){0};
}

String IniSet(IniFile *ini, String key, String value) {
  VecForEach(ini->data, entry) {
    if (StrEq(entry->key, key)) {
      entry->value = value;
      return entry->value;
    }
  }

  IniEntry newEntry;
  newEntry.key = key;
  newEntry.value = value;
  VecPush(ini->data, newEntry);

  return newEntry.value;
}

i32 IniGetInt(IniFile *ini, String key) {
  String value = IniGet(ini, key);
  if (StrIsNull(value)) {
    return 0;
  }

  char *endPtr;
  i32 result = (i32)strtol(value.data, &endPtr, 10);
  if (endPtr == value.data) {
    LogWarn("IniGetLong: Failed to convert [key: %s, value: %s] to int", key.data, value.data);
    return 0;
  }

  return result;
}

i64 IniGetLong(IniFile *ini, String key) {
  String value = IniGet(ini, key);
  if (StrIsNull(value)) {
    return 0;
  }

  char *endPtr;
  i64 result = strtoll(value.data, &endPtr, 10);
  if (endPtr == value.data) {
    LogWarn("IniGetLong: Failed to convert [key: %s, value: %s] to long", key.data, value.data);
    return 0;
  }

  return result;
}

f64 IniGetDouble(IniFile *ini, String key) {
  String value = IniGet(ini, key);
  if (StrIsNull(value)) {
    return 0.0;
  }

  char *endPtr;
  f64 result = strtod(value.data, &endPtr);
  if (endPtr == value.data) {
    LogWarn("IniGetLong: Failed to convert [key: %s, value: %s] to double", key.data, value.data);
    return 0.0;
  }

  return result;
}

bool IniGetBool(IniFile *ini, String key) {
  String value = IniGet(ini, key);
  if (StrIsNull(value)) {
    return false;
  }

  return StrEq(value, S("true"));
}
#endif

#ifdef __cplusplus
}
#endif
