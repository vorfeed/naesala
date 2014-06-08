// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <atomic>
#include <boost/noncopyable.hpp>
#include "lockfree_util.h"

namespace naesala {

struct IStackNode { IStackNode* next; };

/// Treiber (1986)
// Use mask to solve ABA problem¡£
template <class T>
class LockfreeStack : boost::noncopyable {
public:
    LockfreeStack() : _top(0) {}

    // T is a heap object.
    void push(T* node) {
        uint64_t last_top_combine = 0;
        uint64_t node_combine = combine(node, mask());
        do {
            last_top_combine = _top.load(std::memory_order_acquire);
            node->next = pointer<T>(last_top_combine);
        } while (!_top.compare_exchange_weak(last_top_combine, node_combine));
    }

    T* pop() {
        T* top = nullptr;
        uint64_t top_combine = 0, new_top_combine = 0;
        do {
            top_combine = _top.load(std::memory_order_acquire);
            top = pointer<T>(top_combine);
            if (!top) {
                return nullptr;
            }
            new_top_combine = combine(top->next, mask());
        } while(!_top.compare_exchange_weak(top_combine, new_top_combine));
        return top;
    }

private:
    static_assert(std::is_base_of<IStackNode, T>::value,
            "LockfreeStack's node must be derived from IStackNode!");
    std::atomic<uint64_t> _top;
};

} // namespace naesala
