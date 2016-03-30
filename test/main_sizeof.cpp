// Copyright 2016, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#include <iostream>

int main(int argc, char* argv[]) {
  std::cout << "sizeof(bool): " << sizeof(bool) << std::endl;
  std::cout << "sizeof(char): " << sizeof(char) << std::endl;
  std::cout << "sizeof(short): " << sizeof(short) << std::endl;
  std::cout << "sizeof(int): " << sizeof(int) << std::endl;
  std::cout << "sizeof(long int): " << sizeof(long int) << std::endl;
  std::cout << "sizeof(long): " << sizeof(long) << std::endl;
  std::cout << "sizeof(long long): " << sizeof(long long) << std::endl;
  std::cout << "sizeof(float): " << sizeof(float) << std::endl;
  std::cout << "sizeof(double): " << sizeof(double) << std::endl;
  std::cout << "sizeof(long double): " << sizeof(long double) << std::endl;
  std::cout << "sizeof(size_t): " << sizeof(size_t) << std::endl;
  return 0;
}
