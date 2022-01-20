
#include "stdinc.hpp"

//#include <random>

#include <catch2/catch.hpp>

//#include <filesystem>

#include "cli/read-config-file.hpp"
#include "eval/eval-context.hpp"

// #include "eval/integer.hpp"
// #include "parser/parser-internal.hpp"
// #include "scanner/scanner.hpp"

namespace giraffe
{
std::filesystem::path create_temporary_directory(uint32_t max_tries = 1000) noexcept(false)
{
   auto tmp_dir = std::filesystem::temp_directory_path();
   uint32_t i   = 0;
   std::random_device dev;
   std::mt19937 prng(dev());
   std::uniform_int_distribution<uint64_t> rand(0);
   std::filesystem::path path;
   while(true) {
      std::stringstream ss;
      ss << std::hex << rand(prng);
      path = tmp_dir / ss.str();
      // true if the directory was created.
      if(std::filesystem::create_directory(path)) { break; }
      if(i == max_tries) { throw std::runtime_error("could not find non-existing directory"); }
      i++;
   }
   return path;
}
} // namespace giraffe

namespace giraffe::test::detail030
{
constexpr string_view file1 = R"V0G0N(

#pragma once

#ifndef INCLUDE_GUARD
#define INCLUDE_GUARD

#include <cstdio>
#include <iostream>

#include "empty-file.hpp"

#endif

)V0G0N";

} // namespace giraffe::test::detail030

namespace giraffe::test
{

std::pair<vector<IncludePath>, SymbolTable> default_testing_config()
{
   auto data = ConfigFileData::read("tests/test-files/g++-config.text");
   data.include_paths.insert(begin(data.include_paths), {"tests/test-files/include", false});

   SymbolTable symbol_table;
   for(const auto& [key, value] : data.defines) symbol_table.insert(key, {}, value);

   return {data.include_paths, symbol_table};
}

// ---------------------------------------------------------------------------------------- testcase

CATCH_TEST_CASE("030 Evaluate1", "[030-evaluate-1]")
{
   const auto driver_opts                   = DriverOptions{};
   const auto [include_paths, symbol_table] = default_testing_config();

   CATCH_SECTION("30.1")
   {
      auto source = make_unique<StringScannerInput>("test-30", detail030::file1);
      // auto ctx = EvalContext::evaluate(std::move(source), include_paths, symbol_table,
      // driver_opts);
   }
}

} // namespace giraffe::test
