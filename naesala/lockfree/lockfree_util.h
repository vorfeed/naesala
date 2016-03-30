// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

namespace naesala {

/// Get real pointer from combination.
template <class T>
T* pointer(uint64_t combine) {
    return reinterpret_cast<T*>(combine & 0x0000FFFFFFFFFFFF);
}

/// Use the high 16bits as mask to solve ABA problem.
/// Combine pointer and mask.
template <class T>
uint64_t combine(T* pointer) {
    static std::atomic_short version(0);
    return reinterpret_cast<uint64_t>(pointer) |
        (static_cast<uint64_t>(version.fetch_add(1, std::memory_order_acq_rel)) << 48);
}

} // namespace naesala
