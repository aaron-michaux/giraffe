
#pragma once

namespace giraffe
{
struct ScannerOptions final
{
   bool skip_start_symbol = true;
   bool skip_whitespace   = true;
   bool skip_newlines     = false;
   bool skip_comments     = true;
};

} // namespace giraffe
