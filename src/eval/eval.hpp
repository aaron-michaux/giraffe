
#pragma once

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
   using context_lookup_type = unordered_map<sso23::string, unique_ptr<Context>, sso23::hasher>;
   using string_set_type     = unordered_set<sso23::string, sso23::hasher>;

 private:
   SymbolTable symbols_          = {}; //!< Currently active symbol table
   DriverOptions driver_opts_    = {}; //!< Passed command line options, including include paths
   context_lookup_type contexts_ = {}; //!< All the files we've parsed
   string_set_type include_deps_ = {}; //!< The include dependencies for the current file

   // We keep a stack of context pointers

 public:
   //@{ Getters
   auto& symbols() noexcept { return symbols_; }

   const auto& symbols() const noexcept { return symbols_; }
   const auto& driver_opts() const noexcept { return driver_opts_; }
   const auto& contexts() const noexcept { return contexts_; }

   Context& current_context() noexcept;
   //@}
};

void eval_stmts(EvalContext& eval_ctx, const StmtListNode* stmt_list) noexcept;

} // namespace giraffe
