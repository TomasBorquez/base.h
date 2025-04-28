#define BASE_IMPLEMENTATION
#include "base.h"

static void TestVectors() {
    StringVector vec = {0};
    VecForEach(vec, str) {
        LogError("Should not happen!");
        exit(1);
    }
    VecPush(vec, S("1"));
    VecPush(vec, S("2"));
    VecPush(vec, S("3"));
    VecForEach(vec, str) {
        LogInfo("%s", str->data);
    }
    VecFree(vec);
}

static void TestArenas() {
    Arena a = {0};
    ArenaReserve(&a, 200);
    uintptr_t origBuffer = (uintptr_t)a.buffer;
    uintptr_t ptr1 = (uintptr_t)ArenaAlloc(&a, 1);
    uintptr_t ptr2 = (uintptr_t)ArenaAlloc(&a, 1);
    if (ptr2 - ptr1 != DEFAULT_ALIGNMENT) {
        LogError("Arena alignment fail");
        exit(1);
    }
    uintptr_t ptr3 = (uintptr_t)ArenaAllocChars(&a, 2);
    if (ptr3 - ptr2 != 1) {
        LogError("Arena char alignment fail");
        exit(1);
    }
    ArenaAllocChars(&a, 4000);
    if (origBuffer == (uintptr_t)a.buffer) {
        LogError("Arena realloc fail");
        exit(1);
    }
    ArenaFree(&a);
}

int main() {
    TestVectors();
    TestArenas();
    LogInfo("Tests passed!");
}
