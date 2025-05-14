#include "test.c"

void TestForEach() {
  TEST_BEGIN("StringVectorForEach");
  {
    StringVector vec = {0};
    VecForEach(vec, str) {
      TEST_ASSERT(false, "Empty vector iteration");
    }

    TEST_ASSERT(true, "Empty vector does not iterate");

    VecPush(vec, S("1"));
    VecPush(vec, S("2"));
    VecPush(vec, S("3"));

    TEST_ASSERT(vec.length == 3, "Vector has 3 elements");

    size_t count = 0;
    VecForEach(vec, str) {
      count++;
    }

    TEST_ASSERT(count == 3, "Vector iteration count matches length");

    VecFree(vec);
    TEST_ASSERT(true, "Vector freed");
  }
  TEST_END();
}

void TestPushPop() {
  TEST_BEGIN("VectorPushPop");
  {
    StringVector vec = {0};
    TEST_ASSERT(vec.length == 0, "Initial vector is empty");

    VecPush(vec, S("hello"));
    TEST_ASSERT(vec.length == 1, "Length increases after push");
    TEST_ASSERT(StrEq(vec.data[0], S("hello")), "Pushed value is stored correctly");

    VecPush(vec, S("world"));
    TEST_ASSERT(vec.length == 2, "Length increases after second push");
    TEST_ASSERT(StrEq(vec.data[1], S("world")), "Second pushed value is stored correctly");

    String *popped = VecPop(vec);
    TEST_ASSERT(vec.length == 1, "Length decreases after pop");
    TEST_ASSERT(StrEq(*popped, S("world")), "Popped value is correct");

    popped = VecPop(vec);
    TEST_ASSERT(vec.length == 0, "Vector is empty after popping all elements");
    TEST_ASSERT(StrEq(*popped, S("hello")), "Last popped value is correct");

    VecFree(vec);
  }
  TEST_END();
}

void TestUnshiftShift() {
  TEST_BEGIN("VectorUnshiftShift");
  {
    StringVector vec = {0};

    String first = S("first");
    VecUnshift(vec, first);
    TEST_ASSERT(vec.length == 1, "Length increases after unshift");
    TEST_ASSERT(StrEq(VecAt(vec, 0), S("first")), "Unshifted value is stored correctly");

    String zero = S("zero");
    VecUnshift(vec, zero);
    TEST_ASSERT(vec.length == 2, "Length increases after second unshift");
    TEST_ASSERT(StrEq(VecAt(vec, 0), S("zero")), "Second unshifted value is at index 0");
    TEST_ASSERT(StrEq(VecAt(vec, 1), S("first")), "Previous value is now at index 1");

    String shifted = VecAt(vec, 0);
    VecShift(vec);
    TEST_ASSERT(vec.length == 1, "Length decreases after shift");
    TEST_ASSERT(StrEq(shifted, S("zero")), "Shifted value is correct");
    TEST_ASSERT(StrEq(VecAt(vec, 0), S("first")), "Remaining value is correct");

    VecFree(vec);
  }
  TEST_END();
}

void TestInsert() {
  TEST_BEGIN("VectorInsert");
  {
    StringVector vec = {0};

    String end = S("end");
    VecInsert(vec, end, 0);
    TEST_ASSERT(vec.length == 1, "Length increases after insert to empty vector");
    TEST_ASSERT(StrEq(vec.data[0], S("end")), "Inserted value is stored correctly");

    String start = S("start");
    VecInsert(vec, start, 0);
    TEST_ASSERT(vec.length == 2, "Length increases after insert at beginning");
    TEST_ASSERT(StrEq(vec.data[0], S("start")), "Value inserted at beginning is correct");
    TEST_ASSERT(StrEq(vec.data[1], S("end")), "Existing value shifted correctly");

    String middle = S("middle");
    VecInsert(vec, middle, 1);
    TEST_ASSERT(vec.length == 3, "Length increases after insert in middle");
    TEST_ASSERT(StrEq(vec.data[0], S("start")), "First value unchanged");
    TEST_ASSERT(StrEq(vec.data[1], S("middle")), "Middle value inserted correctly");
    TEST_ASSERT(StrEq(vec.data[2], S("end")), "Last value shifted correctly");

    VecFree(vec);
  }
  TEST_END();
}

void TestAccess() {
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

void TestCapacity() {
  TEST_BEGIN("VectorCapacity");
  {
    StringVector vec = {0};
    TEST_ASSERT(vec.capacity == 0, "Initial capacity is zero");

    VecPush(vec, S("first"));
    TEST_ASSERT(vec.capacity == 128, "First push sets capacity to 128");

    StringVector vec2 = {0};
    VecReserve(vec2, 10);
    TEST_ASSERT(vec2.capacity == 10, "VecReserve sets correct initial capacity");

    StringVector vec3 = {0};
    for (size_t i = 0; i < 200; i++) {
      VecPush(vec3, S("item"));
    }
    TEST_ASSERT(vec3.capacity >= 200, "Capacity grows to accommodate elements");

    VecFree(vec);
    VecFree(vec2);
    VecFree(vec3);
  }
  TEST_END();
}

i32 main() {
  StartTest();
  {
    TestForEach();
    TestPushPop();
    TestUnshiftShift();
    TestInsert();
    TestAccess();
    TestCapacity();
  }
  EndTest();
}
