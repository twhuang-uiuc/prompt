#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <algorithm>
#include <iostream>
#include <cstring>
#include <string_view>
#include <random>
#include <unordered_set>

#include "prompt.hpp"

std::string gen_random(size_t range) {
  const size_t len {rand()%range + 1};
	static const char alphanum[] =
		"0123456789 -_"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

  std::string s(len, '0');

	for (size_t i=0; i<len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
  return s;
}


bool has_same_prefix(const prompt::RadixTree::Node& n){
  std::vector<std::string_view> sv;
  for(const auto& c: n.children){
    sv.emplace_back(c.first);
  }
  
  // Compare first character
  for(size_t i=1; i<sv.size(); i++){
    if(sv[i][0] == sv[0][0]){
      return true;
    }
  }

  for(const auto& c: n.children){
    if(has_same_prefix(*c.second)){
      return true;
    }
  }
  return false;
}


TEST_CASE("RadixTree") {

  srand(time(nullptr));

  prompt::RadixTree tree;
  const size_t word_num {500000};
  const size_t range {20};

  std::unordered_set<std::string> words;

  for(size_t i=0; i<word_num; i++){
    words.emplace(gen_random(range));
  }

  for(const auto& w: words){
    tree.insert(w);
  }

  // Check every word must exist
  for(const auto& w: words){
    REQUIRE(tree.exist(w));
  }

  // Check a unknown word must not exist
  for(const auto& w: words){
    if(w.size() > 1){
      size_t last_index {rand()%(w.size()) + 1};
      std::string s(w.data(), last_index);
      if(words.find(s) == words.end()){
        REQUIRE(not tree.exist(s));
      }
    }
  }

  auto ret = tree.all_words();
  REQUIRE(ret.size() == words.size());
  for(const auto& w: ret){
    words.erase(w);
  }

  REQUIRE(words.empty());

  const auto& root {tree.root()};
  REQUIRE(not has_same_prefix(root));
}


