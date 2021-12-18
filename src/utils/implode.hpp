
#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <ranges>
#include <iterator>

namespace giraffe
{
// --------------------------------------------------------------------- Implode

template<std::input_iterator InputIt, typename F>
std::string
implode(InputIt first, InputIt last, const std::string_view glue, F&& f)
{
   std::stringstream stream("");
   bool start = true;
   while(first != last) {
      if(start)
         start = false;
      else
         stream << glue;
      stream << f(*first++);
   }
   return stream.str();
}

template<std::input_iterator InputIt>
std::string implode(InputIt first, InputIt last, const std::string_view glue)
{
   auto f = [](const decltype(*first)& v) -> auto& { return v; };
   return implode(first, last, glue, f);
}

namespace range {
template<std::ranges::range Range, typename F>
std::string implode(Range&& range, const std::string_view glue, F&& f)
{
   return ::giraffe::implode(begin(range), end(range), glue, std::forward<F&&>(f));
}

template<std::ranges::range Range>
std::string implode(Range&& range, const std::string_view glue)
{
   return ::giraffe::implode(begin(range), end(range), glue);
}
}

} // namespace giraffe
