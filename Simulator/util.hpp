#pragma once

#include <cstdint>
#include <utility>

typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t i32;

const int MEM_DEPTH = 1 << 16;

template <typename T, typename U, typename V>
struct trio {
    T first;
    U second;
    V third;

    trio(T first, U second, V third) {
        this->first = first;
        this->second = second;
        this->third = third;
    }
};