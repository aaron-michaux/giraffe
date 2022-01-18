
#pragma once

namespace giraffe
{
struct DriverOptions final
{
   bool color_diagnostics = true;
   bool w_error           = false;
   bool suppress_warnings = false;
   bool skip_isystem      = false
};

} // namespace giraffe
