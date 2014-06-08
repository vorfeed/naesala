// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <assert.h>
#include <pthread.h>
#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>

namespace naesala {

template <typename Task>
class ConditionBoundedBlockingQueue : boost::noncopyable {
public:
    explicit ConditionBoundedBlockingQueue(size_t max_size) : _queue(max_size) {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
        _not_full = _not_empty = PTHREAD_COND_INITIALIZER;
    }

    void put(Task const& t) {
        pthread_mutex_lock(&_mutex);
        while (_queue.full()) {
            pthread_cond_wait(&_not_full, &_mutex);
        }
        assert(!_queue.full());
        _queue.push_back(t);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_not_empty);
    }

    Task take() {
        pthread_mutex_lock(&_mutex);
        while (_queue.empty()) {
            pthread_cond_wait(&_not_empty, &_mutex);
        }
        assert(!_queue.empty());
        Task front(_queue.front());
        _queue.pop_front();
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_not_full);
        return front;
    }

private:
    // error: cannot convert '<brace-enclosed initializer list>' to 'int' in initialization
    pthread_mutex_t _mutex /* { PTHREAD_MUTEX_INITIALIZER } */;
    pthread_cond_t _not_empty /* { PTHREAD_COND_INITIALIZER } */;
    pthread_cond_t _not_full /* { PTHREAD_COND_INITIALIZER } */;
    boost::circular_buffer<Task> _queue;
};

} // namespace naesala
