// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <iostream>
#include <chrono>
#include <ctime>

namespace naesala {

class Duration {
public:
    Duration(std::string const& prefix) :
        _prefix(prefix), _start(std::chrono::system_clock::now()) {}
    ~Duration() {
        std::chrono::time_point<std::chrono::system_clock> end(std::chrono::system_clock::now());
        std::chrono::duration<double> elapsed_seconds = end - _start;
        std::cout << _prefix << ": " << elapsed_seconds.count() << "s\t| ";
    }

private:
    std::string _prefix;
    std::chrono::time_point<std::chrono::system_clock> _start;
};

} // namespace naesala
