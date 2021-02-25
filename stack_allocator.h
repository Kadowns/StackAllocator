//
// Created by Ricardo on 2/25/2021.
//

#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <cstdint>
#include <cassert>

class StackAllocator {
public:
    explicit StackAllocator(size_t size) : m_size(size), m_block((uint8_t*)malloc(m_size)), m_marker(0) {}
    virtual ~StackAllocator() { free (m_block);}


    void* alloc(size_t size) {
        uintptr_t address = reinterpret_cast<uintptr_t>(m_block) + m_marker;
        m_marker += size;
        return reinterpret_cast<void*>(address);
    }

    size_t marker() const { return m_marker; }

    void free_to_marker(size_t marker){
        assert(marker >= 0);
        m_marker = marker;
    }

protected:
    size_t m_size;
    uint8_t* m_block;
    size_t m_marker;
};


class TypedStackAllocator : private StackAllocator {
private:
    typedef void (*DestructorFunc)(void*, size_t);
public:

    explicit TypedStackAllocator(size_t size) : StackAllocator(size) {}
    ~TypedStackAllocator() override = default;

    template<typename T, typename ...Args>
    T* construct(size_t quantity = 1, Args&& ...args) {
        assert(quantity >= 1);

        size_t currentMarker = marker();
        auto destructorAddress = (DestructorFunc*)alloc(sizeof(DestructorFunc));
        *destructorAddress = destruct<T>;

        auto quantityAddress = (size_t*)alloc(sizeof(size_t));
        *quantityAddress = quantity;

        T* obj = (T*)alloc(sizeof(T) * quantity);
        T* objIterator = obj;
        for (int i = 0; i < quantity; i++){
            ::new(objIterator) T(std::forward(args)...);
            objIterator++;
        }
        auto lastMarkerAddress = (size_t*)alloc(sizeof(size_t));
        *lastMarkerAddress = currentMarker;
        return obj;
    }

    void pop(){
        auto lastMarkerAddress = reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(m_block) + m_marker - sizeof(size_t));
        auto destructorAddress = reinterpret_cast<DestructorFunc*>(reinterpret_cast<uintptr_t>(m_block) + *lastMarkerAddress);
        auto quantityAddress = reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(destructorAddress) + sizeof(DestructorFunc*));
        auto objAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(quantityAddress) + sizeof(size_t));
        (*destructorAddress)(objAddress, *quantityAddress);
        free_to_marker(*lastMarkerAddress);
    }

private:

    template<typename T>
    static void destruct(void* obj, size_t quantity){
        T* tobj = static_cast<T*>(obj);
        for (int i = 0; i < quantity; i++){
            tobj->~T();
            tobj++;
        }
    }

};

#endif //STACK_ALLOCATOR_H
