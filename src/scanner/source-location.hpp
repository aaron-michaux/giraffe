
#pragma once

namespace giraffe
{
struct SourceLocation final
{
   uint32_t offset  = 0; // offset in buffer
   uint32_t line_no = 0; // source line-number

   constexpr SourceLocation(uint32_t off = 0, uint32_t line = 0) noexcept
   {
      offset  = off;
      line_no = line;
   }
   constexpr SourceLocation(SourceLocation&&)      = default;
   constexpr SourceLocation(const SourceLocation&) = default;
   constexpr ~SourceLocation()                     = default;
   constexpr SourceLocation& operator=(SourceLocation&&) = default;
   constexpr SourceLocation& operator=(const SourceLocation&) = default;

   // NEED TESTCASES
   constexpr bool operator==(const SourceLocation& o) const noexcept
   {
      return (offset == o.offset);
   }
   constexpr bool operator!=(const SourceLocation& o) const noexcept
   {
      return !(*this == o);
   }
   constexpr bool operator<(const SourceLocation& o) const noexcept
   {
      return (offset < o.offset);
   }
   constexpr bool operator<=(const SourceLocation& o) const noexcept
   {
      return !(o < *this);
   }
   constexpr bool operator>(const SourceLocation& o) const noexcept { return o < *this; }
   constexpr bool operator>=(const SourceLocation& o) const noexcept
   {
      return !(*this < o);
   }
};

static_assert(sizeof(SourceLocation) == 8);

inline string str(SourceLocation loc) noexcept
{
   return format("[{}, {}]", int(loc.offset), int(loc.line_no));
}

using SourceRange = std::pair<SourceLocation, SourceLocation>; // [start..end)

constexpr inline bool is_empty(const SourceRange& s) noexcept
{
   return !(s.first < s.second);
}

inline string str(SourceRange range) noexcept
{
   return format("Range {} => {}", str(range.first), str(range.second));
}

} // namespace giraffe
