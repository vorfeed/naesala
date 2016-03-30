// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#ifndef NAESALA_HASHMAP_LINEAR_HASHMAP_
#define NAESALA_HASHMAP_LINEAR_HASHMAP_

#pragma once

#include <cassert>
#include <cmath>
#include <limits>

#include <naesala/common/likely.h>
#include <naesala/common/range.h>

namespace naesala {

template<class Key, class Value,
          class Hash = std::hash<Key>,
          class Pred = std::equal_to<Key>,
          class Alloc = std::allocator<std::pair<const Key, Value>>>
class LinearHashmap {
 public:
  typedef Key                  key_type;
  typedef Value                mapped_type;
  typedef std::pair<const Key, Value> value_type;
  typedef Hash                 hasher;
  typedef Pred                 key_equal;
  typedef Alloc                allocator_type;

  typedef std::size_t          size_type;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef const value_type&    const_reference;
  typedef value_type*          pointer;
  typedef const value_type*    const_pointer;

  template <class Container, class IterValue> struct Iterator;
  typedef Iterator<LinearHashmap, value_type>             iterator;
  typedef Iterator<const LinearHashmap, const value_type> const_iterator;

  typedef typename Alloc::template rebind<Key>::other   key_allocator_type;
  typedef typename Alloc::template rebind<Value>::other mapped_allocator_type;

  template <class T = Key, typename std::enable_if<std::is_integral<T>::value ||
      std::is_floating_point<T>::value>::type* = nullptr>
  LinearHashmap(size_t init_bucket_count = 0,
      const T& empty_key = std::numeric_limits<T>::max(),
      const T& erased_key = std::numeric_limits<T>::max() - 1) :
      empty_key_(empty_key), erased_key_(erased_key) {
    assert(empty_key_ != erased_key_);
    if (init_bucket_count) {
      Resize(NextPowTwo(init_bucket_count));
    }
  }

  template <class T = Key,
      typename std::enable_if<std::is_convertible<T, std::string>::value>::type* = nullptr>
  LinearHashmap(size_t init_bucket_count = 0, const T& empty_key = "",
      const T& erased_key = {1}) : empty_key_(empty_key), erased_key_(erased_key) {
    assert(empty_key_ != erased_key_);
    if (init_bucket_count) {
      Resize(NextPowTwo(init_bucket_count));
    }
  }

  LinearHashmap(size_t init_bucket_count, const Key& empty_key,
      const Key& erased_key) : empty_key_(empty_key), erased_key_(erased_key) {
    assert(empty_key_ != erased_key_);
    if (init_bucket_count) {
      Resize(NextPowTwo(init_bucket_count));
    }
  }

  ~LinearHashmap() {
    Destory();
  }

  LinearHashmap(const LinearHashmap& other) :
      empty_key_(other.empty_key_), erased_key_(other.erased_key_) {
    if (!other.bucket_count_) {
      return;
    }
    UninitializedCopy(other);
  }
  LinearHashmap& operator=(const LinearHashmap& other) {
    assert(empty_key_ == other.empty_key_ && erased_key_ == other.erased_key_);
    if (unlikely(&other == this)) {
      return *this;
    }
    if (!other.bucket_count_) {
      Destory();
      return *this;
    }
    if (bucket_count_ == other.bucket_count_) {
      InitializedCopy(other);
    } else {
      Destory();
      UninitializedCopy(other);
    }
    return *this;
  }

  LinearHashmap(LinearHashmap&& other) noexcept :
      empty_key_(other.empty_key_), erased_key_(other.erased_key_),
      bucket_count_(other.bucket_count_), resize_count_(other.resize_count_),
      shrink_count_(other.shrink_count_), mask_(other.mask_), size_(other.size_),
      used_(other.used_), buckets_(other.buckets_) {
    other.buckets_ = nullptr;
    other.bucket_count_ = other.resize_count_ = other.shrink_count_ =
        other.mask_ = other.size_ = other.used_ = 0;
  }
  LinearHashmap& operator=(LinearHashmap&& other) noexcept {
    assert(empty_key_ == other.empty_key_ && erased_key_ == other.erased_key_);
    if (unlikely(&other == this)) {
      return *this;
    }
    Destory();
    swap(other);
    return *this;
  }

