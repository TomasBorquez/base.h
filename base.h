/* MIT License

  base.h - Better cross-platform STD
  Version - 2026-04-20 (0.2.7):
  https://github.com/TomasBorquez/base.h

  Usage:
    #define BASE_IMPLEMENTATION
    #include "base.h"

  More on the the `README.md`
*/
#pragma once

/* --- Platform MACROS and includes --- */
#if defined(__clang__)
#  define COMPILER_CLANG
#elif defined(__GNUC__)
#  define COMPILER_GCC
#elif defined(_MSC_VER)
#  define COMPILER_MSVC
#elif defined(__TINYC__)
#  define COMPILER_TCC
#else
#  error "The codebase only supports GCC, Clang, TCC and MSVC"
#endif

#if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#  define NORETURN __attribute__((noreturn))
#  define RETURNS_NON_NULL __attribute__((returns_nonnull))
#  define PARAM_NON_NULL __attribute__((nonnull))

#  define ALLOC_ATTR(sz) __attribute__((malloc, alloc_size(sz), returns_nonnull))
#  define ALLOC_ATTR2(sz, al) __attribute__((malloc, alloc_size(sz), alloc_align(al), returns_nonnull))

#  define UNLIKELY(x) __builtin_expect(!!(x), 0)
#  define FORMAT_CHECK(fmt_pos, args_pos) __attribute__((format(printf, fmt_pos, args_pos)))
#  define WARN_UNUSED __attribute__((warn_unused_result))
#elif defined(COMPILER_MSVC)
#  define NORETURN __declspec(noreturn)
#  define RETURNS_NON_NULL
#  define PARAM_NON_NULL

#  define ALLOC_ATTR(sz)
#  define ALLOC_ATTR2(sz, al)

#  define UNLIKELY(x) x
#  define FORMAT_CHECK(fmt_pos, args_pos)
#  define WARN_UNUSED _Check_return_
#else
#  define NORETURN __declspec(noreturn)
#  define RETURNS_NON_NULL
#  define PARAM_NON_NULL

#  define ALLOC_ATTR(sz)
#  define ALLOC_ATTR2(sz, al)

#  define UNLIKELY(x) x
#  define FORMAT_CHECK(fmt_pos, args_pos)
#  define WARN_UNUSED
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#  define PLATFORM_WIN
#else
#  define PLATFORM_UNIX
#  if defined(__ANDROID__)
#    define PLATFORM_ANDROID
#  elif defined(__linux__) || defined(__gnu_linux__)
#    define PLATFORM_LINUX
#  elif defined(__APPLE__) || defined(__MACH__)
#    define PLATFORM_MACOS
#  elif defined(__FreeBSD__)
#    define PLATFORM_FREEBSD
#  elif defined(__EMSCRIPTEN__)
#    define PLATFORM_EMSCRIPTEN
#  else
#    error "The codebase only supports linux, macos, FreeBSD, windows, android and emscripten"
#  endif
#endif

#if defined(COMPILER_CLANG)
#  define FILE_NAME __FILE_NAME__
#else
#  define FILE_NAME __FILE__
#endif

#if defined(PLATFORM_WIN)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  if !defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING) // old SDKs sometimes dont have it
#    define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#  endif
#  include <BaseTsd.h>
#else
#  define _POSIX_C_SOURCE 200809L
#  define _GNU_SOURCE
#  include <dirent.h>
#  include <errno.h>
#  include <fcntl.h>
#  include <limits.h>
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
#include <inttypes.h>
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
#  define ssize_t SSIZE_T

/* Process/Threading */
#  define sleep(x) Sleep((x) * 1000)
#  define usleep(x) Sleep((x) / 1000)
#  define popen _popen
#  define pclose _pclose

/* Some functions need complete replacements */
#  if defined(COMPILER_MSVC)
#    define snprintf _snprintf
#    define vsnprintf _vsnprintf
#  endif
#endif

/* --- Types and MACRO types --- */
typedef float float32_t;
typedef double float64_t;

typedef struct {
  size_t length; // does not include null terminator
  char *data;
} String;

#define FMT_I8 "%" PRIi8
#define FMT_I16 "%" PRIi16
#define FMT_I32 "%" PRIi32
#define FMT_I64 "%" PRIi64

#define FMT_U8 "%" PRIu8
#define FMT_U16 "%" PRIu16
#define FMT_U32 "%" PRIu32
#define FMT_U64 "%" PRIu64

#define ARR_LEN(arr) sizeof((arr)) / sizeof(*(arr))

/* --- Vector --- */
typedef int32_t (*CompareFunc)(const void *a, const void *b);

int32_t __base_vec_partition(void **data, size_t element_size, CompareFunc compare, int32_t low, int32_t high);
void __base_vec_quicksort(void **data, size_t element_size, CompareFunc compare, int32_t low, int32_t high);

#define VecSort(vector, compare) __base_vec_quicksort((void **)&(vector).data, sizeof(*(vector).data), compare, 0, (vector).length - 1)

#define VEC_TYPE(typeName, valueType) \
  typedef struct {                    \
    valueType *data;                  \
    size_t length;                    \
    size_t capacity;                  \
  } typeName

#define VecReserve(vector, count)                           \
  do {                                                      \
    vector.capacity = (count);                              \
    vector.data = Malloc((count) * sizeof(*(vector).data)); \
  } while (0)

#define VecPush(vector, value) __base_vec_push((void **)&(vector).data, &(vector).length, &(vector).capacity, sizeof(*(vector).data), &(value));
void __base_vec_push(void **data, size_t *length, size_t *capacity, size_t element_size, void *value);

#define VecPop(vector) __base_vec_pop((vector).data, &(vector).length, sizeof(*(vector).data));
void *__base_vec_pop(void *data, size_t *length, size_t element_size);

#define VecShift(vector) __base_vec_shift((void **)&(vector).data, &(vector).length, sizeof(*(vector).data))
void __base_vec_shift(void **data, size_t *length, size_t element_size);

#define VecUnshift(vector, value) __base_vec_unshift((void **)&(vector).data, &(vector).length, &(vector).capacity, sizeof(*(vector).data), &(value))
void __base_vec_unshift(void **data, size_t *length, size_t *capacity, size_t element_size, const void *value);

#define VecInsert(vector, value, index) __base_vec_insert((void **)&(vector).data, &(vector).length, &(vector).capacity, sizeof(*(vector).data), &(value), (index))
void __base_vec_insert(void **data, size_t *length, size_t *capacity, size_t element_size, void *value, size_t index);

#define VecAt(vector, index) (*(__typeof__(*(vector).data) *)__base_vec_at((void **)&(vector).data, &(vector).length, index, sizeof(*(vector).data)))
void *__base_vec_at(void **data, size_t *length, size_t index, size_t elementSize);

#define VecAtPtr(vector, index) (__base_vec_at((void **)&(vector).data, &(vector).length, (index), sizeof(*(vector).data)))

#define VecFree(vector) __base_vec_free((void **)&(vector).data, &(vector).length, &(vector).capacity)
void __base_vec_free(void **data, size_t *length, size_t *capacity);

#define VecForEach(vector, it) for (__typeof__(*(vector).data) *(it) = (vector).data; (vector).data && (it) < (vector).data + (vector).length; (it)++)

