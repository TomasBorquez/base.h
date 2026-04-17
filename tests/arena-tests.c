#include "test-framework.c"

static void TestBasicArena(void) {
  TEST_BEGIN("Basic Arena Test");
  {
    Arena *a = ArenaCreate(1024);
    TEST_ASSERT(a != NULL, "Arena created");
    uintptr_t ptr1 = (uintptr_t)ArenaAlloc(a, 1);
    uintptr_t ptr2 = (uintptr_t)ArenaAlloc(a, 1);
    TEST_ASSERT(ptr2 - ptr1 == DEFAULT_ALIGNMENT, "Arena alignment");
    uintptr_t ptr3 = (uintptr_t)ArenaAllocChars(a, 2);
    TEST_ASSERT(ptr3 - ptr2 == 1, "Arena char alignment");
    void *large_ptr = ArenaAllocChars(a, 4000);
    TEST_ASSERT(large_ptr != NULL, "Large allocation successful");

    ArenaFree(a);
  }
  TEST_END();
}

static void TestArenaChunkExpansion(void) {
  TEST_BEGIN("Arena Chunk Expansion Test");
  {
    Arena *a = ArenaCreate(128);
    TEST_ASSERT(a != NULL, "Arena created with small chunk size");

    void *ptr1 = ArenaAlloc(a, 64);
    void *ptr2 = ArenaAlloc(a, 64);
    void *ptr3 = ArenaAlloc(a, 64);
    TEST_ASSERT(ptr1 != NULL, "First allocation successful");
    TEST_ASSERT(ptr2 != NULL, "Second allocation successful");
    TEST_ASSERT(ptr3 != NULL, "Third allocation (should be in new chunk) successful");

    void *large_ptr = ArenaAlloc(a, 1024);
    TEST_ASSERT(large_ptr != NULL, "Large allocation successful");

    ArenaFree(a);
  }
  TEST_END();
}

static void TestArenaAlignedAllocation(void) {
  TEST_BEGIN("Arena Aligned Allocation Test");
  {
    Arena *a = ArenaCreate(512);
    TEST_ASSERT(a != NULL, "Arena created");

    void *misalign_ptr = ArenaAllocChars(a, 7);
    TEST_ASSERT(misalign_ptr != NULL, "Misalignment allocation successful");

    size_t alignment = 64; // Test with 64-byte alignment
    void *aligned_ptr = ArenaAllocAligned(a, 32, alignment);
    TEST_ASSERT(aligned_ptr != NULL, "Aligned allocation successful");
    TEST_ASSERT(((uintptr_t)aligned_ptr % alignment) == 0, "Pointer is properly aligned");

    alignment = 128;
    void *aligned_ptr2 = ArenaAllocAligned(a, 32, alignment);
    TEST_ASSERT(aligned_ptr2 != NULL, "Second aligned allocation successful");
    TEST_ASSERT(((uintptr_t)aligned_ptr2 % alignment) == 0, "Second pointer is properly aligned");

    ArenaFree(a);
  }
  TEST_END();
}

static void TestArenaReset(void) {
  TEST_BEGIN("Arena Reset Test");
  {
    Arena *a = ArenaCreate(256);
    TEST_ASSERT(a != NULL, "Arena created");

    void *ptr1 = ArenaAlloc(a, 64);
    void *ptr2 = ArenaAlloc(a, 64);
    void *ptr3 = ArenaAlloc(a, 64);
    TEST_ASSERT(ptr1 != NULL && ptr2 != NULL && ptr3 != NULL, "Initial allocations successful");

    ArenaReset(a);
    void *ptr1_after = ArenaAlloc(a, 64);
    void *ptr2_after = ArenaAlloc(a, 64);
    void *ptr3_after = ArenaAlloc(a, 64);

    TEST_ASSERT(ptr1_after != NULL && ptr2_after != NULL && ptr3_after != NULL, "Post-reset allocations successful");
    TEST_ASSERT(ptr1 == ptr1_after, "Reset returned to the beginning of the arena");

    ArenaFree(a);
  }
  TEST_END();
}

static void TestArenaStress(void) {
  TEST_BEGIN("Arena Stress Test");
  {
    Arena *a = ArenaCreate(1024);
    TEST_ASSERT(a != NULL, "Arena created");

    void *ptrs[1000];
    for (int i = 0; i < 1000; i++) {
      ptrs[i] = ArenaAlloc(a, (i % 32) + 1); // Allocate varying sizes from 1 to 32 bytes
      TEST_ASSERT(ptrs[i] != NULL, "Small allocation successful");
    }

    void *large_ptrs[10];
    for (int i = 0; i < 10; i++) {
      large_ptrs[i] = ArenaAlloc(a, 2048); // Much larger than chunk size
      TEST_ASSERT(large_ptrs[i] != NULL, "Large allocation successful");
    }

    ArenaReset(a);
    for (int i = 0; i < 1000; i++) {
      ptrs[i] = ArenaAlloc(a, (i % 32) + 1);
      TEST_ASSERT(ptrs[i] != NULL, "Post-reset small allocation successful");
    }

    ArenaFree(a);
  }
  TEST_END();
}

static void TestZeroSizeAllocations(void) {
  TEST_BEGIN("Zero Size Allocations Test");
  {
    Arena *a = ArenaCreate(256);
    TEST_ASSERT(a != NULL, "Arena created");

    void *zero_ptr1 = ArenaAlloc(a, 0);
    void *zero_ptr2 = ArenaAlloc(a, 0);

    TEST_ASSERT(zero_ptr1 != NULL, "Zero-size allocation returns non-NULL");
    TEST_ASSERT(zero_ptr2 != NULL, "Second zero-size allocation returns non-NULL");

    void *realPtr = ArenaAlloc(a, 64);
    TEST_ASSERT(realPtr != NULL, "Normal allocation after zero-size allocations works");

    ArenaFree(a);
  }
  TEST_END();
}

static void TestEdgeCaseChunkSizes(void) {
  TEST_BEGIN("Edge Case Chunk Sizes Test");
  {
    Arena *tiny = ArenaCreate(16);
    TEST_ASSERT(tiny != NULL, "Tiny arena created");
    void *tiny_ptr = ArenaAlloc(tiny, 8);
    TEST_ASSERT(tiny_ptr != NULL, "Allocation in tiny arena");
    ArenaFree(tiny);

    Arena *huge = ArenaCreate(1024 * 1024 * 10); // 10MB chunks
    TEST_ASSERT(huge != NULL, "Huge arena created");
    void *huge_ptr = ArenaAlloc(huge, 1024 * 1024); // 1MB allocation
    TEST_ASSERT(huge_ptr != NULL, "Large allocation in huge arena");
    ArenaFree(huge);
  }
  TEST_END();
}

int main(void) {
  StartTest();
  {
    TestBasicArena();
    TestArenaChunkExpansion();
    TestArenaAlignedAllocation();
    TestArenaReset();
    TestArenaStress();
    TestZeroSizeAllocations();
    TestEdgeCaseChunkSizes();
  }
  EndTest();
}
