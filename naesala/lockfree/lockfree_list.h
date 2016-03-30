// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <string.h>
#include <atomic>
#include <boost/noncopyable.hpp>

#include <naesala/common/likely.h>
#include "lockfree_util.h"

namespace naesala {

template <class T>
struct IListNode {
    IListNode() {}
    IListNode(uint64_t next) : next(next) {}
    std::atomic<uint64_t> next { 0 };
};

/// Michael Scott (1996), ¡¶Java Concurrency in Practice¡·
// Use mask to solve ABA problem,
// otherwise there will be problems like invalid data, duplicated data and dead lock.
// Same problems with boost::lockfree::queue.
template <class T>
class LockfreeList : boost::noncopyable {
public:
    LockfreeList() : _dummy(combine(new T())), _head(_dummy), _tail(_dummy) {}
    ~LockfreeList() {
        T* t = nullptr;
        while ((t = take())) {
            delete t;
        }
        delete pointer<T>(_tail.load(std::memory_order_acquire));
    }

    // T is a heap object.
    void put(T* node) {
        while (true) {
//            node->next.store(0, std::memory_order_relaxed);
            // (gdb) p tail
            // $1 = (LFLWrapper::Node *) 0x202c30373832202c
            // How can tail be set with an invalid pointer ?
            uint64_t tail_combine = _tail.load(std::memory_order_acquire);
            T* tail = pointer<T>(tail_combine);
            uint64_t tail_next_combine = tail->next.load(std::memory_order_acquire);
            T* tail_next = pointer<T>(tail_next_combine);
            // Tail was not pointing to the last node.
            if (tail_next) {
                // Try to swing Tail to the next node.
                _tail.compare_exchange_strong(tail_combine, combine(tail_next));
            //  Try to link node at the end of the linked list.
            } else if (tail->next.compare_exchange_weak(tail_next_combine, combine(node))) {
                // Try to swing Tail to the inserted node.
                _tail.compare_exchange_strong(tail_combine, combine(node));
                break;
            }
        }
    }

    T* take() {
        while (true) {
            uint64_t head_combine = _head.load(std::memory_order_acquire);
            T* head = pointer<T>(head_combine);
            uint64_t tail_combine = _tail.load(std::memory_order_acquire);
            T* tail = pointer<T>(tail_combine);
            // (gdb) p head_next
            // $2 = (LFLWrapper::Node *) 0x20
            // It always occur if replace compare_exchange_weak with compare_exchange_strong,
            // is there any problem with compare_exchange_strong ?
            uint64_t head_next_combine = head->next.load(std::memory_order_acquire);
            T* head_next = pointer<T>(head_next_combine);
            if (unlikely(head_combine != _head.load(std::memory_order_acquire))) {
              continue;
            }
            if (head == tail) {
                if (!head_next) {
                    // Queue empty.
                    return nullptr;
                }
                // Tail is falling behind, try to advance it.
                _tail.compare_exchange_strong(tail_combine, combine(head_next));
            } else {
                if (!head_next) {
                    // <boost/lockfree/queue.hpp>
                    continue;
                }
                // Try to swing Head to the next node.
                uint8_t bytes[DATA_SIZE];
                memcpy(bytes, reinterpret_cast<uint8_t*>(head_next) + sizeof(IListNode<T>), DATA_SIZE);
                if (_head.compare_exchange_weak(head_combine, combine(head_next))) {
                    memcpy(reinterpret_cast<uint8_t*>(head) + sizeof(IListNode<T>), bytes, DATA_SIZE);
                    head->next.store(0, std::memory_order_release);
                    return head;
                }
            }
        }
        return nullptr;
    }

private:
    static_assert(std::is_base_of<IListNode<T>, T>::value,
            "LockfreeList's node T must be derived from IListNode<T>!");
    static size_t const DATA_SIZE = sizeof(T) - sizeof(IListNode<T>);
    uint64_t _dummy;
    std::atomic<uint64_t> _head, _tail;
};

} // namespace naesala
