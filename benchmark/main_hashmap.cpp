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
#include <unordered_map>        // c++/4.9.1/bits/unordered_map.h
#include <tr1/unordered_map>    // c++/4.9.1/tr1/unordered_map.h
#include <boost/unordered_map.hpp>

#include <naesala/third/alignhash.h>
#include <naesala/common/range.h>
#include <naesala/hashmap/linear_hashmap.h>

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
            _map[KEY_POS + rand() & KEY_SPAN_MASK] = i;
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
            int j = _map[KEY_POS + rand() & KEY_SPAN_MASK];
            (void) j; // suppress unused warning
        }
    }
    void test_delete() {
        Duration duration("delete");
        for (int i : range(OP_TIMES)) {
            _map.erase(KEY_POS + rand() & KEY_SPAN_MASK);
        }
    }

    static int const OP_TIMES = 1 << 24;
    static int const KEY_POS = 0xcdcdcdcd;
    static int const KEY_SPAN_MASK = OP_TIMES * 2 - 1;
    Map _map;
};

int main(int argc, char* argv[]) {
    suite<std::map<int, int>>().test("std::map");
    suite<__gnu_cxx::hash_map<int, int>>().test("__gnu_cxx::hash_map");
    suite<std::unordered_map<int, int>>().test("std::unordered_map");
    suite<std::tr1::unordered_map<int, int>>().test("std::tr1::unordered_map");
    suite<boost::unordered_map<int, int>>().test("boost::unordered_map");
    suite<align_hash_map<int, int>>().test("align_hash_map");
    suite<LinearHashmap<int, int>>().test("LinearHashmap");

    return 0;
}

/*

gcc 4.9.1, boost 1.56, -O2
key不重复率：0.76
16,777,216次操作
--------------------------------
[std::map]
insert: 15.3459s        | traverse: 1.02469s    | find: 17.642s         | delete: 2.027s        | 
--------------------------------
[__gnu_cxx::hash_map]
insert: 4.07776s        | traverse: 1.062s      | find: 4.17001s        | delete: 0.33801s      | 
--------------------------------
[std::unordered_map]
insert: 5.132s          | traverse: 0.384036s   | find: 2.53397s        | delete: 0.569996s     | 
--------------------------------
[std::tr1::unordered_map]
insert: 3.94881s        | traverse: 0.303182s   | find: 1.924s          | delete: 0.334859s     | 
--------------------------------
[boost::unordered_map]
insert: 4.997s          | traverse: 0.442003s   | find: 3.85299s        | delete: 0.574989s     | 
--------------------------------
[align_hash_map]
insert: 2.00799s        | traverse: 0.0600757s  | find: 1.04993s        | delete: 0.0911587s    | 
--------------------------------
[LinearHashmap]
insert: 0.914718s       | traverse: 0.0471907s  | find: 0.746924s       | delete: 0.0620157s    | 

*/

