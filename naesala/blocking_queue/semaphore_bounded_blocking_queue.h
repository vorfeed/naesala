// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>

namespace naesala {

template <typename Task>
class SemaphoreBoundedBlockingQueue : boost::noncopyable {
public:
    explicit SemaphoreBoundedBlockingQueue(size_t max_size) : _queue(max_size) {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
        sem_init(&_not_empty, 0, 0);
        sem_init(&_not_full, 0, max_size);
    }

    void put(Task const& t) {
        sem_wait(&_not_full);
        pthread_mutex_lock(&_mutex);
        assert(!_queue.full());
        _queue.push_back(t);
        pthread_mutex_unlock(&_mutex);
        sem_post(&_not_empty);
    }

    Task take() {
        sem_wait(&_not_empty);
        pthread_mutex_lock(&_mutex);
        assert(!_queue.empty());
        Task front(_queue.front());
        _queue.pop_front();
        pthread_mutex_unlock(&_mutex);
        sem_post(&_not_full);
        return front;
    }

private:
    pthread_mutex_t _mutex;
    sem_t _not_empty, _not_full;
    boost::circular_buffer<Task> _queue;
};

} // namespace naesala
