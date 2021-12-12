
#pragma once

namespace giraffe
{
std::ostream& encode_char(std::ostream& ss, char val) noexcept;
std::string encoded_char(char val) noexcept;
std::string encode_string(std::string_view val) noexcept;
} // namespace giraffe
