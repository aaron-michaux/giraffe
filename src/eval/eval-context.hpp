
#pragma once

#include <stack>

#include "ast/ast.hpp"
#include "driver/context.hpp"
#include "driver/driver-options.hpp"

#include "symbol-table.hpp"

namespace giraffe
{

struct ResolvedPath
{
   std::string filename = {};
   bool is_found        = false;
   bool is_isystem_path = false;
};

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
   bool has_error_                    = false;
   SymbolTable symbols_               = {}; //!< Currently active symbol table
   vector<IncludePath> include_paths_ = {}; //!< Configured include paths
   DriverOptions driver_opts_    = {}; //!< Passed command line options, including include paths
   string_set_type include_deps_ = {}; //!< The include dependencies for the current file
   std::stack<unique_ptr<Context>> context_stack_ = {}; //!< We keep a stack of context pointers

   std::ostream* ostream_ = nullptr; //!< Not owend

   void process_context_(unique_ptr<Context>&&) noexcept;

 public:
   //@{ Constructor
   static unique_ptr<EvalContext> make(vector<IncludePath> include_paths,
                                       SymbolTable initial_symbol_table,
                                       std::ostream* output_stream,
                                       DriverOptions opts = {}) noexcept;
   //@}

   //@{ Getters
   auto& symbols() noexcept { return symbols_; }
   const auto& symbols() const noexcept { return symbols_; }
   const auto& driver_opts() const noexcept { return driver_opts_; }
   Context& current_context() noexcept;
   bool has_error() const noexcept { return has_error_; }
   std::ostream& ostream() noexcept { return (ostream_ == nullptr) ? std::cout : *ostream_; }
   //@}

   //@{ Actions
   //! Returns ""s if the file cannot be found.
   ResolvedPath resolve_include_path(string_view filename, bool is_local_include) const noexcept;

   void process_include(string_view filename, bool is_isystem_path) noexcept;

   void process_input(unique_ptr<ScannerInputInterface>&& input) noexcept;
   //@}
};

} // namespace giraffe
