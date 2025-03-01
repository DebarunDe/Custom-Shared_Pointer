#include "SharedPtr.h"
#include <iostream>
#include <cassert>

class TestObject {
public:
    int value;
    static bool deleted;
    TestObject(int val) : value(val) {}
    ~TestObject() {
        deleted = true;
    }
};

bool TestObject::deleted = false;

void testDefaultConstructor() {
    SharedPtr<TestObject> sp;
    assert(sp.get() == nullptr);
    assert(sp.getCount() == 0);
    std::cout << "testDefaultConstructor passed!" << std::endl;
}

void testConstructorWithPointer() {
    SharedPtr<TestObject> sp(new TestObject(10));
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 10);
    assert(sp.getCount() == 1);
    std::cout << "testConstructorWithPointer passed!" << std::endl;
}

void testCopyConstructor() {
    SharedPtr<TestObject> sp1(new TestObject(20));
    SharedPtr<TestObject> sp2(sp1);
    assert(sp1.get() == sp2.get());
    assert(sp1.getCount() == 2);
    assert(sp2.getCount() == 2);
    std::cout << "testCopyConstructor passed!" << std::endl;
}

void testCopyAssignmentOperator() {
    SharedPtr<TestObject> sp1(new TestObject(30));
    SharedPtr<TestObject> sp2;
    sp2 = sp1;
    assert(sp1.get() == sp2.get());
    assert(sp1.getCount() == 2);
    assert(sp2.getCount() == 2);
    std::cout << "testCopyAssignmentOperator passed!" << std::endl;
}

void testMoveConstructor() {
    SharedPtr<TestObject> sp1(new TestObject(40));
    SharedPtr<TestObject> sp2(std::move(sp1));
    assert(sp1.get() == nullptr);
    assert(sp2.get() != nullptr);
    assert(sp2.get()->value == 40);
    assert(sp2.getCount() == 1);
    std::cout << "testMoveConstructor passed!" << std::endl;
}

void testMoveAssignmentOperator() {
    SharedPtr<TestObject> sp1(new TestObject(50));
    SharedPtr<TestObject> sp2;
    sp2 = std::move(sp1);
    assert(sp1.get() == nullptr);
    assert(sp2.get() != nullptr);
    assert(sp2.get()->value == 50);
    assert(sp2.getCount() == 1);
    std::cout << "testMoveAssignmentOperator passed!" << std::endl;
}

void testDestructor() {
    SharedPtr<TestObject> sp1(new TestObject(60));
    {
        SharedPtr<TestObject> sp2(sp1);
        assert(sp1.getCount() == 2);
    }
    assert(sp1.getCount() == 1);

    SharedPtr<TestObject> sp3(nullptr);
    assert(sp3.getCount() == 0);
    {
        SharedPtr<TestObject> sp4(sp3);
        assert(sp4.getCount() == 0);
    }
    assert(sp3.getCount() == 0);
    std::cout << "testDestructor passed!" << std::endl;
}

void testStress() {
    const int numObjects = 100000;
    SharedPtr<TestObject> spArray[numObjects];

    // Test creating a large number of SharedPtr objects
    for (int i = 0; i < numObjects; ++i) {
        spArray[i] = SharedPtr<TestObject>(new TestObject(i));
    }

    // Test accessing and verifying the values
    for (int i = 0; i < numObjects; ++i) {
        assert(spArray[i].get() != nullptr);
        assert(spArray[i].get()->value == i);
        assert(spArray[i].getCount() == 1);
    }

    // Test copying SharedPtr objects
    for (int i = 0; i < numObjects; ++i) {
        SharedPtr<TestObject> spCopy = spArray[i];
        assert(spCopy.get() == spArray[i].get());
        assert(spCopy.getCount() == 2);
    }

    // Test moving SharedPtr objects
    for (int i = 0; i < numObjects; ++i) {
        SharedPtr<TestObject> spMove = std::move(spArray[i]);
        assert(spArray[i].get() == nullptr);
        assert(spMove.get() != nullptr);
        assert(spMove.get()->value == i);
        assert(spMove.getCount() == 1);
    }

    std::cout << "testStress passed!" << std::endl;
}

void testNullptrAssignment() {
    SharedPtr<TestObject> sp1(new TestObject(70));
    SharedPtr<TestObject> sp2;
    sp2 = nullptr;
    assert(sp2.get() == nullptr);
    assert(sp2.getCount() == 0);
    assert(sp1.get() != nullptr);
    assert(sp1.getCount() == 1);
    std::cout << "testNullptrAssignment passed!" << std::endl;
}

void testSelfAssignment() {
    SharedPtr<TestObject> sp(new TestObject(80));
    sp = sp;
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 80);
    assert(sp.getCount() == 1);
    std::cout << "testSelfAssignment passed!" << std::endl;
}

void testReset() {
    SharedPtr<TestObject> sp(new TestObject(90));
    sp.reset();
    assert(sp.get() == nullptr);
    assert(sp.getCount() == 0);
    std::cout << "testReset passed!" << std::endl;
}

void testResetWithNewPointer() {
    SharedPtr<TestObject> sp(new TestObject(100));
    sp.reset(new TestObject(110));
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 110);
    assert(sp.getCount() == 1);
    std::cout << "testResetWithNewPointer passed!" << std::endl;
}

void testMultipleResets() {
    SharedPtr<TestObject> sp(new TestObject(120));
    sp.reset(new TestObject(130));
    sp.reset(new TestObject(140));
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 140);
    assert(sp.getCount() == 1);
    std::cout << "testMultipleResets passed!" << std::endl;
}

void testCopyingIntoNullptr() {
    SharedPtr<TestObject> sp1(new TestObject(150));
    SharedPtr<TestObject> sp2(nullptr);
    sp1 = sp2;
    assert(sp1.get() == sp2.get());
    assert(sp1.getCount() == 0);
    assert(sp2.getCount() == 0);
    std::cout << "testCopyingIntoNullptr passed!" << std::endl;
}

void testScopeDeletion() {
    TestObject::deleted = false;
    {
        SharedPtr<TestObject> sp(new TestObject(160));
        sp->value = 160;
    }
    assert(TestObject::deleted);
    std::cout << "testScopeDeletion passed!" << std::endl;
}

int main() {
    testDefaultConstructor();
    testConstructorWithPointer();
    testCopyConstructor();
    testCopyAssignmentOperator();
    testMoveConstructor();
    testMoveAssignmentOperator();
    testDestructor();
    testStress();
    testNullptrAssignment();
    testSelfAssignment();
    testReset();
    testResetWithNewPointer();
    testMultipleResets();
    testCopyingIntoNullptr();
    testScopeDeletion();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}