#include <iostream>
#include <stack_allocator.h>

struct MyClass {
    MyClass(){
        std::cout << "Constructed!" << std::endl;
    }

    ~MyClass(){
        std::cout << "Destructed!" << std::endl;
    }

    int a = 0;
    int b = 0;
    int c = 0;
};


int main() {

    TypedStackAllocator typedStackAllocator(1024);

    auto myClasses = typedStackAllocator.construct<MyClass>(3);

    typedStackAllocator.pop();


    return 0;
}
