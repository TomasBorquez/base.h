# About:
C lacks a proper standard library so I decided to create a proper one that makes it easier for me to get started on a project
and have the necessary default functions by default. 

Planned to be cross-platform and work on `windows` and `linux` as well as in many compilers like `gcc`, `clang`, `tcc` and `MSVC`.
This is still very much experimental and if used you should read `base.h` implementations and assume they might have bugs, I strongly
recommend to just use as inspiration or recreationally until version 1.0.

I will add features as I need them and fix stuff as I break it, once decently stable I'll make it as cross-platform as possible and add tests,
and once I make sure everything in stable I'll then finally launch 1.0.

## Features:
- `Defer` - It works similar to Go's defer it only works on `gcc` and `clang` with `-fblocks` flag (`MSVC` doesnt support this yet nor does `tcc`, and they probably wont until C23/C26+)
```c 
i32 *ptr = (i32 *) malloc(10 * sizeof(int));
defer {
  free(ptr);
}
// will execute at the end
```
- `Vector` - In here you have `VecPush`, `VecShift`, `VecUnshift`, etc. It's just a regular macro implementation.
- `Arenas` - Based on Ginger Bill's arena implemenation.
- `String` - Some basic string functions.
- `File System` - Some abstractions for both `windows` and `linux` for files.
- And more...

## Usage:

A good way to keep this updated is by using git submodules, you can add `base.h` like this:

```bash
git submodule add https://github.com/TomasBorquez/base.h.git ./vendor/base
```

This will add `base.h` to `./vendor`, then you can include it from there or add a `-I"./vendor/base"` flag when compiling and importing like:

```C
#define BASE_IMPLEMENTATION
#include "base.h"
```

And for keeping it updated you can:

```C 
git submodule update --init
```

# TODO:
- [x] Return `errno_t` and `LogError` rather than asserting
- [x] Add version at the top of the file and add proper license
- [ ] Create enums of errors:
    - [x] `GeneralError`
    - [x] `FileStatsError`
    - [x] `FileReadError`
    - [ ] `FileWriteError`
    - [ ] `FileDeleteError`
    - [ ] `FileRenameError`
    - [ ] `CreateCompileCommands`
- [ ] Add equivalent FileSystem implementation for linux
- [ ] Assert `Malloc` and `ArenaAlloc`, use `Malloc` and `Free`
- [ ] Make GNU extensions work in both MSVC and Clang (ex: Vectors)
- [ ] Test on `Clang`, `GCC` and `MSVC` on windows
- [ ] Test on `Clang` and `GCC` on linux
- [ ] Add `TCC` support
- [ ] Optimize String functions and add better names
