// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <assert.h>
#include <mutex>
#include <condition_variable>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/lock_types.hpp>
#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>

namespace naesala {

class STDTypes {
public:
    typedef std::mutex Mutex;
    typedef std::condition_variable Condition;
    typedef std::unique_lock<Mutex> MutexGuard;
};

class BoostTypes {
public:
    typedef boost::mutex Mutex;
    typedef boost::condition Condition;
    typedef boost::unique_lock<Mutex> MutexGuard;
};

template <typename Task, class Type = STDTypes>
class CplusConditionBoundedBlockingQueue : boost::noncopyable {
public:
    explicit CplusConditionBoundedBlockingQueue(size_t max_size) : _queue(max_size) {}

    void put(Task const& t) {
        MutexGuard mg(_mutex);
        _not_full.wait(mg, [this] {return !_queue.full();});
        assert(!_queue.full());
        _queue.push_back(t);
        mg.unlock();
        _not_empty.notify_one();
    }

    Task take() {
        MutexGuard mg(_mutex);
        _not_empty.wait(mg, [this] {return !_queue.empty();});
        assert(!_queue.empty());
        Task front(_queue.front());
        _queue.pop_front();
        mg.unlock();
        _not_full.notify_one();
        return front;
    }

private:
    typedef typename Type::MutexGuard MutexGuard;
    typedef typename Type::Mutex Mutex;
    typedef typename Type::Condition Condition;
    Mutex _mutex;
    Condition _not_empty;
    Condition _not_full;
    boost::circular_buffer<Task> _queue;
};

} // namespace naesala
