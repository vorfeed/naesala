// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

namespace naesala {

template <class T>
struct IListNode {
    IListNode() {}
    IListNode(T* next, T* prev = nullptr) : next(next), prev(prev) {}
    T* next { nullptr }, *prev { nullptr };
};

template <class T>
struct ISlistNode {
    ISlistNode() {}
    ISlistNode(T* next) : next(next) {}
    T* next { nullptr };
};

} // namespace naesala
