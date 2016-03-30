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
#include <boost/lockfree/queue.hpp>

#include <naesala/common/range.h>
#include <naesala/common/barrier.h>
#include <naesala/common/duration.h>
#include <naesala/lockfree/lockfree_stack.h>
#include <naesala/lockfree/lockfree_list.h>
#include <naesala/blocking_queue/cplus_condition_bounded_blocking_queue.h>

using namespace naesala;

class LFSWrapper {
public:
    struct Node : IStackNode { int data; int valid = 0xcdcdcdcd; };
    typedef LockfreeStack<Node> Container;
    Container& operator()() {
        return lfs;
    }
    void set(Node* node) { lfs.push(node); }
    Node* get() { return lfs.pop(); }
private:
    Container lfs;
};

class LFLWrapper {
public:
    struct Node : IListNode<Node> { int data; int valid = 0xcdcdcdcd; };
    typedef LockfreeList<Node> Container;
    Container& operator()() {
        return lfl;
    }
    void set(Node* node) { lfl.put(node); }
    Node* get() { return lfl.take(); }
private:
    Container lfl;
};

class BoostQueueWrapper {
public:
    struct Node { int data; int valid = 0xcdcdcdcd; };
    typedef boost::lockfree::queue<Node*> Container;
    Container& operator()() {
        return bq;
    }
    void set(Node* node) { bq.push(node); }
    Node* get() { Node* node; bq.pop(node); return node; }
private:
    Container bq { 1024 };
};

class BBQWrapper {
public:
    struct Node { int data; int valid = 0xcdcdcdcd; };
    typedef CplusConditionBoundedBlockingQueue<Node*> Container;
    Container& operator()() {
        return bbq;
    }
    void set(Node* node) { bbq.put(node); }
    Node* get() { return bbq.take(); }
private:
    Container bbq { 1024 };
};

template <class Wrapper>
void test_lockfree() {
    Wrapper wrapper;
    Duration duration(typeid(Wrapper).name());
    int const PRODUCER_NUM = 32;
    int const CONSUMER_NUM = 32;
    int const DATA_COUNT = 1000;
    std::vector<std::thread*> producers;
    std::vector<std::thread*> consumers;
    std::atomic<int> count(0);  // atomic默认不会初始化
    std::atomic_flag sets[DATA_COUNT*PRODUCER_NUM];
    for (int i : range(DATA_COUNT*PRODUCER_NUM)) {
        sets[i].clear();
    }
    // producers
    Barrier bp(PRODUCER_NUM);
    for (int i : range(PRODUCER_NUM)) {
        producers.push_back(new std::thread([&, i]() {
            bp.await();
            for (int j : range(DATA_COUNT)) {
                typename Wrapper::Node* node = new typename Wrapper::Node();
//                printf("n:%p\n", node);
                node->data = count.fetch_add(1);
//                printf("%d, ", node->data);
                // Make sure flag is not set, then set the flag.
                assert(!sets[node->data].test_and_set());   // return last flag
                wrapper.set(node);
                std::this_thread::yield();
            }
        }));
    }
    // consumers
    Barrier bc(PRODUCER_NUM);
    std::stringstream ss[CONSUMER_NUM];
    for (int i : range(CONSUMER_NUM)) {
        consumers.push_back(new std::thread([&, i]() {
//            ss[i] << "thread[" << i << "]: ";
            bc.await();
            for (int j : range(DATA_COUNT)) {
                typename Wrapper::Node* node = nullptr;
                while (!(node = wrapper.get())) {
//                    usleep(1);
                    std::this_thread::yield();
                }
                // Make sure flag is set, then clear the flag.
                assert(sets[node->data].test_and_set());
                sets[node->data].clear();
//                ss[i] << node->data << ", ";
//                printf("d:%p\n", node);
                delete node;
                std::this_thread::yield();
            }
        }));
    }
    for (int i : range(PRODUCER_NUM)) {
        producers[i]->join();
        delete producers[i];
    }
    for (int i : range(CONSUMER_NUM)) {
        consumers[i]->join();
        delete consumers[i];
    }
    for (int i : range(CONSUMER_NUM)) {
//        std::cout << ss[i].str() << std::endl;
    }
    for (int i : range(DATA_COUNT)) {
        // Make sure all flags are not set.
        assert(!sets[i].test_and_set());
    }
}

int main(int argc, char* argv[]) {
    test_lockfree<LFSWrapper>();
    std::cout << std::endl;
    test_lockfree<LFLWrapper>();
    std::cout << std::endl;
    test_lockfree<BoostQueueWrapper>();
    std::cout << std::endl;
    test_lockfree<BBQWrapper>();
    std::cout << std::endl;
    return 0;
}

