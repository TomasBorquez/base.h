#include "test-framework.c"

static int compare_int(const void *a, const void *b) {
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia - *ib;
}

static void TestForEach(void) {
  TEST_BEGIN("StringVectorForEach");
  {
    StringVector vec = {0};
    VecForEach(vec, str) {
      TEST_ASSERT(false, "empty vector iteration");
    }

    TEST_ASSERT(true, "empty vector does not iterate");

    VecPush(vec, S("1"));
    VecPush(vec, S("2"));
    VecPush(vec, S("3"));

    TEST_ASSERT(vec.length == 3, "vector has 3 elements");

    size_t count = 0;
    VecForEach(vec, str) {
      count++;
    }

    TEST_ASSERT(count == 3, "vector iteration count matches length");

    VecFree(vec);
    TEST_ASSERT(true, "vector freed");
  }
  TEST_END();
}

static void TestPushPop(void) {
  TEST_BEGIN("VectorPushPop");
  {
    StringVector vec = {0};
    TEST_ASSERT(vec.length == 0, "initial vector is empty");

    VecPush(vec, S("hello"));
    TEST_ASSERT(vec.length == 1, "length increases after push");
    TEST_ASSERT(StrEq(vec.data[0], S("hello")), "pushed value is stored correctly");

    VecPush(vec, S("world"));
    TEST_ASSERT(vec.length == 2, "length increases after second push");
    TEST_ASSERT(StrEq(vec.data[1], S("world")), "second pushed value is stored correctly");

    String *popped = VecPop(vec);
    TEST_ASSERT(vec.length == 1, "length decreases after pop");
    TEST_ASSERT(StrEq(*popped, S("world")), "popped value is correct");

    popped = VecPop(vec);
    TEST_ASSERT(vec.length == 0, "vector is empty after popping all elements");
    TEST_ASSERT(StrEq(*popped, S("hello")), "last popped value is correct");

    VecFree(vec);
  }
  TEST_END();
}

static void TestUnshiftShift(void) {
  TEST_BEGIN("VectorUnshiftShift");
  {
    StringVector vec = {0};

    String first = S("first");
    VecUnshift(vec, first);
    TEST_ASSERT(vec.length == 1, "length increases after unshift");
    TEST_ASSERT(StrEq(VecAt(vec, 0), S("first")), "unshifted value is stored correctly");

    String zero = S("zero");
    VecUnshift(vec, zero);
    TEST_ASSERT(vec.length == 2, "length increases after second unshift");
    TEST_ASSERT(StrEq(VecAt(vec, 0), S("zero")), "second unshifted value is at index 0");
    TEST_ASSERT(StrEq(VecAt(vec, 1), S("first")), "previous value is now at index 1");

    String shifted = VecAt(vec, 0);
    VecShift(vec);
    TEST_ASSERT(vec.length == 1, "length decreases after shift");
    TEST_ASSERT(StrEq(shifted, S("zero")), "shifted value is correct");
    TEST_ASSERT(StrEq(VecAt(vec, 0), S("first")), "remaining value is correct");

    VecFree(vec);
  }
  TEST_END();
}

static void TestInsert(void) {
  TEST_BEGIN("VectorInsert");
  {
    StringVector vec = {0};

    String end = S("end");
    VecInsert(vec, end, 0);
    TEST_ASSERT(vec.length == 1, "length increases after insert to empty vector");
    TEST_ASSERT(StrEq(vec.data[0], S("end")), "inserted value is stored correctly");

    String start = S("start");
    VecInsert(vec, start, 0);
    TEST_ASSERT(vec.length == 2, "length increases after insert at beginning");
    TEST_ASSERT(StrEq(vec.data[0], S("start")), "value inserted at beginning is correct");
    TEST_ASSERT(StrEq(vec.data[1], S("end")), "existing value shifted correctly");

    String middle = S("middle");
    VecInsert(vec, middle, 1);
    TEST_ASSERT(vec.length == 3, "length increases after insert in middle");
    TEST_ASSERT(StrEq(vec.data[0], S("start")), "first value unchanged");
    TEST_ASSERT(StrEq(vec.data[1], S("middle")), "middle value inserted correctly");
    TEST_ASSERT(StrEq(vec.data[2], S("end")), "last value shifted correctly");

    VecFree(vec);
  }
  TEST_END();
}

static void TestAccess(void) {
  TEST_BEGIN("VectorAccess");
  {
    StringVector vec = {0};
    VecPush(vec, S("first"));
    VecPush(vec, S("second"));
    VecPush(vec, S("third"));

    String *first = VecAtPtr(vec, 0);
    TEST_ASSERT(StrEq(*first, S("first")), "VecAt(0) returns first element");

    String second = VecAt(vec, 1);
    TEST_ASSERT(StrEq(second, S("second")), "VecAt(1) returns second element");

    String third = VecAt(vec, 2);
    TEST_ASSERT(StrEq(third, S("third")), "VecAt(2) returns third element");

    VecFree(vec);
  }
  TEST_END();
}

static void TestCapacity(void) {
  TEST_BEGIN("VectorCapacity");
  {
    StringVector vec = {0};
    TEST_ASSERT(vec.capacity == 0, "initial capacity is zero");

    VecPush(vec, S("first"));
    TEST_ASSERT(vec.capacity == 128, "first push sets capacity to 128");

    StringVector vec2 = {0};
    VecReserve(vec2, 10);
    TEST_ASSERT(vec2.capacity == 10, "VecReserve sets correct initial capacity");

    StringVector vec3 = {0};
    for (size_t i = 0; i < 200; i++) {
      VecPush(vec3, S("item"));
    }
    TEST_ASSERT(vec3.capacity >= 200, "capacity grows to accommodate elements");

    VecFree(vec);
    VecFree(vec2);
    VecFree(vec3);
  }
  TEST_END();
}

static void TestSort(void) {
  TEST_BEGIN("VectorSort");
  {
    VEC_TYPE(IntVector, int);
    IntVector numbers = {0};
    int32_t values[] = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); ++i) {
      VecPush(numbers, values[i]);
    }
    TEST_ASSERT(numbers.length == 9, "vector has 9 elements");

    VecSort(numbers, compare_int);
    bool is_sorted = true;
    for (size_t i = 1; i < numbers.length; i++) {
      if (numbers.data[i] < numbers.data[i-1]) {
        is_sorted = false;
        break;
      }
    }
    TEST_ASSERT(is_sorted, "vector is correctly sorted");
    VecFree(numbers);
  }
  TEST_END();
}

int main(void) {
  StartTest();
  {
    TestForEach();
    TestPushPop();
    TestUnshiftShift();
    TestInsert();
    TestAccess();
    TestCapacity();
    TestSort();
  }
  EndTest();
}
