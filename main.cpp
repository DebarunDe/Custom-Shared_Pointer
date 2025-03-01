#include "SharedPtr.h"
#include <iostream>
#include <cassert>

class TestObject {
public:
    int value;
    TestObject(int val) : value(val) {}
};

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
    std::cout << "testDestructor passed!" << std::endl;
}

int main() {
    testDefaultConstructor();
    testConstructorWithPointer();
    testCopyConstructor();
    testCopyAssignmentOperator();
    testMoveConstructor();
    testMoveAssignmentOperator();
    testDestructor();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}