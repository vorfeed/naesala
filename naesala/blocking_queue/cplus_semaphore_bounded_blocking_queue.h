// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <assert.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>

namespace naesala {

template <typename Task>
class CplusSemaphoreBoundedBlockingQueue : boost::noncopyable {
public:
    explicit CplusSemaphoreBoundedBlockingQueue(size_t max_size) :
            _queue(max_size), _not_empty(0), _not_full(max_size) {}

    void put(Task const& t) {
        _not_full.wait();
        MutexGuard mg(_mutex);
        assert(!_queue.full());
        _queue.push_back(t);
        mg.unlock();
        _not_empty.post();
    }

    Task take() {
        _not_empty.wait();
        MutexGuard mg(_mutex);
        assert(!_queue.empty());
        Task front(_queue.front());
        _queue.pop_front();
        mg.unlock();
        _not_full.post();
        return front;
    }

private:
    typedef boost::unique_lock<boost::mutex> MutexGuard;
    boost::mutex _mutex;
    boost::interprocess::interprocess_semaphore _not_empty, _not_full;
    boost::circular_buffer<Task> _queue;
};

} // namespace naesala
