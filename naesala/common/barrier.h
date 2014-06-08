// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <assert.h>
#include <mutex>
#include <condition_variable>
#include <boost/noncopyable.hpp>

namespace naesala {

class Barrier : boost::noncopyable {
public:
    explicit Barrier(uint32_t parties) : _parties(parties) {
        assert(_parties > 0);
    }

    void await() {
        MutexGuard mg(_mutex);
        --_parties;
        if (!_parties) {
            _cond.notify_all();
            return;
        }
        _cond.wait(mg, [this] {return _parties == 0;});
        assert(!_parties);
    }

private:
    typedef std::unique_lock<std::mutex> MutexGuard;
    uint32_t _parties;
    std::mutex _mutex;
    std::condition_variable _cond;
};

} // namespace naesala
