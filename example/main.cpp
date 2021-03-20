#include <iostream>
#include <chrono>
#include <stack_allocator.h>
#include <vector>

struct MyStruct {
    MyStruct(){
        std::cout << "Constructed MyStruct!" << std::endl;
    }

    ~MyStruct(){
        std::cout << "Destructed MyStruct!" << std::endl;
    }

    int a = 0;
    int b = 0;
    double c = 0;
};


class MyClass {
public:
    MyClass(){
        std::cout << "Constructed MyClass!" << std::endl;
    }

    ~MyClass(){
        std::cout << "Destructed MyClass!" << std::endl;
    }

    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
};

TypedStackAllocator typedStackAllocator(1024 * 1024);

template<typename TAlloc, typename TDelete>
size_t test(const std::vector<size_t>& sizes, int iterations, TAlloc alloc, TDelete del){
    size_t accumulatedDuration = 0;
    float totalSum = 0;
    for (int i = 0; i < iterations; i++) {
        int qty = sizes[i];
        auto start = std::chrono::steady_clock::now();
        auto myFloats = alloc(qty);
        auto end = std::chrono::steady_clock::now();
        accumulatedDuration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        float sum = 0;
        for (int j = 0; j < qty; j++){
            myFloats[j].a = sizes[i];
            sum += myFloats[j].a;
        }
        totalSum += sum;
        start = std::chrono::steady_clock::now();
        del(myFloats);
        end = std::chrono::steady_clock::now();
        accumulatedDuration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }
    return accumulatedDuration / iterations;
}


int main() {

//    srand(time(NULL));

//    const int iterations = 1000;
//    const int allocationSize = 10000;
//    size_t stackAccumulatedDuration = 0;
//    size_t newAccumulatedDuration = 0;
//
//    for (int i = 0; i < 1000; i++) {
//        std::vector<size_t> allocationSizes(iterations);
//        for (auto &size : allocationSizes) {
//            size = (rand() % allocationSize) + 1;
//        }
//
//        stackAccumulatedDuration += test(allocationSizes, iterations, [](size_t qty) -> MyClass * {
//            return typedStackAllocator.construct<MyClass>(qty);
//        }, [](MyClass *MyClasss) {
//            typedStackAllocator.pop();
//        });
//
//        newAccumulatedDuration += test(allocationSizes, iterations, [](size_t qty) -> MyClass * {
//            return new MyClass[qty];
//        }, [](MyClass *MyClasss) {
//            delete[] MyClasss;
//        });
//    }
//
//    std::cout << "Stack duration: " << stackAccumulatedDuration / 1000 << std::endl;
//    std::cout << "New duration: " << newAccumulatedDuration / 1000 << std::endl;

//    TypedStackAllocator stackAllocator(1024);

    TypedStackAllocator stack(1024 * 1024);
    {
        auto scope =  stack.scope();
        auto myArr1 = scope.construct<MyClass>(10);
        auto myArr2 = scope.construct<MyClass>(10);
        auto myArr3 = scope.construct<MyClass>(14);
        auto myArr4 = scope.construct<MyClass>(12);
        auto myArr5 = scope.construct<MyClass>(10);
        auto myArr6 = scope.construct<MyClass>(10);
    }

    assert(stack.marker() == 0);

    return 0;
}
