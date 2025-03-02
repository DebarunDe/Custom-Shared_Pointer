#include "SharedPtr.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include <memory>

/* TestCases are designed to follow the functionality of std::shared_ptr and cross checking results with SharedPtr
* The following test cases are covered:
* 1. Default constructor
* 2. Constructor with pointer
* 3. Copy constructor
* 4. Copy assignment operator
* 5. Move constructor
* 6. Move assignment operator
* 7. Destructor
* 8. Stress test
* 9. nullptr assignment
* 10. Self assignment
* 11. Reset
* 12. Reset with new pointer
* 13. Multiple resets
* 14. Copying into nullptr
* 15. Scope deletion
* 16. Concurrent access
* 17. Concurrent modification
* 18. Concurrent copy and assignment
* 19. Large number of threads
* 20. Scope deletion with threads
*/

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
    std::shared_ptr<TestObject> sp2;
    assert(sp.get() == sp2.get());
    assert(sp.getCount() == sp2.use_count());
    std::cout << "testDefaultConstructor passed!" << std::endl;
}

void testConstructorWithPointer() {
    SharedPtr<TestObject> sp(new TestObject(10));
    std::shared_ptr<TestObject> sp2(new TestObject(10));
    assert(sp2.get() != nullptr);
    assert(sp2.get()->value == 10);
    assert(sp.getCount() == sp2.use_count());
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 10);
    assert(sp.getCount() == 1);
    std::cout << "testConstructorWithPointer passed!" << std::endl;
}

void testCopyConstructor() {
    SharedPtr<TestObject> sp1(new TestObject(20));
    SharedPtr<TestObject> sp2(sp1);
    std::shared_ptr<TestObject> sp3(new TestObject(20));
    std::shared_ptr<TestObject> sp4(sp3);
    assert(sp3.get() == sp4.get());
    assert(sp3.use_count() == sp4.use_count());
    assert(sp3.use_count() == 2);
    assert(sp4.use_count() == 2);
    assert(sp1.get() == sp2.get());
    assert(sp1.getCount() == 2);
    assert(sp2.getCount() == 2);
    std::cout << "testCopyConstructor passed!" << std::endl;
}

void testCopyAssignmentOperator() {
    SharedPtr<TestObject> sp1(new TestObject(30));
    SharedPtr<TestObject> sp2;
    sp2 = sp1;
    std::shared_ptr<TestObject> sp3(new TestObject(30));
    std::shared_ptr<TestObject> sp4;
    sp4 = sp3;
    assert(sp3.get() == sp4.get());
    assert(sp3.use_count() == sp4.use_count());
    assert(sp3.use_count() == 2);
    assert(sp4.use_count() == 2);
    assert(sp1.get() == sp2.get());
    assert(sp1.getCount() == 2);
    assert(sp2.getCount() == 2);
    std::cout << "testCopyAssignmentOperator passed!" << std::endl;
}

void testMoveConstructor() {
    SharedPtr<TestObject> sp1(new TestObject(40));
    SharedPtr<TestObject> sp2(std::move(sp1));
    std::shared_ptr<TestObject> sp3(new TestObject(40));
    std::shared_ptr<TestObject> sp4(std::move(sp3));
    assert(sp3.get() == nullptr);
    assert(sp4.get() != nullptr);
    assert(sp4.get()->value == 40);
    assert(sp4.use_count() == 1);
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
    std::shared_ptr<TestObject> sp3(new TestObject(50));
    std::shared_ptr<TestObject> sp4;
    sp4 = std::move(sp3);
    assert(sp3.get() == nullptr);
    assert(sp4.get() != nullptr);
    assert(sp4.get()->value == 50);
    assert(sp4.use_count() == 1);
    assert(sp1.get() == nullptr);
    assert(sp2.get() != nullptr);
    assert(sp2.get()->value == 50);
    assert(sp2.getCount() == 1);
    std::cout << "testMoveAssignmentOperator passed!" << std::endl;
}

void testDestructor() {
    SharedPtr<TestObject> sp1(new TestObject(60));
    std::shared_ptr<TestObject> sp5(new TestObject(60));
    {
        SharedPtr<TestObject> sp2(sp1);
        std::shared_ptr<TestObject> sp6(sp5);
        assert(sp1.getCount() == 2);
        assert(sp5.use_count() == 2);
    }
    assert(sp1.getCount() == 1);
    assert(sp5.use_count() == 1);

    SharedPtr<TestObject> sp3(nullptr);
    std::shared_ptr<TestObject> sp7(nullptr);
    assert(sp3.getCount() == 0);
    {
        SharedPtr<TestObject> sp4(sp3);
        std::shared_ptr<TestObject> sp8(sp7);
        assert(sp4.getCount() == 0);
        assert(sp8.use_count() == 0);
    }
    assert(sp3.getCount() == 0);
    assert(sp7.use_count() == 0);
    std::cout << "testDestructor passed!" << std::endl;
}

