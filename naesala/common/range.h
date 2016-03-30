// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

namespace naesala {

template <typename T>
class Range {
public:
    class iterator {
    public:
        iterator(T value, T step) :
            _value(value), _step(step) {}

        bool operator !=(iterator const& other) const {
            return _value != other._value;
        }
        T operator*() const {
            return _value;
        }
        iterator const& operator++() {
            _value += _step;
            return *this;
        }
    private:
        T _value, _step;
    };

    Range(T end) : _start(T()), _end(end), _step(1) {}
    Range(T start, T end, T step = 1) :
            _start(start), _end(end), _step(step) {}

    iterator begin() const {
        return iterator(_start, _step);
    }
    iterator end() const {
        return iterator(_end, _step);
    }

private:
    T _start, _end, _step;
};

typedef Range<int> range;
typedef Range<size_t> sequence;

} // namespace naesala

