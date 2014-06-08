// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>
#include <map>
#include <ext/hash_map>
#include <unordered_map>        // c++/4.8.2/bits/unordered_map.h
#include <tr1/unordered_map>    // c++/4.8.2/tr1/unordered_map.h
#include <boost/unordered_map.hpp>

#include <naesala/third/alignhash.h>
#include <naesala/common/range.h>

using namespace naesala;

// Hash = std::hash<Key>
// KeyEqual = std::equal_to<Key>
// Allocator = std::allocator< std::pair<const Key, T> >

class Duration {
public:
    Duration(std::string const& prefix) :
        _prefix(prefix), _start(std::chrono::system_clock::now()) {}
    ~Duration() {
        std::chrono::time_point<std::chrono::system_clock> end(std::chrono::system_clock::now());
        std::chrono::duration<double> elapsed_seconds = end - _start;
        std::cout << _prefix << ": "
                << elapsed_seconds.count() << "s\t| ";
    }

private:
    std::string _prefix;
    std::chrono::time_point<std::chrono::system_clock> _start;
};

template <class Map>
class suite {
public:
    void test(std::string const& prefix) {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "[" << /* typeid(_map).name() */ prefix << "]" << std::endl;
        test_insert();
        test_traverse();
        test_find();
        test_delete();
        std::cout << std::endl;
    }

private:
    void test_insert() {
        Duration duration("insert");
        for (int i : range(OP_TIMES)) {
            _map[KEY_POS + rand() % KEY_SPAN] = i;
        }
    }
    void test_traverse() {
        Duration duration("traverse");
        for (typename Map::iterator ite(_map.begin()); ite != _map.end(); ++ite) {
            auto i = *ite;
            (void) i; // suppress unused warning
        }
    }
    void test_find() {
        Duration duration("find");
        for (int i : range(OP_TIMES)) {
            int j = _map[KEY_POS + rand() % KEY_SPAN];
            (void) j; // suppress unused warning
        }
    }
    void test_delete() {
        Duration duration("delete");
        for (int i : range(KEY_SPAN)) {
            _map.erase(KEY_POS + rand() % KEY_SPAN);
        }
    }

    static int const OP_TIMES = 1000000;
    static int const KEY_POS = 0xcdcdcdcd;
    static int const KEY_SPAN = OP_TIMES * 2;
    Map _map;
};

int main(int argc, char* argv[]) {
    suite<std::map<int, int>>().test("std::map");
    suite<__gnu_cxx::hash_map<int, int>>().test("__gnu_cxx::hash_map");
    suite<std::unordered_map<int, int>>().test("std::unordered_map");
    suite<std::tr1::unordered_map<int, int>>().test("std::tr1::unordered_map");
    suite<boost::unordered_map<int, int>>().test("boost::unordered_map");
    suite<align_hash_map<int, int>>().test("align_hash_map");

    return 0;
}

/*

[root@localhost build]# ./hashmap
--------------------------------
[std::map]
insert: 1.33993s        | traverse: 0.089017s   | find: 1.24957s        | delete: 2.63042s      |
--------------------------------
[__gnu_cxx::hash_map]
insert: 0.42164s        | traverse: 0.113012s   | find: 0.238216s       | delete: 0.404426s     |
--------------------------------
[std::unordered_map]
insert: 0.527232s       | traverse: 0.042736s   | find: 0.530173s       | delete: 0.60149s      |
--------------------------------
[std::tr1::unordered_map]
insert: 0.35997s        | traverse: 0.075255s   | find: 0.390161s       | delete: 0.371356s     |
--------------------------------
[boost::unordered_map]
insert: 0.651387s       | traverse: 0.036531s   | find: 0.692967s       | delete: 0.785001s     |
--------------------------------
[align_hash_map]
insert: 0.159173s       | traverse: 0.041413s   | find: 0.169958s       | delete: 0.150414s     |

*/


