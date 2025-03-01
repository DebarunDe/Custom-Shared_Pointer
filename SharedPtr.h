#ifndef SHARED_PTR_H
#define SHARED_PTR_H

template <typename T>
class SharedPtr {
    private:
        T* ptr;
        unsigned int* count;
        //mutex?
    public:
        //default constructor
        SharedPtr() : ptr(nullptr), count(new unsigned int(0)) {}
        //constructor with pointer
        SharedPtr(T* ptr) : ptr(ptr), count(new unsigned int(1)) {}

        //copy constructor
        SharedPtr(const SharedPtr & obj) {
            this->ptr = obj.ptr;
            this->count = obj.count;
            if (obj.ptr != nullptr) {
                (*this->count)++;
            }
        }
        SharedPtr& operator=(const SharedPtr & obj) {
            //If already pointing to same object, increment count and return
            if (this != &obj) {
                if (this->ptr != nullptr) {
                    cleanup();
                }
                //if not pointing to a object, just copy pointer and count
                this->ptr = obj.ptr;
                this->count = obj.count;
            }
            //no matter what, increment the count
            if (obj.ptr != nullptr) {
                (*this->count)++;
            }
            return *this;
        }

        //move constructor
        SharedPtr(SharedPtr && obj) {
            this->ptr = obj.ptr;
            this->count = obj.count;
            obj.ptr = nullptr;
            obj.count = nullptr;
        }
        SharedPtr& operator=(SharedPtr && obj) {
            if (this != &obj) {
                if (this->ptr != nullptr) {
                    cleanup();
                }
                this->ptr = obj.ptr;
                this->count = obj.count;
            }
            obj.ptr = nullptr;
            obj.count = nullptr;
            return *this;
        }

        //overloaded dereference operator
        T* operator->() const {
            return this->ptr;
        }
        T& operator*() const {
            return *this->ptr;
        }

        //get count
        unsigned int getCount() const {
            return *this->count;
        }
        //get pointer
        T* get() const {
            return this->ptr;
        }

        //destructor
        ~SharedPtr() {
            cleanup();
        }

        private:
            void cleanup() {
                /* Decrement the count to current object assigned to current SharedPtr, and remove the ptr associated with it,
                * if the count is 1, then we are the last ptr to object, so we can delete the object.
                * if the count is 0, ptr is pointing to a nullptr, so we can just delete the count.
                */
               if (count && --(*this->count) == 0) {
                    if (this->ptr != nullptr) {
                        delete this->ptr;
                    }
                    //free up memory held by SharedPtr's count
                    delete this->count;
                }
            }

};

#endif // SHARED_PTR_H