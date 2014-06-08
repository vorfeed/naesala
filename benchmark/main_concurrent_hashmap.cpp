// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>

#include <naesala/common/range.h>
#include <naesala/common/duration.h>
#include <naesala/third/ulib/hash_chain.h>

using namespace naesala;

template <class ConcurrentHashmap>
void test_concurrent_hashmap() {
    int const THREAD_NUM = 16;
    int const OP_TIMES = 100000;
    int const KEY_POS = 0xcdcdcdcd;
    int const KEY_SPAN = OP_TIMES * THREAD_NUM * 2;
    ConcurrentHashmap hashmap(OP_TIMES);
    std::vector<std::unique_ptr<std::thread>> inserters, traversers, finders, deleters;
    for (int i : range(THREAD_NUM)) {
        inserters.emplace_back(new std::thread([&, i]() {
            Duration duration("insert");
            for (int i : range(OP_TIMES)) {
                hashmap[KEY_POS + rand() % KEY_SPAN] = i;
            }
        }));
    }
    for (int i : range(THREAD_NUM)) {
        traversers.emplace_back(new std::thread([&, i]() {
            Duration duration("traverse");
            for (typename ConcurrentHashmap::iterator ite(hashmap.begin());
                    ite != hashmap.end(); ++ite) {
                auto i = *ite;
                (void) i; // suppress unused warning
            }
        }));
    }
    for (int i : range(THREAD_NUM)) {
        finders.emplace_back(new std::thread([&, i]() {
            Duration duration("find");
            for (int i : range(OP_TIMES)) {
                int j = hashmap[KEY_POS + rand() % KEY_SPAN];
                (void) j; // suppress unused warning
            }
        }));
    }
    for (int i : range(THREAD_NUM)) {
        deleters.emplace_back(new std::thread([&, i]() {
            Duration duration("delete");
            for (int i : range(KEY_SPAN)) {
                hashmap.erase(KEY_POS + rand() % KEY_SPAN);
            }
        }));
    }
}

int main(int argc, char* argv[]) {
    test_concurrent_hashmap<ulib::chain_hash_map<int, int>>();
    return 0;
}

