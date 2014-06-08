// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <boost/noncopyable.hpp>

#include "../common/likely.h"
#include "list_node.h"

namespace naesala {

template <class T>
class List : boost::noncopyable {
public:
    typedef void (*DestoryFunc)(T*);

    static DestoryFunc const default_destory = [](T* node) { delete node; };

    List(DestoryFunc const& destory = default_destory) : _destory(destory) {}
    ~List() {
        while (_start != _end) {
            T* node = pop();
            _destory(node);
        }
    }

    T* front() { return _start; }

    void push(T* node) {
        node->next = nullptr;
        if (likely(!empty())) {
            _end->next = node;
            _end = node;
        } else {
            _end = _start = node;
        }
    }

    template <typename OtherT>
    void append(List<OtherT>& l) {
        if (unlikely(l.empty())) {
            return;
        }
        if (unlikely(empty())) {
            _start = l._start;
        } else {
            _end->next = l._start;
        }
        _end = l._end;
        l._end = l._start = nullptr;
    }

    // Destory node by user.
    T* pop() {
        T* node = _start;
        if (likely(!empty())) {
            _start = _start->next;
            if (unlikely(nullptr == _start)) {
                // must same
                _end = nullptr;
            }
            node->next = nullptr;
        }
        return node;
    }

    bool empty() const {
        return nullptr == _start;
    }

private:
    static_assert(std::is_base_of<IListNode, T>::value,
            "List's node must be derived from IListNode!");

    T* _start { nullptr }, _end { nullptr };    ///< first and last node
    DestoryFunc _destory;
};

} // namespace naesala
