
#pragma once

#include "scanner/token-types.h"

namespace giraffe
{
// --------------------------------------------------------------------- modules
constexpr auto first_set_module = to_array<int>({ TMODULE, TEXPORT, TIMPORT });

constexpr auto first_set_command = to_array<int>({
      TDEFINE, TINCLUDE, TLINE, TUNDEF, TERROR
   });

constexpr auto first_set_error_warning = to_array<int>({
      TERROR, TWARNING
   });

constexpr auto first_set_ifthen = to_array<int>({
      TIF, TIFDEF, TIFNDEF
   });

constexpr auto stray_ifthen_parts = to_array<int>({
      TELIF, TELSE, TENDIF
   });


} // namespace giraffe
