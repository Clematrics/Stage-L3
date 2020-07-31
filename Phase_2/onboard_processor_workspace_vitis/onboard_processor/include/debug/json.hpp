#pragma once

#undef str

#include "dependencies/nlohmann/json.hpp"
#include "dependencies/tsl/ordered_map.h"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

template<class Key, class T, class Ignore, class Allocator,
         class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>,
         class AllocatorPair = typename std::allocator_traits<Allocator>::template rebind_alloc<std::pair<Key, T>>,
         class ValueTypeContainer = std::vector<std::pair<Key, T>, AllocatorPair>>
using ordered_map = tsl::ordered_map<Key, T, Hash, KeyEqual, AllocatorPair, ValueTypeContainer>;

using json = nlohmann::basic_json<ordered_map>;