  iterator begin() {
    for (size_t index : sequence(bucket_count_)) {
      if (Exists(index)) {
        return iterator(this, index);
      }
    }
    return end();
  }
  const_iterator begin() const {
    for (size_t index : sequence(bucket_count_)) {
      if (Exists(index)) {
        return const_iterator(this, index);
      }
    }
    return end();
  }

  iterator end() {
    return iterator(this, bucket_count_);
  }
  const_iterator end() const {
    return const_iterator(this, bucket_count_);
  }

  iterator find(const Key& key) {
    return iterator(this, Get(key));
  }
  const_iterator find(const Key& key) const {
//    return const_cast<LinearHashmap*>(this)->find(key);
    return const_iterator(this, Get(key));
  }

  template <class KV>
  iterator insert(KV&& kv) {
    return emplace(std::forward<KV>(kv));
  }
  template <class InputIter>
  void insert(InputIter begin, InputIter end) {
    for (auto ite = begin; ite != end; ++ite) {
      Set(ite->first, ite->second, false);
    }
  }

  template <class K, class V>
  iterator emplace(K&& key, V&& value) {
    return iterator(this, Set(std::forward<K>(key), std::forward<V>(value), false));
  }
  template <class KV>
  iterator emplace(KV&& kv) {
    return iterator(this, Set(std::forward<typename PairTraits<KV>::first_type>(kv.first),
        std::forward<typename PairTraits<KV>::second_type>(kv.second), false));
  }

  template <class KV>
  iterator insert_or_assign(KV&& kv) {
    return iterator(this, Set(std::forward<typename PairTraits<KV>::first_type>(kv.first),
        std::forward<typename PairTraits<KV>::second_type>(kv.second), true));
  }
  template <class InputIter>
  void insert_or_assign(InputIter begin, InputIter end) {
    for (auto ite = begin; ite != end; ++ite) {
      Set(ite->first, ite->second, true);
    }
  }

  void erase(const Key& key) {
    Delete(Get(key));
  }
  void erase(const_iterator& pos) {
    Delete(pos.current_);
  }
  void erase(iterator& pos) {
    Delete(pos.current_);
  }

  mapped_type& operator[](const Key& key) {
//    return buckets_[Set(key, Value(), false)].second; // address changed in gcc 4.9.1, why?
    size_t index = Set(key, DummyValue(), false);
    return buckets_[index].second;
  }
  mapped_type& operator[](Key&& key) {
    size_t index = Set(std::move(key), DummyValue(), false);
    return buckets_[index].second;
  }

  size_type size() const {
    return size_;
  }

  bool empty() const {
    return !size();
  }

  void clear() {
    for (size_t index : sequence(bucket_count_)) {
      Clear(index);
    }
    size_ = used_ = 0;
  }

  size_type count(const Key& key) const {
    return Get(key) != bucket_count_;
  }

  size_type bucket_count() const {
    return bucket_count_;
  }

  key_equal key_eq() const {
    return key_equal();
  }

  hasher hash_function() const {
    return hasher();
  }

  float load_factor() const {
    return kLoadFactor;
  }

  void swap(LinearHashmap& other) {
    assert(empty_key_ == other.empty_key_ && erased_key_ == other.erased_key_);
    std::swap(bucket_count_, other.bucket_count_);
    std::swap(resize_count_, other.resize_count_);
    std::swap(shrink_count_, other.shrink_count_);
    std::swap(mask_, other.mask_);
    std::swap(size_, other.size_);
    std::swap(used_, other.used_);
    std::swap(buckets_, other.buckets_);
  }

 private:
  // deprecated
  class BitFlags;

  template <class Pair, class Enable = void> struct PairTraits;
  template <class Pair>
  struct PairTraits<Pair,
      typename std::enable_if<std::is_lvalue_reference<Pair>::value>::type> {
    typedef const typename std::decay<Pair>::type::first_type& first_type;
    typedef const typename std::decay<Pair>::type::second_type& second_type;
  };
  template <class Pair>
  struct PairTraits<Pair,
      typename std::enable_if<!std::is_lvalue_reference<Pair>::value>::type> {
    typedef typename std::decay<Pair>::type::first_type&& first_type;
    typedef typename std::decay<Pair>::type::second_type&& second_type;
  };

  static constexpr float kLoadFactor = 0.85;
  static constexpr float kShrinkFactor = 0.2;
  static const Value& DummyValue() { static Value dummy_; return dummy_; }

