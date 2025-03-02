#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include <mutex>
#include <atomic>
#include <utility>

template <typename T>
class SharedPtr {
    private:
        T* ptr;
        //thread safe count and access guards
        std::atomic<unsigned int>* count;
        mutable std::mutex mtx;
    public:
        //default constructor
        SharedPtr() : ptr(nullptr), count(new std::atomic<unsigned int>(0)) {}
        //constructor with pointer
        SharedPtr(T* ptr) : ptr(ptr), count(new std::atomic<unsigned int>(1)) {
            //Check specifically if we are indirectly pointed to nullptr, in that case, treat like nullptr SharedPtr
            if (ptr == nullptr) {
                *this->count = 0;
            }
        }

        //copy constructor
        /*
        * Check to see if we are already pointing to object, if not do assignments, making sure to check for nullptr to treat as special case
        */
        SharedPtr(const SharedPtr & obj) {
            std::lock_guard<std::mutex> guard(obj.mtx);
            this->ptr = obj.ptr;
            this->count = obj.count;
            if (obj.ptr != nullptr) {
                (*this->count)++;
            }
        }
        SharedPtr& operator=(const SharedPtr & obj) {
            //If already pointing to same object, increment count and return
            if (this != &obj) {
                std::lock(mtx, obj.mtx);
                std::lock_guard<std::mutex> guard1(mtx, std::adopt_lock);
                std::lock_guard<std::mutex> guard2(obj.mtx, std::adopt_lock);
                if (this->ptr != nullptr) {
                    cleanup();
                }
                //if not pointing to a object, just copy pointer and count
                this->ptr = obj.ptr;
                this->count = obj.count;
                
                if (obj.ptr != nullptr) {
                    (*this->count)++;
                }
            }
            return *this;
        }

        //move constructor
        /*
        * Check to see if we are already pointing to object, if not do assignments, making sure to check for nullptr to treat as special case
        * since it is a move, no need to worry about the count, it will remain the same
        */
        SharedPtr(SharedPtr && obj) {
            std::lock_guard<std::mutex> guard(obj.mtx);
            this->ptr = obj.ptr;
            this->count = obj.count;
            obj.ptr = nullptr;
            obj.count = nullptr;
        }
        SharedPtr& operator=(SharedPtr && obj) {
            if (this != &obj) {
                std::lock(mtx, obj.mtx);
                std::lock_guard<std::mutex> guard1(mtx, std::adopt_lock);
                std::lock_guard<std::mutex> guard2(obj.mtx, std::adopt_lock);
                if (this->ptr != nullptr) {
                    cleanup();
                }
                this->ptr = obj.ptr;
                this->count = obj.count;
                obj.ptr = nullptr;
                obj.count = nullptr;
            }
            return *this;
        }

        //overloaded dereference operator
        T* operator->() const {
            std::lock_guard<std::mutex> guard(mtx);
            return this->ptr;
        }
        T& operator*() const {
            std::lock_guard<std::mutex> guard(mtx);
            return *this->ptr;
        }

        //get count
        unsigned int getCount() const {
            std::lock_guard<std::mutex> guard(mtx);
            return *this->count;
        }
        //get pointer
        T* get() const {
            std::lock_guard<std::mutex> guard(mtx);
            return this->ptr;
        }

        //destructor
        ~SharedPtr() {
            std::lock_guard<std::mutex> guard(mtx);
            cleanup();
        }

        //reset
        void reset() {
            std::lock_guard<std::mutex> guard(mtx);
            if (this->ptr != nullptr) {
                cleanup();
            }
            this->ptr = nullptr;
            this->count = new std::atomic<unsigned int>(0);
        }

        //reset with new pointer
        void reset(T* ptr) {
            const std::lock_guard<std::mutex> lock(mtx);
            if (this->ptr != nullptr) {
                cleanup();
            }
            this->ptr = ptr;
            if (ptr != nullptr) {
                this->count = new std::atomic<unsigned int>(1);
            } else {
                this->count = new std::atomic<unsigned int>(0);
            }
        }

        private:
            /* Decrement the count to current object assigned to current SharedPtr, and remove the ptr associated with it,
            * if the count is 1, then we are the last ptr to object, so we can delete the object.
            * if the count is 0, ptr is pointing to a nullptr, the block should be left alone.
            */
            void cleanup() {
                if (count) {
                    if (*(this->count) > 0 && --(*this->count) == 0) {
                        delete this->ptr;
                        delete this->count;
                    }
                }
            }

};

#endif // SHARED_PTR_H