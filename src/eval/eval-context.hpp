
#pragma once

#include <stack>

#include "ast/ast.hpp"
#include "driver/context.hpp"
#include "driver/driver-options.hpp"

#include "symbol-table.hpp"

namespace giraffe
{

/**
 * Contains:
 *  1. A Symbol Table
 *  2. A place to collate results
 *  3. All of the Context objects encounted during parsing
 */
class EvalContext
{
 public:
   using string_set_type = unordered_set<sso23::string, sso23::hasher>;

 private:
   SymbolTable symbols_          = {}; //!< Currently active symbol table
   DriverOptions driver_opts_    = {}; //!< Passed command line options, including include paths
   string_set_type include_deps_ = {}; //!< The include dependencies for the current file
   std::stack<unique_ptr<Context>> context_stack_ = {}; //!< We keep a stack of context pointers

 public:
   //@{ Getters
   auto& symbols() noexcept { return symbols_; }

   const auto& symbols() const noexcept { return symbols_; }
   const auto& driver_opts() const noexcept { return driver_opts_; }

   Context& current_context() noexcept;
   //@}

   //@{ Actions
   //! Returns ""s if the file cannot be found.
   std::string resolve_include_path(string_view filename, bool is_local_include) const noexcept;

   void process_file(string_view filename) noexcept;
   //@}
};

} // namespace giraffe
