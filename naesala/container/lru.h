// Copyright 2016, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <list>
#include <unordered_map>

namespace naesala {

template <class Key, class Value>
class LRU {
 public:
  LRU(size_t capacity, const Value& not_exist = Value()) :
    capacity_(capacity), kNotExist_(not_exist) {}

  Value& Get(const Key& key) {
    auto ite = index_.find(key);
    if (ite == index_.end()) {
      return kNotExist_;
    }
    cache_.splice(std::next(ite->second, 1), cache_, cache_.begin(), ite->second);
    return ite->second->second;
  }

  template <class T>
  void Set(const Key& key, T&& value) {
    auto ite = index_.find(key);
    if (ite != index_.end()) {
      cache_.splice(std::next(ite->second, 1), cache_, cache_.begin(), ite->second);
      ite->second->second = std::forward<T>(value);
      return;
    }
    if (index_.size() == capacity_) {
      index_.erase(cache_.back().first);
      cache_.pop_back();
    }
    cache_.emplace_front(key, std::forward<T>(value));
    index_.emplace(key, cache_.begin());
  }

 private:
  typedef std::list<std::pair<Key, Value>> Cache;
  typedef std::unordered_map<Key, Cache::iterator> Index;

  size_t capacity_;
  const Value& kNotExist_;
  Cache cache_;
  Index index_;
};

}
