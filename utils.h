#pragma once

#define pow2(x) (x * x)

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}