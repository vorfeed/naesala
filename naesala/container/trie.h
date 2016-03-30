// Copyright 2016, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <cstring>
#include <array>
#include <string>

namespace naesala {

class Trie {
 public:
  ~Trie() {
    delete root_;
  }

  void Insert(const std::string& word) {
    TrieNode* p = root_;
    for (char c : word) {
      if (!p->next[c - 'a']) {
        p->next[c - 'a'] = new TrieNode();
      }
      p = p->next[c - 'a'];
    }
    p->is_word = true;
  }

  bool Search(const std::string& word) {
    TrieNode* p = Find(word);
    return p && p->is_word;
  }

  bool StartsWith(const std::string& prefix) {
    return Find(prefix);
  }

 private:
  struct TrieNode {
    ~TrieNode() {
      for (TrieNode* child_node : next) {
        delete child_node;
      }
    }
    TrieNode(bool b = false) {
      std::memset(next.data(), 0, sizeof(TrieNode*) * next.size());
      is_word = b;
    }
    std::array<TrieNode*, 26> next;
    bool is_word;
  };

  TrieNode* Find(const std::string& str) {
    TrieNode* p = root_;
    for (char c : str) {
      if (!p) {
        break;
      }
      p = p->next[c - 'a'];
    }
    return p;
  }

  TrieNode* root_ { new TrieNode() };
};

}
