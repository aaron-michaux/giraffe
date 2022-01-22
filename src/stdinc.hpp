
#ifndef GIRAFFE__STDINC_HPP__INCLUDE_GUARD__
#define GIRAFFE__STDINC_HPP__INCLUDE_GUARD__

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef CATCH_BUILD
#include <unistd.h>
#endif

#include <sso-string/string.hpp>

#include "fmt/format.h"

#include "utils/concepts.hpp"
#include "utils/encode-char.hpp"
#include "utils/file-put-contents.hpp"
#include "utils/for-each-adjacent-pair.hpp"
#include "utils/implode.hpp"
#include "utils/in-list.hpp"
#include "utils/is-regular-file.hpp"
#include "utils/is-reverse-iterator.hpp"
#include "utils/remove-duplicates.hpp"
#include "utils/to-array.hpp"

namespace giraffe
{
using namespace std::string_literals;

using ssize_t = std::make_signed_t<std::size_t>;

using std::array;
using std::make_unique;
using std::string_view;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using fmt::format;
using std::function;

using std::cout;
using std::endl;
using std::ostream;

using std::begin;
using std::cbegin;
using std::cend;
using std::crbegin;
using std::crend;
using std::end;
using std::rbegin;
using std::rend;

using string_set_type = unordered_set<sso23::string, sso23::hasher>;

#ifdef NDEBUG
#define TRACE(msg)
#else
#define TRACE(msg)                                                                             \
   {                                                                                           \
      std::cout << format("\x1b[97m\x1b[42mTRACE\x1b[0m {}:{}: {}", __FILE__, __LINE__, (msg)) \
                << std::endl;                                                                  \
   }
#endif

#define FATAL(msg)                                                                             \
   {                                                                                           \
      std::cerr << format("\x1b[97m\x1b[41mFATAL\x1b[0m {}:{}: {}", __FILE__, __LINE__, (msg)) \
                << std::endl;                                                                  \
      assert(false);                                                                           \
      exit(1);                                                                                 \
   }

} // namespace giraffe

#endif
