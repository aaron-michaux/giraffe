
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

void This::insert(string_view symbol) noexcept { insert(symbol, {}, {}); }

void This::insert(string_view symbol,
                  vector<sso23::string> arglist,
                  vector<Token> token_sequence) noexcept
{
   data_[symbol] = {symbol, arglist, token_sequence};
}

void This::remove(string_view symbol) noexcept { data_.erase(symbol); }

} // namespace giraffe

#undef This
