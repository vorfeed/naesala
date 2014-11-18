// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#ifndef NAESALA_HASHMAP_LINEAR_HASHMAP_
#error "This should only be included by linear_hashmap.h"
#endif

#include <boost/iterator/iterator_facade.hpp>

namespace naesala {

template<class Key, class Value, class Hash, class Pred, class Alloc>
template <class Container, class IterValue>
struct LinearHashmap<Key, Value, Hash, Pred, Alloc>::Iterator
    : boost::iterator_facade<
      Iterator<Container, IterValue>,
      IterValue,
      boost::forward_traversal_tag
    > {
 public:
  Iterator() {}

  template <class OtherContainer, class OtherIterValue>
  Iterator(const Iterator<OtherContainer, OtherIterValue>& other,
      typename std::enable_if<std::is_convertible<OtherContainer, Container>::value>::type* = nullptr)
      : container_(other.container_), current_(other.current_) {}

  template <class OtherContainer, class OtherIterValue,
      typename std::enable_if<std::is_convertible<OtherContainer, Container>::value>::type* = nullptr>
  bool operator==(const Iterator<OtherContainer, OtherIterValue>& other) const {
    return container_ == other.container_ && current_ == other.current_;
  }

 private:
  friend class boost::iterator_core_access;
  friend class LinearHashmap;

  Iterator(Container* container, size_t current) : container_(container), current_(current) {}

  bool equal(const Iterator& other) const {
    return container_ == other.container_ && current_ == other.current_;
  }

  void increment() {
    if (likely(current_ != container_->bucket_count_)) {
      ++current_;
    }
    // TODO: optimize by exchanging sequence
    while (likely(current_ != container_->bucket_count_) && !container_->Exists(current_)) {
      ++current_;
    }
  }

  IterValue& dereference() const {
    return container_->buckets_[current_];
  }

  Container* container_ { nullptr };
  size_t current_ { 0 };
};

template<class Key, class Value, class Hash, class Pred, class Alloc>
size_t LinearHashmap<Key, Value, Hash, Pred, Alloc>::Get(const Key& key) const {
  if (unlikely(!bucket_count_)) {
    return bucket_count_;
  }
  size_t index = hasher()(key) & mask_;
  // ensure not full by ResizeCount
  while (Deleted(index) || (Exists(index) && !key_equal()(buckets_[index].first, key))) {
    index = (index + 1) & mask_;
  }
  return Empty(index) ? bucket_count_ : index;
}

template<class Key, class Value, class Hash, class Pred, class Alloc>
template <class K, class V>
size_t LinearHashmap<Key, Value, Hash, Pred, Alloc>::Set(K&& key, V&& value, bool displace) {
  if (used_ >= resize_count_) {
    // expand factor must be the index of 2
    Resize(bucket_count_ ? bucket_count_ * 2 : 8);
  }
  size_t index = hasher()(key) & mask_;
  // ensure not full by ResizeCount
  while (Exists(index) && !key_equal()(buckets_[index].first, key)) {
    index = (index + 1) & mask_;
  }
  if (Exists(index)) {
    if (!displace) {
      return index;
    }
    buckets_[index].second = std::forward<V>(value);
  } else {
    mapped_allocator_type().construct(&buckets_[index].second, std::forward<V>(value));
    ++size_;
    if (Empty(index)) {
      ++used_;
    }
  }
//  allocator_type().construct(buckets_ + index, { key, value }); // available in C++14
  *const_cast<Key*>(&buckets_[index].first) = std::forward<K>(key);
  return index;
}

template<class Key, class Value, class Hash, class Pred, class Alloc>
void LinearHashmap<Key, Value, Hash, Pred, Alloc>::Delete(size_t index) {
  if (unlikely(index >= bucket_count_) || DeletedOrEmpty(index)) {
    return;
  }
  SetDeleted(index);
  --size_;
}

template<class Key, class Value, class Hash, class Pred, class Alloc>
bool LinearHashmap<Key, Value, Hash, Pred, Alloc>::Resize(size_t new_bucket_count) {
  // TODO: whether need unlikely here
  if (!bucket_count_) {
    // initialize
    buckets_ = allocator_type().allocate(new_bucket_count);
    bucket_count_ = new_bucket_count;
    // new_bucket_count is already pow of 2
    // make sure new_bucket_count is larger than current bucket_count
    resize_count_ = ResizeCount(new_bucket_count);
    mask_ = bucket_count_ - 1;
    for (size_t index = 0; index < bucket_count_; ++index) {
      SetEmpty(index);
    }
    return true;
  }
  LinearHashmap new_lh(new_bucket_count, empty_key_, erased_key_);
  for (size_t index : sequence(bucket_count_)) {
    if (Exists(index)) {
      new_lh.Set(std::move(*const_cast<Key*>(&buckets_[index].first)), std::move(buckets_[index].second), false);
    }
  }
  swap(new_lh);
  return true;
}

} // namespace naesala

