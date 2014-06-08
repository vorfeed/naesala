// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <limits.h>

namespace naesala {

/// Use the high 16bits as mask to solve ABA problem.
inline uint64_t mask() {
    return static_cast<uint64_t>(rand() % INT16_MAX) << 48;
}

/// Get real pointer from combination.
template <class T>
T* pointer(uint64_t combine) {
    return reinterpret_cast<T*>(combine & 0x0000FFFFFFFFFFFF);
}

/// Combine pointer and mask.
template <class T>
uint64_t combine(T* pointer, uint64_t mask) {
    return reinterpret_cast<uint64_t>(pointer) | mask;
}

} // namespace naesala
