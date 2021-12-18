
#pragma once

namespace giraffe
{
template<typename InputIt, typename F>
constexpr void for_each_adjacent_pair(InputIt bb, InputIt ee, F&& f)
{
   if(bb == ee) return; // empty
   for(auto ii = bb, jj = std::next(bb); jj != ee; ++ii, ++jj) f(ii, jj);
}
} // namespace giraffe
