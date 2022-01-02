/******************************************************************/
/**
 * \file   string.hpp
 * \author Elliot Goodrich
 * \author Aaron Michaux
 *           constexpr everything
 *           removed reinterpret_cast
 *           initialize from iterators
 *           initialize from string_view
 *           constant iterators
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *********************************************************************/

#ifndef INCLUDE_GUARD_2EE24263_BD1F_4E5D_8CDA_A3217E867BF0
#define INCLUDE_GUARD_2EE24263_BD1F_4E5D_8CDA_A3217E867BF0

#include <algorithm>
#include <bit>
#include <climits>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <ostream>
#include <type_traits>

namespace sso23
{
template<typename CharT, typename Traits = std::char_traits<CharT>> class basic_string
{
   using UCharT = typename std::make_unsigned<CharT>::type;

 public:
   using traits_type            = Traits;
   using value_type             = CharT;
   using size_type              = std::size_t;
   using difference_type        = std::ptrdiff_t;
   using reference              = value_type&;
   using const_reference        = const value_type&;
   using pointer                = CharT*;
   using const_pointer          = const CharT*;
   using iterator               = pointer;
   using const_iterator         = const_pointer;
   using reverse_iterator       = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;

 public:
   constexpr basic_string() noexcept
       : basic_string{"", static_cast<size_type>(0)}
   {}

   constexpr basic_string(CharT const* string, size_type size)
   {
      if(size <= sso_capacity) {
         Traits::move(data_.sso.string, string, size);
         Traits::assign(data_.sso.string[size], static_cast<CharT>(0));
         set_sso_size(size);
      } else {
         data_.non_sso.ptr = new CharT[size + 1];
         Traits::move(data_.non_sso.ptr, string, size);
         Traits::assign(data_.non_sso.ptr[size], static_cast<CharT>(0));
         set_non_sso_data(size, size);
      }
   }

   constexpr basic_string(std::basic_string_view<CharT, Traits> sv)
       : basic_string{sv.data(), sv.size()}
   {}

   template<std::input_iterator InputIt>
   constexpr basic_string(InputIt first, InputIt last)
       : basic_string{std::basic_string_view<CharT, Traits>{first, last}}
   {}

   constexpr basic_string(CharT const* string)
       : basic_string{string, std::strlen(string)}
   {}

   constexpr basic_string(const basic_string& string)
   {
      if(string.sso()) {
         data_.sso = string.data_.sso;
      } else {
         new(this) basic_string{string.data(), string.size()};
      }
   }

   constexpr basic_string(basic_string&& string) noexcept
   {
      data_ = string.data_;
      string.set_moved_from();
   }

   constexpr ~basic_string()
   {
      if(!sso()) { delete[] data_.non_sso.ptr; }
   }

   constexpr basic_string& operator=(basic_string const& other)
   {
      auto copy = other;
      swap(copy, *this);
      return *this;
   }

   constexpr basic_string& operator=(basic_string&& other) noexcept
   {
      this->~basic_string();
      data_ = other.data_;
      other.set_moved_from();
      return *this;
   }

   //@{ Element Access
   constexpr reference at(size_type pos) noexcept
   {
      if(pos >= size()) throw std::out_of_range{};
      return data()[pos];
   }
   constexpr const_reference at(size_type pos) const noexcept
   {
      if(pos >= size()) throw std::out_of_range{};
      return data()[pos];
   }

   constexpr reference operator[](size_type pos) noexcept { return data()[pos]; }
   constexpr const_reference operator[](size_type pos) const noexcept
   {
      return data()[pos];
   }

   constexpr reference front() noexcept { return data()[0]; }
   constexpr const_reference front() const noexcept { return data()[0]; }

   constexpr reference back() noexcept { return data()[size() - 1]; }
   constexpr const_reference back() const noexcept { return data()[size() - 1]; }

   constexpr CharT* data() noexcept
   {
      return sso() ? data_.sso.string : data_.non_sso.ptr;
   }

   constexpr CharT const* data() const noexcept
   {
      return sso() ? data_.sso.string : data_.non_sso.ptr;
   }

   operator std::basic_string_view<CharT, Traits>() const noexcept
   {
      return std::basic_string_view<CharT, Traits>{data(), size()};
   }
   //@}

   //@{ Iterators
   auto begin() noexcept { return data(); }
   auto begin() const noexcept { return data(); }
   auto rbegin() noexcept { return std::make_reverse_iterator(end()); }
   auto rbegin() const noexcept { return std::make_reverse_iterator(end()); }
   auto cbegin() const noexcept { return data(); }
   auto crbegin() const noexcept { return std::make_reverse_iterator(cend()); }

   auto end() noexcept { return data() + size(); }
   auto end() const noexcept { return data() + size(); }
   auto rend() noexcept { return std::make_reverse_iterator(begin()); }
   auto rend() const noexcept { return std::make_reverse_iterator(begin()); }
   auto cend() const noexcept { return data() + size(); }
   auto crend() const noexcept { return std::make_reverse_iterator(cbegin()); }
   //@}

   //@{ Capacity
   constexpr bool empty() const noexcept { return size() == 0; }

   constexpr size_type size() const noexcept
   {
      return sso() ? sso_size() : read_non_sso_data().first;
   }

   constexpr size_type length() const noexcept { return size(); }

   constexpr size_type capacity() const noexcept
   {
      return sso() ? (sizeof(data_) - 1) : read_non_sso_data().second;
   }
   //@}

   friend constexpr void swap(basic_string& lhs, basic_string& rhs)
   {
      std::swap(lhs.data_, rhs.data_);
   }

 private:
   constexpr void set_moved_from() noexcept { set_sso_size(0); }

   // We are using sso if the last two bits are 0
   constexpr bool sso() const noexcept
   {
      return !lsb_<0>(data_.sso.size) && !lsb_<1>(data_.sso.size);
   }

   // good
   constexpr void set_sso_size(unsigned char size) noexcept
   {
      data_.sso.size = static_cast<UCharT>(sso_capacity - size) << 2;
   }

   // good
   constexpr size_type sso_size() const noexcept
   {
      return sso_capacity - ((data_.sso.size >> 2) & 63u);
   }

   constexpr void set_non_sso_data(size_type size, size_type capacity)
   {
      auto& size_hsb           = most_sig_byte_(size);
      auto const size_high_bit = msb_<0>(size_hsb);

      auto& cap_hsb               = most_sig_byte_(capacity);
      auto const cap_high_bit     = msb_<0>(cap_hsb);
      auto const cap_sec_high_bit = msb_<1>(cap_hsb);

      set_msb_<0>(size_hsb, cap_sec_high_bit);

      cap_hsb <<= 2;
      set_lsb_<0>(cap_hsb, cap_high_bit);
      set_lsb_<1>(cap_hsb, !size_high_bit);

      data_.non_sso.size     = size;
      data_.non_sso.capacity = capacity;
   }

   constexpr std::pair<size_type, size_type> read_non_sso_data() const
   {
      auto size     = data_.non_sso.size;
      auto capacity = data_.non_sso.capacity;

      auto& size_hsb = most_sig_byte_(size);
      auto& cap_hsb  = most_sig_byte_(capacity);

      // Remember to negate the high bits
      auto const cap_high_bit     = lsb_<0>(cap_hsb);
      auto const size_high_bit    = !lsb_<1>(cap_hsb);
      auto const cap_sec_high_bit = msb_<0>(size_hsb);

      set_msb_<0>(size_hsb, size_high_bit);

      cap_hsb >>= 2;
      set_msb_<0>(cap_hsb, cap_high_bit);
      set_msb_<1>(cap_hsb, cap_sec_high_bit);

      return std::make_pair(size, capacity);
   }

 private:
   union Data
   {
      struct NonSSO
      {
         CharT* ptr;
         size_type size;
         size_type capacity;
      } non_sso;
      struct SSO
      {
         CharT string[sizeof(NonSSO) / sizeof(CharT) - 1];
         UCharT size;
      } sso;
   } data_;

   static_assert(sizeof(typename Data::SSO) == sizeof(typename Data::NonSSO));
   static_assert(sizeof(data_) == sizeof(void*) + 2 * sizeof(size_type));

 public:
   static constexpr size_type const sso_capacity
       = sizeof(typename Data::NonSSO) / sizeof(CharT) - 1;

 private:
   static constexpr size_type const high_bit_mask_
       = static_cast<size_type>(1) << (sizeof(size_type) * CHAR_BIT - 1);
   static constexpr size_type const sec_high_bit_mask_
       = static_cast<size_type>(1) << (sizeof(size_type) * CHAR_BIT - 2);

   template<typename T> static constexpr unsigned char& most_sig_byte_(T& obj)
   {
      return *(std::bit_cast<unsigned char*>(&obj) + sizeof(obj) - 1);
   }

   template<int N> static constexpr bool lsb_(unsigned char byte)
   {
      return byte & (1u << N);
   }

   template<int N> static constexpr bool msb_(unsigned char byte)
   {
      return byte & (1u << (CHAR_BIT - N - 1));
   }

   template<int N> static constexpr void set_lsb_(unsigned char& byte, bool bit)
   {
      if(bit) {
         byte |= 1u << N;
      } else {
         byte &= ~(1u << N);
      }
   }

   template<int N> static constexpr void set_msb_(unsigned char& byte, bool bit)
   {
      if(bit) {
         byte |= 1u << (CHAR_BIT - N - 1);
      } else {
         byte &= ~(1u << (CHAR_BIT - N - 1));
      }
   }
};

template<typename CharT, typename Traits>
constexpr auto operator<=>(const std::basic_string_view<CharT, Traits>& lhs,
                           const basic_string<CharT, Traits>& rhs) noexcept
{
   return std::lexicographical_compare_three_way(
       begin(lhs), end(lhs), begin(rhs), end(rhs));
}

template<typename CharT, typename Traits>
constexpr auto operator<=>(const CharT* lhs,
                           const basic_string<CharT, Traits>& rhs) noexcept
{
   return std::lexicographical_compare_three_way(
       lhs, lhs + strlen(lhs), begin(rhs), end(rhs));
}

template<typename CharT, typename Traits>
constexpr auto operator<=>(const basic_string<CharT, Traits>& lhs,
                           const basic_string<CharT, Traits>& rhs) noexcept
{
   return std::lexicographical_compare_three_way(
       begin(lhs), end(lhs), begin(rhs), end(rhs));
}

template<typename CharT, typename Traits>
std::ostream& operator<<(std::ostream& stream, const basic_string<CharT, Traits>& string)
{
   return stream << string.data();
}

using string = basic_string<char>;

static_assert(sizeof(string) == 3 * sizeof(std::size_t));
static_assert(std::is_nothrow_default_constructible<string>::value);
static_assert(std::is_nothrow_move_constructible<string>::value);
static_assert(std::is_nothrow_move_assignable<string>::value);

} // namespace sso23

#endif