/* --- Time and Platforms --- */
int64_t TimeNow(void);
void WaitTime(int64_t ms);

bool isLinux(void);
bool isMacOs(void);
bool isWindows(void);
bool isUnix(void);
bool isAndroid(void);
bool isEmscripten(void);
bool isFreeBSD(void);

typedef enum { WINDOWS = 1, LINUX, MACOS, FREEBSD } Platform;
Platform GetPlatform(void);

typedef enum { GCC = 1, CLANG, TCC, MSVC } Compiler;
Compiler GetCompiler(void);

/* --- Error --- */
typedef int32_t errno_t;

typedef enum {
  SUCCESS = 0,

  FILE_ACCESS_DENIED,    // EACCES
  FILE_NOT_FOUND,        // ENOENT
  FILE_NO_MEMORY,        // ENOMEM
  FILE_DISK_FULL,        // ENOSPC
  FILE_READ_FAILED,      // read returned fewer bytes than expected
  FILE_WRITE_FAILED,     // write returned fewer bytes than expected
  FILE_TOO_MANY_OPEN,    // EMFILE / ENFILE
  FILE_PATH_TOO_LONG,    // ENAMETOOLONG
  FILE_IS_DIRECTORY,     // EISDIR
  FILE_READ_ONLY_FS,     // EROFS
  FILE_ALREADY_EXISTS,   // EEXIST
} Error;

#define RESULT_TYPE(result_name, result_type) \
  typedef struct {                            \
    result_type data;                         \
    Error error;                              \
  } result_name

Error ErrnoMatch(errno_t err);
String ErrToStr(errno_t err);

#define Assert(expression, ...) (void)((!!(expression)) || (_custom_assert(#expression, __FILE__, __LINE__, __VA_ARGS__), 0))
static void _custom_assert(const char *expr, const char *file, unsigned line, const char *format, ...) FORMAT_CHECK(4, 5);

#define Unreachable(...) (void)((_custom_unreachable(__FILE__, __LINE__, __VA_ARGS__), 0))
static void _custom_unreachable(const char *file, unsigned line, const char *format, ...) FORMAT_CHECK(3, 4);

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

#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

Arena *ArenaCreate(size_t chunkSize); // TODO: add allocation hint

ALLOC_ATTR(2) void *ArenaAlloc(Arena *arena, size_t size);
ALLOC_ATTR(2) char *ArenaAllocChars(Arena *arena, size_t count);
ALLOC_ATTR2(2, 3) void *ArenaAllocAligned(Arena *arena, size_t size, size_t align);

void ArenaFree(Arena *arena) PARAM_NON_NULL;
void ArenaReset(Arena *arena) PARAM_NON_NULL;

/* --- Memory Allocations --- */
void *Realloc(void *block, size_t size) RETURNS_NON_NULL;
void *Malloc(size_t size) RETURNS_NON_NULL;
void Free(void *address) PARAM_NON_NULL;

/* --- String and Macros --- */
#define TYPE_INIT(type) (type)
#define STRING_LENGTH(s) ((sizeof((s)) / sizeof((s)[0])) - sizeof((s)[0])) // NOTE: Inspired from clay.h
#define ENSURE_STRING_LITERAL(x) ("" x "")

// NOTE: If an error led you here, it's because `S` can only be used with string literals, i.e. `S("SomeString")` and not `S(yourString)` - for that use `s()`
#define S(string) (TYPE_INIT(String){.length = STRING_LENGTH(ENSURE_STRING_LITERAL(string)), .data = (char *)(uintptr_t)(string)})
String s(char *msg);
String F(Arena *arena, const char *format, ...) FORMAT_CHECK(2, 3);

VEC_TYPE(StringVector, String);

void SetMaxStrSize(size_t size);
String StrNew(Arena *arena, char *str);
String StrNewSize(Arena *arena, char *str, size_t len); // len without null terminator

void StrCopy(String *destination, String source);
bool StrEq(String string1, String string2);
String StrConcat(Arena *arena, String string1, String string2);
StringVector StrSplit(Arena *arena, String string, String delimiter);

void StrToUpper(String string1);
void StrToLower(String string1);

bool StrIsNull(String string);
void StrTrim(String *string);

String StrSlice(Arena *arena, String str, size_t start, ssize_t end);
bool StrIncludes(String source, String subStr);

String NormalizePath(Arena *arena, String path);
String NormalizeExePath(Arena *arena, String path);
String NormalizeExtension(Arena *arena, String path);
String NormalizeStaticLibPath(Arena *arena, String path);
String NormalizePathStart(Arena *arena, String path);
String NormalizePathEnd(Arena *arena, String path);

typedef struct {
  size_t capacity;
  String buffer;
  Arena *arena_builder;
} StringBuilder;

StringBuilder SBCreate(Arena *arena);
StringBuilder SBReserve(Arena *arena, size_t capacity);
void SBAdd(StringBuilder *builder, String string);
void SBAddF(StringBuilder *builder, char *fmt, ...);
void SBAddFormatV(StringBuilder *builder, char *fmt, va_list args);

#define SBAddS(builder, string) SBAdd(builder, S(string))

/* --- Random --- */
void RandomInit(void);
uint64_t RandomGetSeed(void);
void RandomSetSeed(uint64_t newSeed);
int32_t RandomInteger(int32_t min, int32_t max);
float32_t RandomFloat(float32_t min, float32_t max);

/* --- File System --- */
typedef struct {
  char *name;
  char *extension;
  int64_t size;
  int64_t createTime;
  int64_t modifyTime;
} File;

RESULT_TYPE(GetCwdResult, String);
WARN_UNUSED GetCwdResult GetCwd(void);

WARN_UNUSED Error SetCwd(char *destination);
WARN_UNUSED Error Mkdir(String path);

RESULT_TYPE(ListDirResult, StringVector);
WARN_UNUSED ListDirResult ListDir(Arena *arena, String path);

RESULT_TYPE(FileStatsResult, File);
WARN_UNUSED FileStatsResult FileStats(String path);

RESULT_TYPE(FileReadResult, String);
WARN_UNUSED FileReadResult FileRead(Arena *arena, String path, size_t file_size);

WARN_UNUSED Error FileWrite(String path, String data);
WARN_UNUSED Error FileAdd(String path, String data);
WARN_UNUSED Error FileDelete(String path);
WARN_UNUSED Error FileRename(String oldPath, String newPath);
WARN_UNUSED Error FileCopy(String sourcePath, String destPath);

/* --- Logger --- */
#define _RESET "\x1b[0m"
#define _GRAY "\x1b[0;36m"
#define _RED "\x1b[0;31m"
#define _GREEN "\x1b[0;32m"
#define _ORANGE "\x1b[0;33m"

