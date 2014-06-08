// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <stddef.h>
#include <assert.h>
#include "../common/likely.h"
#include "../lockfree/lockfree_stack.h"

namespace naesala {

template <class T> class LockfreeSlabPool;

template <class T>
class Shared: public IStackNode, boost::noncopyable {
public:
    Shared(LockfreeSlabPool<T>* pool) : _pool(pool) { assert(_pool); }

    bool incr_ref() {
        while (true) {
            int32_t last = _user_count.load(std::memory_order_acquire);
            if (unlikely(last == BACKING)) {
                // Another user is deleting this block, try to help reseting _user_count.
                _user_count.compare_exchange_strong(last, 0);
            }
            if (likely(_user_count.compare_exchange_weak(last, last + 1))) {
                break;
            }
        }
        return true;
    }
    void decr_ref() {
        while (true) {
            int32_t last = _user_count.load(std::memory_order_acquire);
            assert(last != BACKING);
            if (last == 1) {
                // To be deleted.
                if (unlikely(!_user_count.compare_exchange_weak(last, BACKING))) {
                    continue;
                }
                // If incr right after decr, maybe in pool but used.
                _pool->back(this);
                // Set reference count 0.
                // If failed, another thread has already reset this node.
                int backing = BACKING;
                _user_count.compare_exchange_strong(backing, 0);
                break;
            }
            if (likely(_user_count.compare_exchange_weak(last, last - 1))) {
                break;
            }
        }
    }

private:
    static int32_t const BACKING = -1;
    LockfreeSlabPool<T>* _pool { nullptr };
    std::atomic<int32_t> _user_count { 0 };
};

template <class T>
class LockfreeSlabPool : boost::noncopyable {
public:
    T* get() {
        T* node = _stack.pop();
        if (!node) {
            return new T(this);
        }
        return node;
    }

    void back(T* block) {
        _stack.push(block);
    }

private:
    static_assert(std::is_base_of<Shared<T>, T>::value,
            "LockfreeSlabPool's element must be derived from Shared!");

    LockfreeStack<T> _stack;
};

} // namespace naesala
