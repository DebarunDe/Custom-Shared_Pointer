#ifndef SHARED_PTR_H
#define SHARED_PTR_H

template <typename T>
class SharedPtr {
    private:
        T* ptr;
        uint* count;
        //mutex?
        mutable std::mutex lock;
    public:
        //default constructor
        SharedPtr() : ptr(nullptr), count(new uint(0)) {
            
        }
};

#endif // SHARED_PTR_H