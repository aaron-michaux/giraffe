
#pragma once

namespace giraffe
{

struct Symbol
{
   sso23::string symbol          = {};
   vector<sso23::string> arglist = {};
   sso23::string text            = {};
};

struct SymbolTable
{
 private:
   unordered_map<sso23::string, Symbol, sso23::hash<sso23::string>> data_;

 public:
   bool has(string_view) const noexcept;

   void insert(string_view symbol, string_view text = "") noexcept;
   void insert(string_view symbol, vector<sso23::string> arglist, string_view text) noexcept;
   void remove(string_view) noexcept;

   sso23::string eval(string_view) const noexcept(false);
};

} // namespace giraffe
