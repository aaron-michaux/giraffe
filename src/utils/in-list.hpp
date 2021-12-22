
#pragma once

#include <type_traits>

namespace giraffe
{
constexpr bool in_list(int id, auto&& list) noexcept
{
   if constexpr(std::is_integral<std::remove_reference_t<decltype(list)>>::value) {
      return id == int(list);
   } else {
      return std::any_of(
          cbegin(list), cend(list), [&](auto&& x) { return in_list(id, x); });
   }
}
} // namespace giraffe
