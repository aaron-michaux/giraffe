
#pragma once

#include "context.hpp"

namespace giraffe
{
bool execute(unique_ptr<ScannerInputInterface>&& input,
             vector<IncludePath> include_paths,
             SymbolTable initial_symbol_table,
             std::ostream* output_stream,
             DriverOptions opts = {}) noexcept;
}