void testStress() {
    const int numObjects = 100000;
    std::vector<SharedPtr<TestObject>> spArray(numObjects);
    std::vector<std::shared_ptr<TestObject>> spArray2(numObjects);

    // Test creating a large number of SharedPtr objects
    for (int i = 0; i < numObjects; ++i) {
        spArray[i] = SharedPtr<TestObject>(new TestObject(i));
        spArray2[i] = std::make_shared<TestObject>(i);
    }

    // Test accessing and verifying the values
    for (int i = 0; i < numObjects; ++i) {
        assert(spArray[i].get() != nullptr);
        assert(spArray[i].get()->value == i);
        assert(spArray[i].getCount() == 1);
        assert(spArray2[i].get() != nullptr);
        assert(spArray2[i].get()->value == i);
        assert(spArray2[i].use_count() == 1);
    }

    // Test copying SharedPtr objects
    for (int i = 0; i < numObjects; ++i) {
        SharedPtr<TestObject> spCopy = spArray[i];
        std::shared_ptr<TestObject> spCopy2 = spArray2[i];
        assert(spCopy.get() == spArray[i].get());
        assert(spCopy.getCount() == 2);
        assert(spCopy2.get() == spArray2[i].get());
        assert(spCopy2.use_count() == 2);
    }

    // Test moving SharedPtr objects
    for (int i = 0; i < numObjects; ++i) {
        SharedPtr<TestObject> spMove = std::move(spArray[i]);
        std::shared_ptr<TestObject> spMove2 = std::move(spArray2[i]);
        assert(spArray[i].get() == nullptr);
        assert(spMove.get() != nullptr);
        assert(spMove.get()->value == i);
        assert(spMove.getCount() == 1);
        assert(spArray2[i].get() == nullptr);
        assert(spMove2.get() != nullptr);
        assert(spMove2.get()->value == i);
        assert(spMove2.use_count() == 1);
    }

    std::cout << "testStress passed!" << std::endl;
}

void testNullptrAssignment() {
    SharedPtr<TestObject> sp1(new TestObject(70));
    SharedPtr<TestObject> sp2;
    sp2 = nullptr;
    std::shared_ptr<TestObject> sp4(new TestObject(70));
    std::shared_ptr<TestObject> sp5;
    assert(sp2.get() == nullptr);
    assert(sp2.getCount() == 0);
    assert(sp1.get() != nullptr);
    assert(sp1.getCount() == 1);
    assert(sp5.get() == nullptr);
    assert(sp5.use_count() == 0);
    assert(sp4.get() != nullptr);
    assert(sp4.use_count() == 1);

    SharedPtr<TestObject> sp3(nullptr);
    sp3 = sp1;
    std::shared_ptr<TestObject> sp6(nullptr);
    sp6 = sp4;
    assert(sp3.get() == sp1.get());
    assert(sp3.getCount() == 2);
    assert(sp1.getCount() == 2);
    assert(sp6.get() == sp4.get());
    assert(sp6.use_count() == 2);
    assert(sp4.use_count() == 2);

    sp1 = sp2;
    sp4 = sp5;
    assert(sp1.get() == nullptr);
    assert(sp1.getCount() == 0);
    assert(sp2.getCount() == 0);
    assert(sp3.get() != sp1.get());
    assert(sp3.getCount() == 1);
    assert(sp4.get() == nullptr);
    assert(sp4.use_count() == 0);
    assert(sp5.use_count() == 0);
    assert(sp6.get() != sp4.get());
    assert(sp6.use_count() == 1);
    std::cout << "testNullptrAssignment passed!" << std::endl;
}

void testSelfAssignment() {
    SharedPtr<TestObject> sp(new TestObject(80));
    sp = sp;
    std::shared_ptr<TestObject> sp2(new TestObject(80));
    sp2 = sp2;
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 80);
    assert(sp.getCount() == 1);
    assert(sp2.get() != nullptr);
    assert(sp2.get()->value == 80);
    assert(sp2.use_count() == 1);
    std::cout << "testSelfAssignment passed!" << std::endl;
}

void testReset() {
    SharedPtr<TestObject> sp(new TestObject(90));
    sp.reset();
    std::shared_ptr<TestObject> sp2(new TestObject(90));
    sp2.reset();
    assert(sp.get() == nullptr);
    assert(sp.getCount() == 0);
    assert(sp2.get() == nullptr);
    assert(sp2.use_count() == 0);
    std::cout << "testReset passed!" << std::endl;
}