  static constexpr int FindLastSet(unsigned x) {
    return x ? sizeof(x) * 8 - __builtin_clz(x) : 0;  // count leading zeros
  }

  static constexpr int NextPowTwo(unsigned x) {
    return x ? (1ul << FindLastSet(x - 1)) : 1;
  }

  static size_t ResizeCount(size_t bucket_count, float factor) {
//    return (std::floor(bucket_count * kLoadFactor) + 7) & ~7;
    if (!bucket_count) {
      return 0;
    }
    size_t resize_count = std::floor(bucket_count * factor);
    return resize_count < bucket_count ? resize_count : bucket_count - 1;
  }

  size_t Get(const Key& key) const;

  template <class K, class V>
  size_t Set(K&& key, V&& value, bool displace);

  void Delete(size_t index);

  bool Resize(size_t new_bucket_count);

  bool Deleted(size_t index) const {
    return buckets_[index].first == erased_key_;
  }
  bool Empty(size_t index) const {
    return buckets_[index].first == empty_key_;
  }
  bool DeletedOrEmpty(size_t index) const {
    return Deleted(index) || Empty(index);
  }
  bool Exists(size_t index) const {
    return !DeletedOrEmpty(index);
  }

  void SetDeleted(size_t index) {
    assert(buckets_[index].first != empty_key_ && buckets_[index].first != erased_key_);
    *const_cast<Key*>(&buckets_[index].first) = erased_key_;
    mapped_allocator_type().destroy(&buckets_[index].second);
  }
  void SetEmpty(size_t index) {
    key_allocator_type().construct(&buckets_[index].first, empty_key_);
  }
  void Clear(size_t index) {
    if (buckets_[index].first == empty_key_) {
      return;
    } else if (buckets_[index].first != erased_key_) {
      mapped_allocator_type().destroy(&buckets_[index].second);
    }
    *const_cast<Key*>(&buckets_[index].first) = empty_key_;
  }

  void Destory() {
    if (!bucket_count_) {
      return;
    }
    for (size_t index : sequence(bucket_count_)) {
      if (Exists(index)) {
        mapped_allocator_type().destroy(&buckets_[index].second);
      }
      key_allocator_type().destroy(&buckets_[index].first);
    }
    allocator_type().deallocate(buckets_, bucket_count_);
    buckets_ = nullptr;
    bucket_count_ = resize_count_ = shrink_count_ = mask_ = size_ = used_ = 0;
  }

  void UninitializedCopy(const LinearHashmap& from) {
    assert(!bucket_count_ && !buckets_);
    buckets_ = allocator_type().allocate(from.bucket_count_);
    bucket_count_ = from.bucket_count_;
    resize_count_ = from.resize_count_;
    shrink_count_ = from.shrink_count_;
    mask_ = bucket_count_ - 1;
    for (size_t index : sequence(bucket_count_)) {
      key_allocator_type().construct(&buckets_[index].first, from.buckets_[index].first);
      if (Exists(index)) {
        mapped_allocator_type().construct(&buckets_[index].second, from.buckets_[index].second);
      }
    }
    size_ = from.size_;
    used_ = from.used_;
  }

  void InitializedCopy(const LinearHashmap& from) {
    assert(from.bucket_count_ == bucket_count_);
    for (size_t index : sequence(bucket_count_)) {
      if (from.buckets_[index].first == buckets_[index].first) {
        if (Exists(index)) {
          buckets_[index].second = from.buckets_[index].second;
        }
      } else {
        Clear(index);
        *const_cast<Key*>(&buckets_[index].first) = from.buckets_[index].first;
        if (Exists(index)) {
          mapped_allocator_type().construct(&buckets_[index].second, from.buckets_[index].second);
        }
      }
    }
    size_ = from.size_;
    used_ = from.used_;
  }

  // TODO: remove it
  const Key empty_key_;
  const Key erased_key_;
  size_t bucket_count_ { 0 }; // must be the power of 2, and minimal 8
  size_t resize_count_ { 0 };
  size_t shrink_count_ { 0 };
  size_t mask_ { 0 };         // bucket_count_ - 1
  size_t size_ { 0 };         // exists
  size_t used_ { 0 };         // exists and deleted
  value_type* buckets_ { nullptr };
};

} // namespace naesala

#include "linear_hashmap-inl.h"

#endif // NAESALA_HASHMAP_LINEAR_HASHMAP_
