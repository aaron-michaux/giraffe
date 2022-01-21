
#pragma once

#include "scanner/token.hpp"

namespace giraffe
{

struct Symbol
{
   sso23::string symbol          = {};
   vector<sso23::string> arglist = {};
   vector<Token> token_sequence  = {};
};

struct SymbolTable
{
 private:
   unordered_map<sso23::string, Symbol, sso23::hash<sso23::string>> data_;

 public:
   bool has(string_view) const noexcept;

   void insert(string_view symbol) noexcept;
   void insert(string_view symbol, //
               vector<sso23::string> arglist,
               vector<Token> token_sequence) noexcept;
   void remove(string_view) noexcept;
};

} // namespace giraffe
