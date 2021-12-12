
#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <string_view>

namespace giraffe
{
// --------------------------------------------------------------------- Implode

template<typename InputIt, typename F>
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

template<typename InputIt>
std::string implode(InputIt first, InputIt last, const std::string_view glue)
{
   auto f = [](const decltype(*first)& v) -> auto& { return v; };
   return implode(first, last, glue, f);
}

namespace range {
template<typename Range, typename F>
std::string implode(Range&& range, const std::string_view glue, F&& f)
{
   return ::giraffe::implode(begin(range), end(range), glue, std::forward<F&&>(f));
}

template<typename Range>
std::string implode(Range&& range, const std::string_view glue)
{
   return ::giraffe::implode(begin(range), end(range), glue);
}
}

} // namespace giraffe
