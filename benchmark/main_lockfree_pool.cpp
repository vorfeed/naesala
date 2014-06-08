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
#include <naesala/common/barrier.h>
#include <naesala/lockfree_pool/lockfree_slab_pool.h>

using namespace naesala;

void test_lockfree() {
    struct LSPNode : Shared<LSPNode> {
        LSPNode(LockfreeSlabPool<LSPNode>* pool) : Shared<LSPNode>(pool) {}
        int data = 0;
        int valid = 0xcdcdcdcd;
    };
    LockfreeSlabPool<LSPNode> lsp;
    int const WORKER_NUM = 8;
    int const EACH_FETCH = 10;
    int const FETCH_TIMES = 5;
    int const DATA_COUNT = EACH_FETCH * FETCH_TIMES;
    std::vector<std::unique_ptr<std::thread>> workers;
    std::atomic<int> count(0);  // atomic默认不会初始化
    std::atomic_flag sets[DATA_COUNT * WORKER_NUM];
    for (int i : range(DATA_COUNT * WORKER_NUM)) {
        sets[i].clear();
    }
    Barrier barrier(WORKER_NUM);
    std::stringstream ss[WORKER_NUM];
    for (int i : range(WORKER_NUM)) {
        workers.emplace_back(new std::thread([&, i]() {
            ss[i] << "thread[" << i << "]: " << std::endl;
            barrier.await();
            std::vector<LSPNode*> nodes;
            nodes.reserve(EACH_FETCH);
            for (int j : range(FETCH_TIMES)) {
                ss[i] << "  ";
                for (int k : range(EACH_FETCH)) {
                    LSPNode* node = lsp.get();
                    // First time used.
                    if (!node->data) {
                        node->data = count.fetch_add(1);
                    }
                    ss[i] << node->data << ", ";
                    printf("get %d, ", node->data);
                    // Make sure flag is not set, then set the flag.
                    assert(!sets[node->data].test_and_set());   // return last flag
                    nodes.push_back(node);
                    std::this_thread::yield();
                }
                ss[i] << std::endl;
                for (int k : range(EACH_FETCH)) {
                    LSPNode* node = nodes.back();
                    nodes.pop_back();
                    sets[node->data].clear();
                    lsp.back(node);
                    printf("back %d, ", node->data);
                    std::this_thread::yield();
                }
            }
        }));
    }
    for (int i : range(WORKER_NUM)) {
        workers[i]->join();
//        std::cout << ss[i].str() << std::endl;
    }
    for (int i : range(DATA_COUNT)) {
        // Make sure all flags are not set.
        assert(!sets[i].test_and_set());
    }
}

int main(int argc, char* argv[]) {
    test_lockfree();
    return 0;
}

