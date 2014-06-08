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
class SlabPool : boost::noncopyable {
public:
    typedef void (*DestoryFunc)(T*);

    static DestoryFunc const default_destory = [](T* node) { delete node; };

    SlabPool(DestoryFunc const& destory = default_destory) : _destory(destory) {}
    ~SlabPool() {
        while (_live) {
            T* node = _live;
            _live = _live->next;
            _destory(node);
        }
        while (_free) {
            T* node = _free;
            _free = _free->next;
            _destory(node);
        }
    }

    T* live() { return _live; }

    T* get() {
        T* ret = _free;
        if (ret) {
            _free = _free->next;
        } else {
            ret = new T();
        }
        ret->next = _live;
        _live = ret;
        return ret;
    }

    void back(T*& node) {
        if (!_free) {
            node->next = nullptr;
            _free = node;
            return;
        }
        node->next = _free;
        _free = node;
        node = nullptr;
    }

private:
    static_assert(std::is_base_of<IListNode, T>::value,
            "SlabPool's node must be derived from IListNode!");

    T* _live { nullptr }, _free { nullptr };
    DestoryFunc _destory;
};

} // namespace naesala
