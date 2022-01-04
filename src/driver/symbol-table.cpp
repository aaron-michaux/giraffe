
#include "stdinc.hpp"

#include "symbol-table.hpp"

#define This SymbolTable

namespace giraffe
{

bool This::has(string_view symbol) const noexcept
{
   auto ii = data_.find(symbol);
   return ii != cend(data_);
}

void This::insert(string_view symbol, string_view text) noexcept { insert(symbol, {}, text); }

void This::insert(string_view symbol, vector<sso23::string> arglist, string_view text) noexcept
{
   data_[symbol] = Symbol{symbol, arglist, text};
}

void This::remove(string_view symbol) noexcept { data_.erase(symbol); }

sso23::string This::eval(string_view) const noexcept(false) { return ""; }

} // namespace giraffe

#undef This