void testResetWithNewPointer() {
    SharedPtr<TestObject> sp(new TestObject(100));
    sp.reset(new TestObject(110));
    std::shared_ptr<TestObject> sp2(new TestObject(100));
    sp2.reset(new TestObject(110));
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 110);
    assert(sp.getCount() == 1);
    assert(sp2.get() != nullptr);
    assert(sp2.get()->value == 110);
    assert(sp2.use_count() == 1);
    std::cout << "testResetWithNewPointer passed!" << std::endl;
}

void testMultipleResets() {
    SharedPtr<TestObject> sp(new TestObject(120));
    sp.reset(new TestObject(130));
    sp.reset(new TestObject(140));
    std::shared_ptr<TestObject> sp2(new TestObject(120));
    sp2.reset(new TestObject(130));
    sp2.reset(new TestObject(140));
    assert(sp.get() != nullptr);
    assert(sp.get()->value == 140);
    assert(sp.getCount() == 1);
    assert(sp2.get() != nullptr);
    assert(sp2.get()->value == 140);
    assert(sp2.use_count() == 1);
    std::cout << "testMultipleResets passed!" << std::endl;
}

void testCopyingIntoNullptr() {
    SharedPtr<TestObject> sp1(new TestObject(150));
    SharedPtr<TestObject> sp2(nullptr);
    sp1 = sp2;
    std::shared_ptr<TestObject> sp3(new TestObject(150));
    std::shared_ptr<TestObject> sp4(nullptr);
    sp3 = sp4;
    assert(sp1.get() == sp2.get());
    assert(sp1.getCount() == 0);
    assert(sp2.getCount() == 0);
    assert(sp3.get() == sp4.get());
    assert(sp3.use_count() == 0);
    assert(sp4.use_count() == 0);
    std::cout << "testCopyingIntoNullptr passed!" << std::endl;
}

void testScopeDeletion() {
    TestObject::deleted = false;
    {
        SharedPtr<TestObject> sp(new TestObject(160));
        sp->value = 160;
    }
    assert(TestObject::deleted);

    TestObject::deleted = false;
    {
        std::shared_ptr<TestObject> sp(new TestObject(160));
        sp->value = 160;
    }
    assert(TestObject::deleted);
    std::cout << "testScopeDeletion passed!" << std::endl;
}

void testConcurrentAccess() {
    SharedPtr<TestObject> sp(new TestObject(170));
    const int numThreads = 10;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&sp]() {
            for (int j = 0; j < 1000; ++j) {
                assert(sp.get() != nullptr);
                assert(sp.get()->value == 170);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::shared_ptr<TestObject> sp2(new TestObject(170));
    const int numThreads2 = 10;
    std::vector<std::thread> threads2;

    for (int i = 0; i < numThreads2; ++i) {
        threads2.emplace_back([&sp2]() {
            for (int j = 0; j < 1000; ++j) {
                assert(sp2.get() != nullptr);
                assert(sp2.get()->value == 170);
            }
        });
    }

    for (auto& thread2 : threads2) {
        thread2.join();
    }

    std::cout << "testConcurrentAccess passed!" << std::endl;
}

void testConcurrentModification() {
    SharedPtr<TestObject> sp(new TestObject(180));
    const int numThreads = 10;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&sp]() {
            for (int j = 0; j < 1000; ++j) {
                sp.reset(new TestObject(180 + j));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "testConcurrentModification passed!" << std::endl;
}

void testConcurrentCopyAndAssignment() {
    SharedPtr<TestObject> sp1(new TestObject(190));
    const int numThreads = 10;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&sp1]() {
            for (int j = 0; j < 1000; ++j) {
                SharedPtr<TestObject> sp2(sp1);
                SharedPtr<TestObject> sp3;
                sp3 = sp1;
                assert(sp2.get() == sp1.get());
                assert(sp3.get() == sp1.get());
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "testConcurrentCopyAndAssignment passed!" << std::endl;
}

void testLargeNumberOfThreads() {
    SharedPtr<TestObject> sp(new TestObject(200));
    const int numThreads = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&sp]() {
            for (int j = 0; j < 1000; ++j) {
                SharedPtr<TestObject> spCopy(sp);
                assert(spCopy.get() == sp.get());
                assert(spCopy.get()->value == 200);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
    std::cout << "testLargeNumberOfThreads passed!" << std::endl;
}

void testScopeDeletionWithThreads() {
    const int numThreads = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([]() {
            SharedPtr<TestObject> sp(new TestObject(160));
            sp->value = 160;
            assert(sp.get() != nullptr);
            assert(sp.get()->value == 160);
            assert(sp.getCount() == 1);
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    assert(TestObject::deleted);
    std::cout << "testScopeDeletionWithThreads passed!" << std::endl;
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
    testConcurrentAccess();
    testConcurrentModification();
    testConcurrentCopyAndAssignment();
    testLargeNumberOfThreads();
    testScopeDeletionWithThreads();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}