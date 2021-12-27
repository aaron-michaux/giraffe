
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

constexpr auto first_set_else_elif_parts = to_array<int>({
      TELSE, TELIF, TELIFDEF, TELIFNDEF
   });

constexpr auto stray_ifthen_parts = to_array<int>({
      TELIF, TELSE, TELIFDEF, TELIFNDEF, TENDIF
   });

constexpr auto first_set_expressions = to_array<int>({
      TPLUS, TMINUS, TSHOUT, TTILDE, TINTEGER, TIDENTIFIER, TLPAREN, TDEFINED
   });

} // namespace giraffe
