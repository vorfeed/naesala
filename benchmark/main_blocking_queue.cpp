// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <time.h>
#include <vector>
#include <thread>

#include <naesala/common/range.h>
#include <naesala/blocking_queue/condition_bounded_blocking_queue.h>
#include <naesala/blocking_queue/semaphore_bounded_blocking_queue.h>
#include <naesala/blocking_queue/cplus_condition_bounded_blocking_queue.h>
#include <naesala/blocking_queue/cplus_semaphore_bounded_blocking_queue.h>

using namespace naesala;

struct suite {
    suite(int p, int c, int nelts) :
        p(p), c(c), nelts(nelts) {}
    template <class Queue>
    void operator()() {
        Queue queue(nelts);
        std::cout << "testing " << typeid(queue).name() << " ..." << std::endl;
        clock_gettime(CLOCK_MONOTONIC, &start);
        // produce
        std::vector<std::thread*> producers;
        for (int i : range(0, p)) {
            producers.push_back(new std::thread([this, &queue, i]() {
                clock_gettime(CLOCK_MONOTONIC, &produce_start[i]);
                for (int i : range(0, nelts)) {
                    queue.put(rand());
                }
                clock_gettime(CLOCK_MONOTONIC, &produce_stop[i]);
                produce_time_used[i] =
                        (produce_stop[i].tv_sec - produce_start[i].tv_sec) +
                        (produce_stop[i].tv_nsec - produce_start[i].tv_nsec) /
                        1000000000.0;
            }));
        }
        // consume
        std::vector<std::thread*> consumers;
        for (int i : range(0, c)) {
            consumers.push_back(new std::thread([this, &queue, i]() {
                clock_gettime(CLOCK_MONOTONIC, &consume_start[i]);
                for (int i : range(0, nelts)) {
                    queue.take();
                }
                clock_gettime(CLOCK_MONOTONIC, &consume_stop[i]);
                consume_time_used[i] =
                        (consume_stop[i].tv_sec - consume_start[i].tv_sec) +
                        (consume_stop[i].tv_nsec - consume_start[i].tv_nsec) /
                        1000000000.0;
            }));
        }
        for (int i : range(0, p)) {
            producers[i]->join();
        }
        for (auto thread : producers) {
            delete thread;
        }
        for (int i : range(0, c)) {
            consumers[i]->join();
        }
        for (auto thread : consumers) {
            delete thread;
        }
        clock_gettime(CLOCK_MONOTONIC, &stop);
        double total = 0;
        std::cout << "[produce time used]" << std::endl;
        for (int i : range(0, p)) {
            std::cout << "thread[" << i << "]: " << produce_time_used[i] << std::endl;
            total += produce_time_used[i];
        }
        std::cout << "average: " << total * 1.0 / p << std::endl << std::endl;
        total = 0;
        std::cout << "[consumer time used]" << std::endl;
        for (int i : range(0, c)) {
            std::cout << "thread[" << i << "]: " << consume_time_used[i] << std::endl;
            total += consume_time_used[i];
        }
        std::cout << "average: " << total * 1.0 / c << std::endl << std::endl;
        std::cout << "[total time used]" << std::endl;
        std::cout << "total: " << (stop.tv_sec - start.tv_sec)
                + (stop.tv_nsec - start.tv_nsec) / 1000000000.0 << std::endl;
        std::cout << std::endl;
    }
    static int const MAX_THREAD_NUM = 16;   ///< limited
    int p, c, nelts;
    struct timespec start, stop;
    struct timespec produce_start[MAX_THREAD_NUM], produce_stop[MAX_THREAD_NUM];
    struct timespec consume_start[MAX_THREAD_NUM], consume_stop[MAX_THREAD_NUM];
    double produce_time_used[MAX_THREAD_NUM], consume_time_used[MAX_THREAD_NUM];
};

int main(int argc, char* argv[]) {
    int p = 8, c = 8;
    int nelts = 1000000;
    if (argc > 1) {
        nelts = atoi(argv[1]);
    }
    if (argc > 3) {
        p = strtoul(argv[2], 0, 10);
        c = strtoul(argv[3], 0, 10);
    }
    std::cout <<
            "elements num: " << nelts <<
            ", producer num: " << p <<
            ", consumer num: " << c << std::endl << std::endl;

    suite s(p, c, nelts);
    s.operator()<ConditionBoundedBlockingQueue<int>>();
    s.operator()<SemaphoreBoundedBlockingQueue<int>>();
    s.operator()<CplusConditionBoundedBlockingQueue<int, STDTypes>>();
    s.operator()<CplusConditionBoundedBlockingQueue<int, BoostTypes>>();
    s.operator()<CplusSemaphoreBoundedBlockingQueue<int>>();

    return 0;
}