void LogInit(void);
void LogInfo(const char *format, ...) FORMAT_CHECK(1, 2);
void LogWarn(const char *format, ...) FORMAT_CHECK(1, 2);
void LogError(const char *format, ...) FORMAT_CHECK(1, 2);
void LogSuccess(const char *format, ...) FORMAT_CHECK(1, 2);
void logErrorV(const char *format, va_list args) FORMAT_CHECK(1, 0);

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
#if defined(DEFER_MACRO)
/* [GCC implementation] Must use C23 (depending on the platform) */
#  if defined(COMPILER_GCC)
#    define defer __DEFER(__COUNTER__)
#    define __DEFER(N) __DEFER_(N)
#    define __DEFER_(N) __DEFER__(__DEFER_FUNCTION_##N, __DEFER_VARIABLE_##N)
#    define __DEFER__(F, V)      \
      auto void F(int *);        \
      [[gnu::cleanup(F)]] int V; \
      auto void F(int *)

/* [Clang implementation] Must compile with flag `-fblocks` */
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

/* [MSVC implementation] */
#  elif defined(COMPILER_MSVC)
#    error "Not available yet in MSVC, use `_try/_finally`"
#  endif
#endif

/* --- INI Parser --- */
typedef struct {
  String key;
  String value;
} IniEntry;
VEC_TYPE(IniEntries, IniEntry);

typedef struct {
  IniEntries entries;
  Arena *source_file_arena;
} IniFile;
RESULT_TYPE(IniParseResult, IniFile);
WARN_UNUSED IniParseResult IniParse(String path);

WARN_UNUSED Error IniWrite(String path, IniFile *ini_file);

void IniFree(IniFile *ini_file);

String IniGet(IniFile *ini_file, String key);
String IniSet(IniFile *ini_file, String key, String value);
int32_t IniGetInt(IniFile *ini_file, String key);
int64_t IniGetLong(IniFile *ini_file, String key);
float64_t IniGetDouble(IniFile *ini_file, String key);
bool IniGetBool(IniFile *ini_file, String key);

/* MIT License
   base.h - Implementation of base.h
   https://github.com/TomasBorquez/base.h
*/
#if defined(BASE_IMPLEMENTATION)
/* --- Vector Implementation --- */
int32_t __base_vec_partition(void **data, size_t element_size, CompareFunc compare, int32_t low, int32_t high) {
  void *pivot = (char *)(*data) + (high * element_size);
  int32_t i = low - 1;
  for (int32_t j = low; j < high; j++) {
    void *current = (char *)(*data) + (j * element_size);
    if (compare(current, pivot) < 0) {
      i++;
      void *temp = (char *)(*data) + (i * element_size);
      char *temp_buffer = Malloc(element_size);
      memcpy(temp_buffer, temp, element_size);
      memcpy(temp, current, element_size);
      memcpy(current, temp_buffer, element_size);
      Free(temp_buffer);
    }
  }
  i++;
  void *temp = (char *)(*data) + (i * element_size);
  char *temp_buffer = Malloc(element_size);
  memcpy(temp_buffer, temp, element_size);
  memcpy(temp, pivot, element_size);
  memcpy(pivot, temp_buffer, element_size);
  Free(temp_buffer);
  return i;
}

void __base_vec_quicksort(void **data, size_t element_size, CompareFunc compare, int32_t low, int32_t high) {
  if (low < high) {
    int32_t pi = __base_vec_partition(data, element_size, compare, low, high);
    __base_vec_quicksort(data, element_size, compare, low, pi - 1);
    __base_vec_quicksort(data, element_size, compare, pi + 1, high);
  }
}

void __base_vec_push(void **data, size_t *length, size_t *capacity, size_t element_size, void *value) {
  // WARNING: Vector must always be initialized to zero `Vector vector = {0}`
  Assert(*length <= *capacity, "VecPush: Possible memory corruption or vector not initialized, `Vector vector = {0}`");
  Assert(!(*length > 0 && *data == NULL), "VecPush: Possible memory corruption, data should be NULL only if length == 0");

  if (*length >= *capacity) {
    if (*capacity == 0) *capacity = 128;
    else *capacity *= 2;

    *data = Realloc(*data, *capacity * element_size);
  }

  void *address = (char *)(*data) + (*length * element_size);
  memcpy(address, value, element_size);

  (*length)++;
}

void *__base_vec_pop(void *data, size_t *length, size_t element_size) {
  Assert(*length > 0, "VecPop: Cannot pop from empty vector");
  (*length)--;
  return (char *)data + (*length * element_size);
}

void __base_vec_shift(void **data, size_t *length, size_t element_size) {
  Assert(*length != 0, "VecShift: Length should at least be >= 1");
  memmove(*data, (char *)(*data) + element_size, ((*length) - 1) * element_size);
  (*length)--;
}

void __base_vec_unshift(void **data, size_t *length, size_t *capacity, size_t element_size, const void *value) {
  if (*length >= *capacity) {
    if (*capacity == 0) *capacity = 2;
    else *capacity *= 2;
    *data = Realloc(*data, *capacity * element_size);
  }

  if (*length > 0) {
    memmove((char *)(*data) + element_size, *data, (*length) * element_size);
  }

  memcpy(*data, value, element_size);
  (*length)++;
}

void __base_vec_insert(void **data, size_t *length, size_t *capacity, size_t element_size, void *value, size_t index) {
  Assert(index <= *length, "VecInsert: Index out of bounds for insertion");

  if (*length >= *capacity) {
    if (*capacity == 0) *capacity = 2;
    else *capacity *= 2;
    *data = Realloc(*data, *capacity * element_size);
  }

  if (index < *length) {
    memmove((char *)(*data) + ((index + 1) * element_size), (char *)(*data) + (index * element_size), (*length - index) * element_size);
  }

  memcpy((char *)(*data) + (index * element_size), value, element_size);
  (*length)++;
}

void *__base_vec_at(void **data, size_t *length, size_t index, size_t elementSize) {
  Assert(index < *length, "VecAt: Index out of bounds");
  void *address = (char *)(*data) + (index * elementSize);
  return address;
}

void __base_vec_free(void **data, size_t *length, size_t *capacity) {
  free(*data);
  *data = NULL;
  *length = 0;
  *capacity = 0;
}

/* --- Time and Platforms Implementation --- */

#  if !defined(PLATFORM_WIN)
WARN_UNUSED errno_t memcpy_s(void *dest, size_t destSize, const void *src, size_t count);

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
#  endif

bool isLinux(void) {
#  if defined(PLATFORM_LINUX)
  return true;
#  else
  return false;
#  endif
}

bool isMacOs(void) {
#  if defined(PLATFORM_MACOS)
  return true;
#  else
  return false;
#  endif
}

bool isWindows(void) {
#  if defined(PLATFORM_WIN)
  return true;
#  else
  return false;
#  endif
}

bool isUnix(void) {
#  if defined(PLATFORM_UNIX)
  return true;
#  else
  return false;
#  endif
}

bool isAndroid(void) {
#  if defined(PLATFORM_EMSCRIPTEN)
  return true;
#  else
  return false;
#  endif
}

bool isEmscripten(void) {
#  if defined(PLATFORM_EMSCRIPTEN)
  return true;
#  else
  return false;
#  endif
}

bool isFreeBSD(void) {
#  if defined(PLATFORM_FREEBSD)
  return true;
#  else
  return false;
#  endif
}

Compiler GetCompiler(void) {
#  if defined(COMPILER_CLANG)
  return CLANG;
#  elif defined(COMPILER_GCC)
  return GCC;
#  elif defined(COMPILER_TCC)
  return TCC;
#  elif defined(COMPILER_MSVC)
  return MSVC;
#  endif
}

Platform GetPlatform(void) {
#  if defined(PLATFORM_WIN)
  return WINDOWS;
#  elif defined(PLATFORM_LINUX)
  return LINUX;
#  elif defined(PLATFORM_MACOS)
  return MACOS;
#  elif defined(PLATFORM_FREEBSD)
  return FREEBSD;
#  endif
}

int64_t TimeNow(void) {
#  if defined(PLATFORM_WIN)
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  LARGE_INTEGER li;
  li.LowPart = ft.dwLowDateTime;
  li.HighPart = ft.dwHighDateTime;
  // Convert Windows FILETIME (100-nanosecond intervals since January 1, 1601)
  // to UNIX timestamp in milliseconds
  int64_t currentTime = (li.QuadPart - 116444736000000000LL) / 10000;
#  else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  int64_t currentTime = (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
#  endif
  Assert(currentTime != -1, "TimeNow: currentTime should never be -1");
  return currentTime;
}

void WaitTime(int64_t ms) {
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
#  if defined(PLATFORM_WIN)
Error ErrnoMatch(errno_t err) {
  switch (err) {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:      return FILE_NOT_FOUND;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:         return FILE_NO_MEMORY;
    case ERROR_ACCESS_DENIED:       return FILE_ACCESS_DENIED;
    case ERROR_DISK_FULL:           return FILE_DISK_FULL;
    case ERROR_ALREADY_EXISTS:      return FILE_ALREADY_EXISTS;
    case ERROR_TOO_MANY_OPEN_FILES: return FILE_TOO_MANY_OPEN;
    case ERROR_BUFFER_OVERFLOW:     return FILE_PATH_TOO_LONG;
    case ERROR_DIRECTORY:           return FILE_IS_DIRECTORY;
    case ERROR_WRITE_PROTECT:       return FILE_READ_ONLY_FS;
    default:                        return FILE_ACCESS_DENIED;
  }
}
#else
Error ErrnoMatch(errno_t err) {
  if (err == SUCCESS) return SUCCESS;
  switch (err) {
    case ENOENT:       return FILE_NOT_FOUND;
    case ENOMEM:       return FILE_NO_MEMORY;
    case ENOSPC:       return FILE_DISK_FULL;
    case EMFILE:       return FILE_TOO_MANY_OPEN;
    case ENFILE:       return FILE_TOO_MANY_OPEN;
    case EACCES:       return FILE_ACCESS_DENIED;
    case ENAMETOOLONG: return FILE_PATH_TOO_LONG;
    case EISDIR:       return FILE_IS_DIRECTORY;
    case EROFS:        return FILE_READ_ONLY_FS;
    case EEXIST:       return FILE_ALREADY_EXISTS;
  }
  return FILE_ACCESS_DENIED;
}
#endif

String ErrToStr(errno_t err) {
  switch (err) {
    case FILE_ACCESS_DENIED: return S("File access denied");
    case FILE_NOT_FOUND: return S("File not found");
    case FILE_NO_MEMORY: return S("File no memory");
    case FILE_DISK_FULL: return S("File disk full");
    case FILE_READ_FAILED: return S("File read Failed");
    case FILE_WRITE_FAILED: return S("File write failed");
    case FILE_TOO_MANY_OPEN: return S("File too many open");
    case FILE_PATH_TOO_LONG: return S("File path too long");
    case FILE_IS_DIRECTORY: return S("File is directory");
    case FILE_READ_ONLY_FS: return S("File read only FS");
    case FILE_ALREADY_EXISTS: return S("File already exists");
  }

  return S("");
}

static void _custom_assert(const char *expr, const char *file, unsigned line, const char *format, ...) {
  printf("%sAssertion failed: %s, file %s, line %u %s\n", _RED, expr, file, line, _RESET);

  if (format) {
    va_list args;
    va_start(args, format);
    logErrorV(format, args);
    va_start(args, format);
  }

  abort();
}

static void _custom_unreachable(const char *file, unsigned line, const char *format, ...) {
  printf("%sReached Unreachable code at file %s, line %u %s\n", _RED, file, line, _RESET);

  if (format) {
    va_list args;
    va_start(args, format);
    logErrorV(format, args);
    va_start(args, format);
  }

  abort();
}

/* --- Arena Implementation --- */
// Allocate or iterate to next chunk that can fit `bytes`
static void __ArenaNextChunk(Arena *arena, size_t bytes) {
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

void *ArenaAlloc(Arena *arena, const size_t size) {
  return ArenaAllocAligned(arena, size, DEFAULT_ALIGNMENT);
}

char *ArenaAllocChars(Arena *arena, size_t count) {
  return (char *)ArenaAllocAligned(arena, count, 1);
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

void ArenaFree(Arena *arena) {
  __ArenaChunk *chunk = arena->root;
  while (chunk) {
    __ArenaChunk *next = chunk->next;
    Free(chunk);
    chunk = next;
  }
  Free(arena);
}

void ArenaReset(Arena *arena) {
  arena->current = arena->root;
  arena->offset = 0;
}

Arena *ArenaCreate(size_t chunkSize) {
  Arena *res = Malloc(sizeof(Arena));
  memset(res, 0, sizeof(*res));
  res->chunkSize = chunkSize;
  __ArenaNextChunk(res, chunkSize);
  res->root = res->current;
  return res;
}

/* --- Memory Allocations --- */
void *Malloc(size_t size) {
  Assert(size != 0, "Malloc: size cant be zero");

  void *address = malloc(size);
  Assert(address != NULL, "Malloc: failed, returned address should never be NULL");
  return address;
}

void *Realloc(void *block, size_t size) {
  Assert(size != 0, "Realloc: size cant be zero");

  void *address = realloc(block, size);
  Assert(address != NULL, "Realloc: failed, returned address should never be NULL");
  return address;
}

void Free(void *address) {
  free(address);
}

/* --- String Implementation --- */
static size_t max_string_size = 10000;
String s(char *msg) {
  if (msg == NULL) {
    return (String){0};
  }

  return (String){
    .length = strlen(msg),
    .data = msg,
  };
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

static size_t str_len(char *str, size_t maxSize) {
  if (str == NULL) {
    return 0;
  }

  size_t len = 0;
  while (len < maxSize && str[len] != '\0') {
    len++;
  }

  return len;
}

static void add_null_terminator(char *str, size_t len) {
  str[len] = '\0';
}

void SetMaxStrSize(size_t size) {
  max_string_size = size;
}

String StrNew(Arena *arena, char *str) {
  const size_t len = str_len(str, max_string_size);
  if (len == 0) {
    return (String){0};
  }
  size_t memory_size = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocated_str = ArenaAllocChars(arena, memory_size);

  memcpy(allocated_str, str, memory_size);
  add_null_terminator(allocated_str, len);
  return (String){len, allocated_str};
}

String StrNewSize(Arena *arena, char *str, size_t len) {
  Assert(str != NULL, "StrNewSize: str should never be NULL");

  size_t memory_size = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocated_str = ArenaAllocChars(arena, memory_size);

  memcpy(allocated_str, str, len);
  add_null_terminator(allocated_str, len);
  return (String){len, allocated_str};
}

void StrCopy(String *destination, String source) {
  Assert(!StrIsNull(*destination), "StrCopy: destination should never be NULL");
  Assert(!StrIsNull(source), "StrCopy: source should never be NULL");
  Assert(destination->length >= source.length, "destination length should never smaller than source length");

  errno_t err = memcpy_s(destination->data, destination->length + 1, source.data, source.length);
  Assert(err == SUCCESS, "StrCopy: memcpy_s failed, err: %d", err);

  destination->length = source.length;
  add_null_terminator(destination->data, destination->length);
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

String StrConcat(Arena *arena, String string1, String string2) {
  if (StrIsNull(string1)) {
    const size_t len = string2.length;
    const size_t memorySize = sizeof(char) * len + 1;
    char *allocatedString = ArenaAllocChars(arena, memorySize);

    errno_t err = memcpy_s(allocatedString, memorySize, string2.data, string2.length);
    Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

    add_null_terminator(allocatedString, len);
    return (String){len, allocatedString};
  }

  if (StrIsNull(string2)) {
    const size_t len = string1.length;
    const size_t memorySize = sizeof(char) * len + 1;
    char *allocatedString = ArenaAllocChars(arena, memorySize);
    errno_t err = memcpy_s(allocatedString, memorySize, string1.data, string1.length);
    Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

    add_null_terminator(allocatedString, len);
    return (String){len, allocatedString};
  }

  const size_t len = string1.length + string2.length;
  const size_t memorySize = sizeof(char) * len + 1;
  char *allocatedString = ArenaAllocChars(arena, memorySize);

  errno_t err = memcpy_s(allocatedString, memorySize, string1.data, string1.length);
  Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

  err = memcpy_s(allocatedString + string1.length, memorySize, string2.data, string2.length);
  Assert(err == SUCCESS, "StrConcat: memcpy_s failed, err: %d", err);

  add_null_terminator(allocatedString, len);
  return (String){len, allocatedString};
}

StringVector StrSplit(Arena *arena, String str, String delimiter) {
  Assert(!StrIsNull(str), "StrSplit: str should never be NULL");
  Assert(!StrIsNull(delimiter), "StrSplit: delimiter should never be NULL");

  char *start = str.data;
  char *end = str.data + str.length;
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

void StrToUpper(String str) {
  for (size_t i = 0; i < str.length; ++i) {
    char curr_char = str.data[i];
    str.data[i] = toupper(curr_char);
  }
}

void StrToLower(String str) {
  for (size_t i = 0; i < str.length; ++i) {
    char curr_char = str.data[i];
    str.data[i] = tolower(curr_char);
  }
}

bool StrIsNull(String str) {
  return str.data == NULL;
}

static bool is_space(char character) {
  return character == ' ' || character == '\n' || character == '\t' || character == '\r';
}

void StrTrim(String *str) {
  if (str->length == 0) {
    return;
  }

  char *first_char = NULL;
  char *last_char = NULL;
  for (size_t i = 0; i < str->length; ++i) {
    char *curr_char = &str->data[i];
    if (is_space(*curr_char)) {
      continue;
    }

    if (first_char == NULL) {
      first_char = curr_char;
    } else {
      last_char = curr_char;
    }
  }

  if (first_char == NULL || last_char == NULL) {
    str->data[0] = '\0';
    str->length = 0;
    add_null_terminator(str->data, 0);
    return;
  }

  str->length = (last_char - first_char) + 1;
  memmove(str->data, first_char, str->length);
  add_null_terminator(str->data, str->length);
}

String StrSlice(Arena *arena, String str, size_t start, ssize_t end) {
  Assert(start <= str.length, "StrSlice: start index out of bounds");

  if (end < 0) {
    end = str.length + end;
  }

  Assert(end >= (ssize_t)start, "StrSlice: end must be greater than or equal to start: start=%lu, end=%lu", start, end);
  Assert(end <= (ssize_t)str.length, "StrSlice: end index out of bounds");

  size_t len = end - start;
  return StrNewSize(arena, str.data + start, len);
}

bool StrIncludes(String source, String subStr) {
  Assert(!StrIsNull(source), "StrIncludes: source should never be NULL");
  Assert(!StrIsNull(subStr), "StrIncludes: subStr should never be NULL");

  if (source.length == 0 || subStr.length == 0 || subStr.length > source.length) {
    return false;
  }

  for (size_t i = 0; i <= source.length - subStr.length; i++) {
    if (source.data[i] != subStr.data[0]) {
      continue;
    }

    if (memcmp(&source.data[i], subStr.data, subStr.length) == 0) {
      return true;
    }
  }

  return false;
}

static String normSlashes(String path) {
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
  Platform platform = GetPlatform();
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

  if (platform == WINDOWS) {
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

String NormalizeExtension(Arena *arena, String path) {
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  size_t filenameStart = 0;
  for (size_t i = 0; i < result.length; i++) {
    if (result.data[i] == '/' || result.data[i] == '\\') {
      filenameStart = i + 1;
    }
  }

  size_t lastDotIndex = 0;
  for (size_t i = 0; i < result.length; i++) {
    if (result.data[i] == '.') {
      lastDotIndex = i;
    }
  }

  if (lastDotIndex <= filenameStart) {
    return normSlashes(result);
  }

  result = StrSlice(arena, result, filenameStart, lastDotIndex);
  return normSlashes(result);
}

String NormalizeStaticLibPath(Arena *arena, String path) {
  Platform platform = GetPlatform();
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

  if (platform == WINDOWS) {
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

String NormalizePathEnd(Arena *arena, String path) {
  size_t lastSlashIndex = 0;
  for (size_t i = 0; i < path.length; i++) {
    if (path.data[i] == '/' || path.data[i] == '\\') {
      lastSlashIndex = i + 1;
    }
  }

  return StrNewSize(arena, path.data + lastSlashIndex, path.length - lastSlashIndex);
}

StringBuilder SBCreate(Arena *arena) {
  StringBuilder result = {0};
  char *data = ArenaAllocChars(arena, 128);

  result.arena_builder = arena;
  result.capacity = 128;
  result.buffer = (String){.data = data, .length = 0};
  return result;
}

StringBuilder SBReserve(Arena *arena, size_t capacity) {
  StringBuilder result = {0};
  char *data = ArenaAllocChars(arena, capacity);

  result.arena_builder = arena;
  result.capacity = capacity;
  result.buffer = (String){.data = data, .length = 0};
  return result;
}

void SBAdd(StringBuilder *builder, String string) {
  size_t newLength = builder->buffer.length + string.length;
  if (newLength + 1 >= builder->capacity) {
    size_t newCapacity = (newLength + 1) * 2;
    char *data = ArenaAllocChars(builder->arena_builder, newCapacity);

    memcpy(data, builder->buffer.data, builder->buffer.length);
    builder->buffer.data = data;
    builder->capacity = newCapacity;
  }

  memcpy(builder->buffer.data + builder->buffer.length, string.data, string.length);
  builder->buffer.length = newLength;
  builder->buffer.data[builder->buffer.length] = '\0';
}

static String string_from_u64(char *buf, size_t buff_size, uint64_t value) {
  char *end = buf + buff_size;
  char *p = end;
  *--p = '\0';

  if (value == 0) {
    *--p = '0';
  } else {
    while (value > 0) {
      *--p = '0' + (int32_t)(value % 10);
      value /= 10;
    }
  }

  return (String){.data = p, .length = (size_t)(end - p - 1)};
}

static String string_from_i64(char *buf, size_t buff_size, int64_t value) {
  int32_t neg = value < 0;
  uint64_t abs = neg ? -(uint64_t)value : (uint64_t)value;

  char *end = buf + buff_size;
  char *p = end;
  *--p = '\0';

  if (abs == 0) {
    *--p = '0';
  } else {
    while (abs > 0) {
      *--p = '0' + (int)(abs % 10);
      abs /= 10;
    }
  }
  if (neg) *--p = '-';

  return (String){.data = p, .length = (size_t)(end - p - 1)};
}

/* Format specifiers:
*  %s  — char * (null terminated)
*  %S  — String
*  %d  — i32
*  %l  — i64
*  %ud — u32
*  %ul — u64
*/
void SBAddFormatV(StringBuilder *builder, char *fmt, va_list args) {
  char *cursor = fmt;

  while (*cursor) {
    if (*cursor != '%') {
      char *start = cursor;
      while (*cursor && *cursor != '%')
        cursor++;

      SBAdd(builder, (String){.data = (char *)start, .length = (size_t)(cursor - start)});
      continue;
    }
    cursor++;

    bool is_unsigned = false;
    if (*cursor == 'u') {
      is_unsigned = true;
      cursor++;
    }

    char tmp[32];
    String segment = {0};

    switch (*cursor++) {
    case 's': {
      Assert(!is_unsigned, "%%us is not a valid specifier");
      char *str = va_arg(args, char *);
      Assert(str != NULL, "str should never be NULL");
      segment = (String){.data = (char *)str, .length = strlen(str)};
    } break;
    case 'S': {
      Assert(!is_unsigned, "%%uS is not a valid specifier");
      segment = va_arg(args, String);
      Assert(!StrIsNull(segment), "String should never be NULL");
    } break;
    case 'd': {
      if (is_unsigned) {
        uint32_t v = va_arg(args, uint32_t);
        segment = string_from_u64(tmp, sizeof(tmp), (uint64_t)v);
      } else {
        int32_t v = va_arg(args, int32_t);
        segment = string_from_i64(tmp, sizeof(tmp), (int64_t)v);
      }
    } break;
    case 'l': {
      if (is_unsigned) {
        uint64_t v = va_arg(args, uint64_t);
        segment = string_from_u64(tmp, sizeof(tmp), (uint64_t)v);
      } else {
        int64_t v = va_arg(args, int64_t);
        segment = string_from_i64(tmp, sizeof(tmp), v);
      }
    } break;
    case '%': {
      Assert(!is_unsigned, "%%u%% is not a valid specifier");
      segment = (String){.data = "%", .length = 1};
    } break;
    default:
      Unreachable("SBAddFormatV: unknown format specifier");
    }

    if (segment.length > 0) {
      SBAdd(builder, segment);
    }
  }
}

void SBAddF(StringBuilder *builder, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    SBAddFormatV(builder, fmt, args);
    va_end(args);
}

/* --- Random Implemenation --- */
static uint64_t seed = 0;

uint64_t RandomGetSeed(void) {
  return seed;
}

void RandomSetSeed(uint64_t newSeed) {
  seed = newSeed;
  srand(seed);
}

int32_t RandomInteger(int32_t min, int32_t max) {
  Assert(min <= max, "RandomInteger: min should always be less than or equal to max");
  Assert(max - min <= INT32_MAX - 1, "RandomInteger: range too large");

  int32_t range = max - min + 1;

  // calculate scaling factor to avoid modulo bias
  uint32_t buckets = RAND_MAX / range;
  uint32_t limit = buckets * range;

  // reject numbers that would create bias
  uint32_t r;
  do {
    r = rand();
  } while (r >= limit);

  return min + (r / buckets);
}

float32_t RandomFloat(float32_t min, float32_t max) {
  Assert(min <= max, "RandomFloat: min must be less than or equal to max");
  float32_t normalized = rand() / (float32_t)RAND_MAX;
  return min + normalized * (max - min);
}

/* --- File System Implementation --- */
#  if defined(PLATFORM_WIN)
static char curr_path[MAX_PATH];
GetCwdResult GetCwd(void) {
  GetCwdResult result = {0};
  DWORD length = GetCurrentDirectory(MAX_PATH, curr_path);
  if (length == 0) {
    result.error = ErrnoMatch(GetLastError());
    result.data = S("");
    return result;
  }

  result.data = (String){.data = curr_path, .length = (size_t)length};
  return result;
}

Error SetCwd(char *destination) {
  if (!SetCurrentDirectory(destination)) {
    return ErrnoMatch(GetLastError());
  }
  return GetCwd().error;
}

FileStatsResult FileStats(String path) {
  FileStatsResult result = {0};
  WIN32_FILE_ATTRIBUTE_DATA attr = {0};
  if (!GetFileAttributesExA(path.data, GetFileExInfoStandard, &attr)) {
    result.error = ErrnoMatch(GetLastError());
    return result;
  }

  char *name_start = strrchr(path.data, '\\');
  if (!name_start) name_start = strrchr(path.data, '/');
  char *name = name_start ? name_start + 1 : path.data;
  result.data.name = name;
  char *ext = strrchr(name, '.');
  result.data.extension = ext ? ext + 1 : "";

  LARGE_INTEGER size, ctime, mtime;
  size.HighPart = attr.nFileSizeHigh;
  size.LowPart = attr.nFileSizeLow;
  ctime.HighPart = attr.ftCreationTime.dwHighDateTime;
  ctime.LowPart = attr.ftCreationTime.dwLowDateTime;
  mtime.HighPart = attr.ftLastWriteTime.dwHighDateTime;
  mtime.LowPart = attr.ftLastWriteTime.dwLowDateTime;

  const int64_t TICKS_PER_SEC = 10000000LL;
  const int64_t EPOCH_DELTA_SECS = 11644473600LL;
  result.data.size = (size_t)size.QuadPart;
  result.data.createTime = ctime.QuadPart / TICKS_PER_SEC - EPOCH_DELTA_SECS;
  result.data.modifyTime = mtime.QuadPart / TICKS_PER_SEC - EPOCH_DELTA_SECS;
  return result;
}

FileReadResult FileRead(Arena *arena, String path, size_t file_size) {
  FileReadResult result = {0};
  HANDLE hFile = CreateFileA(path.data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    result.error = ErrnoMatch(GetLastError());
    return result;
  }

  char *buffer = ArenaAllocChars(arena, file_size + 1);
  DWORD bytes_read;
  if (!ReadFile(hFile, buffer, (DWORD)file_size, &bytes_read, NULL)) {
    result.error = ErrnoMatch(GetLastError());
  } else if ((size_t)bytes_read != file_size) {
    result.error = FILE_READ_FAILED; // short read
  } else {
    buffer[bytes_read] = '\0';
    result.data = (String){.data = buffer, .length = (size_t)bytes_read};
  }
  CloseHandle(hFile);
  return result;
}

Error FileWrite(String path, String data) {
  HANDLE hFile = CreateFileA(path.data, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) return ErrnoMatch(GetLastError());

  DWORD bytes_written;
  Error err = SUCCESS;
  if (!WriteFile(hFile, data.data, (DWORD)data.length, &bytes_written, NULL)) err = ErrnoMatch(GetLastError());
  else if ((size_t)bytes_written != data.length) err = FILE_WRITE_FAILED;
  CloseHandle(hFile);
  return err;
}

Error FileAdd(String path, String data) {
  HANDLE hFile = CreateFileA(path.data, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) return ErrnoMatch(GetLastError());
  if (SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
    CloseHandle(hFile);
    return FILE_ACCESS_DENIED;
  }

  DWORD bytes_written;
  Error err = SUCCESS;
  if (!WriteFile(hFile, data.data, (DWORD)data.length, &bytes_written, NULL)) err = ErrnoMatch(GetLastError());
  else if ((size_t)bytes_written != data.length) err = FILE_WRITE_FAILED;
  CloseHandle(hFile);
  return err;
}

Error FileDelete(String path) {
  if (!DeleteFileA(path.data)) return ErrnoMatch(GetLastError());
  return SUCCESS;
}

Error FileRename(String old_path, String new_path) {
  if (!MoveFileExA(old_path.data, new_path.data, MOVEFILE_REPLACE_EXISTING)) return ErrnoMatch(GetLastError());
  return SUCCESS;
}

Error Mkdir(String path) {
  if (CreateDirectoryA(path.data, NULL)) return SUCCESS;
  DWORD err = GetLastError();
  if (err == ERROR_ALREADY_EXISTS) return SUCCESS;
  return ErrnoMatch(err);
}

ListDirResult ListDir(Arena *arena, String path) {
  ListDirResult result = {0};
  char search[MAX_PATH];
  snprintf(search, MAX_PATH, "%s\\*", path.data);

  WIN32_FIND_DATAA find_data;
  HANDLE hFind = FindFirstFileA(search, &find_data);
  if (hFind == INVALID_HANDLE_VALUE) {
    result.error = ErrnoMatch(GetLastError());
    return result;
  }

  do {
    if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) continue;
    String entry = StrNew(arena, find_data.cFileName);
    VecPush(result.data, entry);
  } while (FindNextFileA(hFind, &find_data));
  DWORD err = GetLastError();

  if (err != ERROR_NO_MORE_FILES) result.error = ErrnoMatch(err);
  FindClose(hFind);
  return result;
}

Error FileCopy(String src_path, String dest_path) {
  HANDLE src = CreateFileA(src_path.data, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (src == INVALID_HANDLE_VALUE) return ErrnoMatch(GetLastError());

  LARGE_INTEGER size;
  if (!GetFileSizeEx(src, &size)) {
    CloseHandle(src);
    return ErrnoMatch(GetLastError());
  }

  HANDLE dest = CreateFileA(dest_path.data, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (dest == INVALID_HANDLE_VALUE) {
    DWORD err = GetLastError();
    CloseHandle(src);
    return ErrnoMatch(err);
  }

  DWORD open_err = GetLastError();

  char buffer[8192];
  DWORD bytes_read, bytes_written;
  Error err = SUCCESS;
  while (ReadFile(src, buffer, sizeof(buffer), &bytes_read, NULL) && bytes_read > 0) {
    if (!WriteFile(dest, buffer, bytes_read, &bytes_written, NULL)) {
      err = ErrnoMatch(GetLastError());
      break;
    } else if (bytes_written != bytes_read) {
      err = FILE_WRITE_FAILED;
      break;
    }
  }

  if (err == SUCCESS) {
    DWORD loop_err = GetLastError();
    if (loop_err != ERROR_SUCCESS &&
        loop_err != ERROR_HANDLE_EOF &&
        loop_err != ERROR_ALREADY_EXISTS &&
        loop_err != open_err) {
      err = ErrnoMatch(loop_err);
    }
  }

  CloseHandle(src);
  CloseHandle(dest);

  if (err != SUCCESS) DeleteFileA(dest_path.data);
  return err;
}
#  else
static char curr_path[PATH_MAX];
GetCwdResult GetCwd(void) {
  GetCwdResult result = {0};
  if (getcwd(curr_path, sizeof(curr_path)) == NULL) {
    result.error = ErrnoMatch(errno);
    result.data = S("");
    return result;
  }

  result.data = (String) {.data = curr_path, .length = strnlen(curr_path, PATH_MAX)};
  return result;
}

Error SetCwd(char *destination) {
  if (chdir(destination) != SUCCESS) {
    return ErrnoMatch(errno);
  }
  return GetCwd().error;
}

FileStatsResult FileStats(String path) {
  FileStatsResult result = {0};

  struct stat file_stat;
  if (stat(path.data, &file_stat) != SUCCESS) {
    result.error = ErrnoMatch(errno);
    return result;
  }

  char *name_start = strrchr(path.data, '/');
  char *name = name_start ? name_start + 1 : path.data;
  result.data.name = name;
  char *extension_start = strrchr(name, '.');
  if (extension_start) {
    result.data.extension = extension_start + 1;
  }

  result.data.size = file_stat.st_size;
  result.data.createTime = file_stat.st_ctime;
  result.data.modifyTime = file_stat.st_mtime;
  return result;
}

FileReadResult FileRead(Arena *arena, String path, size_t file_size) {
  FileReadResult result = {0};
  int fd = open(path.data, O_RDONLY);
  if (fd < 0) {
    result.error = ErrnoMatch(errno);
    return result;
  }

  char *buffer = ArenaAllocChars(arena, file_size + 1);
  ssize_t bytes_read = read(fd, buffer, file_size);
  if (bytes_read < 0) {
    result.error = ErrnoMatch(errno);
  } else if ((size_t)bytes_read != file_size) {
    result.error = FILE_READ_FAILED;
  } else {
    buffer[bytes_read] = '\0';
    result.data = (String){.length = bytes_read, .data = buffer};
  }
  close(fd);
  return result;
}

Error FileWrite(String path, String data) {
  int fd = open(path.data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    return ErrnoMatch(errno);
  }

  Error err = SUCCESS;
  ssize_t bytes_written = write(fd, data.data, data.length);
  if (bytes_written < 0) err = ErrnoMatch(errno);
  else if ((size_t)bytes_written != data.length) err = FILE_WRITE_FAILED;

  close(fd);
  return err;
}

Error FileAdd(String path, String data) {
  int fd = open(path.data, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if (fd < 0) {
    return ErrnoMatch(errno);
  }

  Error err = SUCCESS;
  ssize_t bytes_written = write(fd, data.data, data.length);
  if (bytes_written < 0)                         err = ErrnoMatch(errno);
  else if ((size_t)bytes_written != data.length) err = FILE_WRITE_FAILED;

  close(fd);
  return err;
}

Error FileDelete(String path) {
  if (unlink(path.data) != SUCCESS) return ErrnoMatch(errno);
  return SUCCESS;
}

Error FileRename(String old_path, String new_path) {
  if (rename(old_path.data, new_path.data) != SUCCESS) return ErrnoMatch(errno);
  return SUCCESS;
}

Error Mkdir(String path) {
  if (mkdir(path.data, 0755) == SUCCESS) return SUCCESS;

  if (errno == EEXIST) {
    struct stat st;
    if (stat(path.data, &st) == SUCCESS && S_ISDIR(st.st_mode)) return SUCCESS;
  }
  return ErrnoMatch(errno);
}

ListDirResult ListDir(Arena *arena, String path) {
  ListDirResult result = {0};
  DIR *dir = opendir(path.data);

  if (dir == NULL) {
    result.error = ErrnoMatch(errno);
    return result;
  }

  errno = 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    String entryStr = StrNew(arena, entry->d_name);
    VecPush(result.data, entryStr);
  }

  if (errno != SUCCESS) {
    result.error = ErrnoMatch(errno);
  }

  closedir(dir);
  return result;
}

Error FileCopy(String sourcePath, String destPath) {
  Error err = SUCCESS;
  int src_fd, dest_fd;

  src_fd = open(sourcePath.data, O_RDONLY);
  if (src_fd < 0) {
    return ErrnoMatch(errno);
  }

  struct stat src_stat;
  if (fstat(src_fd, &src_stat) != SUCCESS) {
    close(src_fd);
    return ErrnoMatch(errno);
  }

  dest_fd = open(destPath.data, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode & 0777);
  if (dest_fd < 0) {
    close(src_fd);
    return ErrnoMatch(errno);
  }

  char buffer[8192];
  ssize_t bytes_read, bytes_written;
  while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
    bytes_written = write(dest_fd, buffer, bytes_read);
    if (bytes_written < 0) {
      err = ErrnoMatch(errno);
      break;
    } else if (bytes_written != bytes_read) {
      err = FILE_WRITE_FAILED;
      break;
    }
  }

  if (bytes_read < 0) err = ErrnoMatch(errno);

  close(src_fd);
  close(dest_fd);

  if (err != SUCCESS) {
    unlink(destPath.data);
  }

  return err;
}
#  endif

/* --- Logger Implemenation --- */
void LogInit(void) {
#  if defined(PLATFORM_WIN)
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#  endif
}

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

void LogSuccess(const char *format, ...) {
  printf("%s[SUCCESS]: ", _GREEN);
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

void logErrorV(const char *format, va_list args) {
  printf("%s[ERROR]: ", _RED);
  vprintf(format, args);
  printf("%s\n", _RESET);
}

/* --- INI Parser Implementation --- */
IniParseResult IniParse(String path) {
  IniParseResult result = {0};
  FileStatsResult stats = FileStats(path);
  if (stats.error == FILE_NOT_FOUND) { // create file
    result.error = FileWrite(path, S(""));
    return result;
  }

  if (stats.error != SUCCESS || stats.data.size == 0) {
    result.error = stats.error;
    return result;
  }

  Arena *arena = ArenaCreate(stats.data.size + 1);
  FileReadResult ini_file = FileRead(arena, path, stats.data.size);
  if (ini_file.error != SUCCESS) {
    result.error = ini_file.error;
    return result;
  }

  size_t start_pos = 0, equal_pos = 0, newline_pos = 0;
  String buffer = ini_file.data;
  for (size_t i = 0; i < buffer.length; i++) {
    char curr_char = buffer.data[i];
    if (curr_char == '=') {
      equal_pos = i;
      continue;
    }

    if (curr_char == '\n' || i == buffer.length - 1) {
      if (equal_pos == 0) { // empty line case
        start_pos = i + 1;
        continue;
      }

      if (buffer.data[start_pos] == ';') { // comment case
        start_pos = i + 1;
        equal_pos = 0;
        continue;
      }

      newline_pos = i;
      {
        String key = {.data = buffer.data + start_pos, .length = equal_pos - start_pos};
        key.data[key.length] = '\0';

        String value = { .data = (buffer.data) + (equal_pos + 1), .length = newline_pos - (equal_pos + 1) };
        value.data[value.length] = '\0';

        IniEntry entry = {.key = key, .value = value};
        VecPush(result.data.entries, entry);
      }
      equal_pos = 0;
      start_pos = i + 1;
      continue;
    }
  }

  result.data.source_file_arena = arena;
  return result;
}

Error IniWrite(String path, IniFile *ini_file) {
  Error err = FileWrite(path, S(""));
  if (err != SUCCESS) {
    return err;
  }

  VecForEach(ini_file->entries, entry) {
    size_t total_size = entry->key.length + entry->value.length + 3; // for =, '\n' and '\0'
    char *entry_str = Malloc(total_size);

    memcpy(entry_str, entry->key.data, entry->key.length);
    entry_str[entry->key.length] = '=';
    memcpy(entry_str + (entry->key.length + 1), entry->value.data, entry->value.length);
    entry_str[total_size - 2] = '\n';
    entry_str[total_size - 1] = '\0';

    err = FileAdd(path, (String) { .data = entry_str, .length = total_size - 1});
    Free(entry_str);

    if (err) return err;
  }

  return SUCCESS;
}

void IniFree(IniFile *ini_file) {
  VecFree(ini_file->entries);
  if (ini_file->source_file_arena != NULL) { // only if we have a source file
    ArenaFree(ini_file->source_file_arena);
  }
}

String IniGet(IniFile *ini_file, String key) {
  VecForEach(ini_file->entries, entry) {
    if (StrEq(entry->key, key)) {
      return entry->value;
    }
  }

  return (String){0};
}

String IniSet(IniFile *ini_file, String key, String value) {
  VecForEach(ini_file->entries, entry) {
    if (StrEq(entry->key, key)) {
      entry->value = value;
      return entry->value;
    }
  }

  IniEntry new_entry = { .key = key, .value = value};
  VecPush(ini_file->entries, new_entry);
  return new_entry.value;
}

int32_t IniGetInt(IniFile *ini_file, String key) {
  String value = IniGet(ini_file, key);
  if (StrIsNull(value)) {
    return 0;
  }

  char *end_ptr;
  int32_t result = (int32_t)strtol(value.data, &end_ptr, 10);
  if (end_ptr == value.data) {
    LogWarn("IniGetLong: Failed to convert [key: %s, value: %s] to int", key.data, value.data);
    return 0;
  }

  return result;
}

int64_t IniGetLong(IniFile *ini_file, String key) {
  String value = IniGet(ini_file, key);
  if (StrIsNull(value)) {
    return 0;
  }

  char *endPtr;
  int64_t result = strtoll(value.data, &endPtr, 10);
  if (endPtr == value.data) {
    LogWarn("IniGetLong: Failed to convert [key: %s, value: %s] to long", key.data, value.data);
    return 0;
  }

  return result;
}

float64_t IniGetDouble(IniFile *ini_file, String key) {
  String value = IniGet(ini_file, key);
  if (StrIsNull(value)) {
    return 0.0;
  }

  char *endPtr;
  float64_t result = strtod(value.data, &endPtr);
  if (endPtr == value.data) {
    LogWarn("IniGetLong: Failed to convert [key: %s, value: %s] to double", key.data, value.data);
    return 0.0;
  }

  return result;
}

bool IniGetBool(IniFile *ini_file, String key) {
  String value = IniGet(ini_file, key);
  if (StrIsNull(value)) {
    return false;
  }

  return StrEq(value, S("true"));
}
#endif
