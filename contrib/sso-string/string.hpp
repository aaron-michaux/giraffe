/******************************************************************/
/**
 * \file   string.hpp
 * \author Elliot Goodrich -- initial proof-of-concept
 * \author Aaron Michaux
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

#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
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

   using string_view_type = std::basic_string_view<CharT, Traits>;

   static constexpr size_type npos = -1;

 private:
   template<typename T, typename ResultT>
   using _enable_if_sv = std::enable_if_t<
       std::conjunction_v<std::is_convertible_v<const T&, string_view_type>,
                          std::negation_v<std::is_convertible<const T*, const basic_string*>>,
                          std::negation_v<std::is_convertible<const T&, const CharT*>>>,
       ResultT>;

   /** Create a string with a given capacity */
   constexpr basic_string(CharT const* string, size_type size, size_type capacity)
   {
      if(capacity < size) capacity = size;

      if(capacity <= sso_capacity) {
         Traits::move(data_.sso.string, string, size);
         Traits::assign(data_.sso.string[size], static_cast<CharT>(0));
         set_sso_size_(size);
      } else {
         data_.non_sso.ptr = new CharT[capacity + 1];
         Traits::move(data_.non_sso.ptr, string, size);
         Traits::assign(data_.non_sso.ptr[size], static_cast<CharT>(0));
         set_non_sso_data_(size, capacity);
      }
   }

 public:
   //@{ Construction
   constexpr basic_string() noexcept
       : basic_string{"", static_cast<size_type>(0)}
   {}

   constexpr basic_string(CharT const* string, size_type size)
       : basic_string{string, size, size}
   {}

   constexpr basic_string(string_view_type sv)
       : basic_string{sv.data(), sv.size()}
   {}

   template<std::input_iterator InputIt>
   constexpr basic_string(InputIt first, InputIt last)
       : basic_string{string_view_type{first, last}}
   {}

   constexpr basic_string(CharT const* string)
       : basic_string{string, std::strlen(string)}
   {}

   constexpr basic_string(const basic_string& string)
   {
      if(string.is_sso_()) {
         data_.sso = string.data_.sso;
      } else {
         new(this) basic_string{string.data(), string.size()};
      }
   }

   constexpr basic_string(basic_string&& string) noexcept
   {
      data_ = string.data_;
      string.set_moved_from_();
   }

   constexpr ~basic_string()
   {
      if(!is_sso_()) { delete[] data_.non_sso.ptr; }
   }
   //@}

   //@{ Assignment
   constexpr basic_string& operator=(basic_string const& other)
   {
      if(this != &other) {
         auto copy = other;
         swap(copy, *this);
      }
      return *this;
   }

   constexpr basic_string& operator=(basic_string&& other) noexcept
   {
      if(this != &other) {
         this->~basic_string();
         data_ = other.data_;
         other.set_moved_from_();
      }
      return *this;
   }

   constexpr basic_string& assign(size_type count, CharT ch) { return insert(cbegin(), count, ch); }
   constexpr basic_string& assign(const basic_string& str) { return *this = str; }
   constexpr basic_string& assign(const basic_string& str, size_type pos, size_type count = npos)
   {
      return insert(0, str, pos, count);
   }
   constexpr basic_string& assign(basic_string&& str) noexcept { return *this == std::move(str); }
   constexpr basic_string& assign(const CharT* s, size_type count) { return insert(0, s, count); }
   constexpr basic_string& assign(const CharT* s) { return insert(0, s); }
   template<class InputIt> constexpr basic_string& assign(InputIt first, InputIt last)
   {
      insert(cbegin(), first, last);
      return *this;
   }
   constexpr basic_string& assign(std::initializer_list<CharT> ilist)
   {
      insert(cbegin(), ilist);
      return *this;
   }
   template<class T> constexpr _enable_if_sv<T, basic_string&> assign(const T& t)
   {
      return insert(0, t);
   }
   template<class T>
   constexpr _enable_if_sv<T, basic_string&>
   assign(const T& t, size_type pos, size_type count = npos)
   {
      return insert(0, t, pos, count);
   }
   //@}

   //@{ Compare
   constexpr bool operator==(const basic_string& rhs) const noexcept { return lex_comp_(rhs) == 0; }
   constexpr bool operator!=(const basic_string& rhs) const noexcept { return !(*this == rhs); }
   constexpr auto operator<=>(const string_view_type& rhs) const noexcept { return lex_comp_(rhs); }
   constexpr auto operator<=>(const CharT* rhs) const noexcept { return lex_comp_(rhs); }
   constexpr auto operator<=>(const basic_string& rhs) const noexcept { lex_comp_(rhs); }
   //@}

   //@{ Element Access
   constexpr reference at(size_type pos) noexcept { return at_(pos); }
   constexpr const_reference at(size_type pos) const noexcept { return at_(pos); }
   constexpr reference operator[](size_type pos) noexcept { return data()[pos]; }
   constexpr const_reference operator[](size_type pos) const noexcept { return data()[pos]; }
   constexpr reference front() noexcept { return data()[0]; }
   constexpr const_reference front() const noexcept { return data()[0]; }
   constexpr reference back() noexcept { return data()[size() - 1]; }
   constexpr const_reference back() const noexcept { return data()[size() - 1]; }
   constexpr CharT* data() noexcept { return data_ptr_(); }
   constexpr CharT const* data() const noexcept { return data_ptr_(); }
   operator string_view_type() const noexcept { return string_view_type{data(), size()}; }
   //@}

   //@{ Iterators
   iterator begin() noexcept { return data(); }
   const_iterator begin() const noexcept { return data(); }
   const_iterator cbegin() const noexcept { return data(); }
   iterator end() noexcept { return data() + size(); }
   const_iterator end() const noexcept { return data() + size(); }
   const_iterator cend() const noexcept { return data() + size(); }

   reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(end()); }
   const_reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(end()); }
   const_reverse_iterator crbegin() const noexcept { return std::make_reverse_iterator(cend()); }
   reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }
   const_reverse_iterator rend() const noexcept { return std::make_reverse_iterator(begin()); }
   const_reverse_iterator crend() const noexcept { return std::make_reverse_iterator(cbegin()); }
   //@}

   //@{ Capacity
   constexpr bool empty() const noexcept { return size() == 0; }
   constexpr size_type size() const noexcept { return size_(); }
   constexpr size_type length() const noexcept { return size(); }
   constexpr size_type capacity() const noexcept { return capacity_(); }
   constexpr void reserve() { shrink_to_fit(); }
   constexpr void reserve(size_type new_cap) { set_capacity_(new_cap); }
   constexpr void shrink_to_fit() { set_capacity_(size()); }
   //@}

   //@{ Operations
   constexpr void clear() noexcept { set_new_size_(0); }
   constexpr void push_back(CharT ch) { push_back_(ch); }
   constexpr void pop_back(CharT ch) { set_new_size_(size() == 0 ? 0 : size() - 1); }
   constexpr void resize(size_type new_size) { set_new_size_(new_size); }
   constexpr void resize(size_type new_size, CharT ch)
   {
      const auto current_size = size();
      if(new_size < current_size) {
         set_new_size_(new_size);
      } else if(new_size > current_size) {
         insert(cend(), new_size - current_size, ch);
      }
   }
   //@}

   //@{ Operator +=
   constexpr basic_string& operator+=(const basic_string& str) { return insert(size(), str); }
   constexpr basic_string& operator+=(CharT ch) { return insert(size(), 1, ch); }
   constexpr basic_string& operator+=(const CharT* s) { return insert(size(), s); }
   constexpr basic_string& operator+=(std::initializer_list<CharT> ilist)
   {
      insert(cend(), ilist);
      return *this;
   }
   template<class T> constexpr _enable_if_sv<T, basic_string&> operator+=(const T& t)
   {
      return insert(size(), t);
   }

   constexpr basic_string operator+(const basic_string& rhs) const
   {
      basic_string ret = *this;
      ret += rhs;
      return ret;
   }
   constexpr basic_string operator+(CharT rhs) const
   {
      basic_string ret = *this;
      ret += rhs;
      return ret;
   }
   constexpr basic_string operator+(const CharT* rhs) const
   {
      basic_string ret = *this;
      ret += rhs;
      return ret;
   }
   template<class T> constexpr _enable_if_sv<T, basic_string> operator+(const T& rhs) const
   {
      basic_string ret = *this;
      ret += rhs;
      return ret;
   }
   //@}

   //@{ Append
   constexpr basic_string& append(size_type count, CharT ch) { return *this += ch; }
   constexpr basic_string& append(const basic_string& str) { return *this += str; }
   constexpr basic_string& append(const basic_string& str, size_type pos, size_type count = npos)
   {
      return insert(size(), str, pos, count);
   }
   constexpr basic_string& append(const CharT* s, size_type count)
   {
      return insert(size(), s, count);
   }
   constexpr basic_string& append(const CharT* s) { return insert(size(), s); }
   template<class InputIt> constexpr basic_string& append(InputIt first, InputIt last)
   {
      insert(cend(), first, last);
      return *this;
   }
   constexpr basic_string& append(std::initializer_list<CharT> ilist) { return *this += ilist; }
   template<class T> constexpr _enable_if_sv<T, basic_string&> append(const T& t)
   {
      return insert(size(), t);
   }
   template<class T>
   constexpr _enable_if_sv<T, basic_string&>
   append(const T& t, size_type pos, size_type count = npos)
   {
      return insert(size(), t, pos, count);
   }
   //@}

   //@{ Insert
   constexpr basic_string& insert(size_type index, size_type count, CharT ch)
   {
      insert(cbegin() + index, count, ch);
      return *this;
   }

   constexpr basic_string& insert(size_type index, const CharT* s)
   {
      return insert(index, s, Traits::length(s));
   }

   constexpr basic_string& insert(size_type index, const CharT* s, size_type count)
   {
      set_new_size_(std::min(size(), index + count));
      Traits::move(data() + index, s, count);
      return *this;
   }

   constexpr basic_string& insert(size_type index, const basic_string& str)
   {
      set_new_size_(std::min(size(), index + str.size()));
      Traits::move(data() + index, str.data(), str.size());
      return *this;
   }

   constexpr basic_string&
   insert(size_type index, const basic_string& str, size_type index_str, size_type count = npos)
   {
      assert(index_str <= str.size());
      if(count == npos) count = str.size() - index_str;
      return insert(index, str.data() + index_str, count);
   }

   constexpr iterator insert(const_iterator pos, CharT ch)
   {
      auto ptr = &ch;
      return insert(pos, ptr, ptr + 1);
   }

   constexpr iterator insert(const_iterator pos, size_type count, CharT ch)
   {
      const auto index = std::distance(cbegin(), pos);
      set_new_size_(std::min(size(), index + count));
      auto ptr       = data() + index;
      const auto end = ptr + count;
      while(ptr != end) *ptr++ = ch;
      return *this;
   }

   template<class InputIt>
   constexpr iterator insert(const_iterator pos, InputIt first, InputIt last)
   {
      const auto delta = std::distance(cbegin(), pos);
      const auto len   = std::distance(first, last);
      set_new_size_(std::min(size(), delta + len));
      std::copy(first, last, pos);
      return begin() + delta;
   }

   constexpr iterator insert(const_iterator pos, std::initializer_list<CharT> ilist)
   {
      return insert(pos, ilist.begin(), ilist.end());
   }

   template<typename T> constexpr _enable_if_sv<T, basic_string&> insert(size_type pos, const T& t)
   {
      string_view_type sv = t;
      insert(pos, sv.data(), sv.size());
      return *this;
   }

   template<class T>
   constexpr _enable_if_sv<T, basic_string&>
   insert(size_type index, const T& t, size_type index_str, size_type count = npos)
   {
      string_view_type sv = t;
      if(index > size() || index_str > sv.size()) throw std::out_of_range{};
      if(count == npos) count = sv.size() - index_str;
      if(index_str + count > sv.size()) throw std::out_of_range{};
      insert(index, sv.data() + index_str, count);
      return *this;
   }
   //@}

   //@{ Friends
   friend constexpr void swap(basic_string& lhs, basic_string& rhs) noexcept
   {
      std::swap(lhs.data_, rhs.data_);
   }

   friend std::ostream& operator<<(std::ostream& stream, const basic_string& string)
   {
      return stream << string.data();
   }
   //@}

   constexpr std::size_t hash() const noexcept
   {
      using sv = string_view_type;
      auto f   = std::hash<sv>{};
      return f(sv{data(), size()});
   }

 private:
   constexpr reference at_(size_type pos) noexcept
   {
      if(pos >= size()) throw std::out_of_range{};
      return data()[pos];
   }
   constexpr const_reference at_(size_type pos) const noexcept
   {
      return const_cast<basic_string*>(this)->at_(pos);
   }

   constexpr auto lex_comp_(const string_view_type& rhs) const noexcept
   {
      return std::lexicographical_compare_three_way(begin(), end(), std::begin(rhs), std::end(rhs));
   }

   constexpr CharT* data_ptr_() noexcept
   {
      return is_sso_() ? data_.sso.string : data_.non_sso.ptr;
   }

   constexpr const CharT* data_ptr_() const noexcept
   {
      return is_sso_() ? data_.sso.string : data_.non_sso.ptr;
   }

   constexpr size_type size_() const noexcept
   {
      return is_sso_() ? sso_size_() : read_non_sso_data_().first;
   }

   constexpr size_type capacity_() const noexcept
   {
      return is_sso_() ? (sizeof(data_) - 1) : read_non_sso_data_().second;
   }

   constexpr void set_capacity_(size_type new_cap)
   {
      if(new_cap <= size()) return;       // nothing to do
      if(new_cap <= sso_capacity) return; // also nothing to do
      *this = basic_string{data(), size(), new_cap};
   }

   constexpr void increase_capacity_(size_type min_new_cap = 0)
   {
      const auto new_cap = 7 + size_t(capacity() * 2.1);
      set_capacity_(std::max(new_cap, min_new_cap + 1));
   }

   constexpr void set_new_size_(size_t size) noexcept
   {
      if(size > capacity()) increase_capacity_(size);
      if(is_sso_()) {
         set_sso_size_(size);
         Traits::assign(data_.sso.string[size], static_cast<CharT>(0));
      } else {
         set_non_sso_data_(size, capacity());
         Traits::assign(data_.non_sso.ptr[size], static_cast<CharT>(0));
      }
   }

   constexpr void set_null_byte_() noexcept { data()[size()] = static_cast<CharT>(0); }

   constexpr void push_back_(CharT ch)
   {
      const auto pos = size();
      set_new_size_(pos + 1);
      data()[pos] = ch;
   }

   constexpr void set_moved_from_() noexcept { set_sso_size_(0); }

   // We are using sso if the last two bits are 0
   constexpr bool is_sso_() const noexcept
   {
      return !lsb_<0>(data_.sso.size) && !lsb_<1>(data_.sso.size);
   }

   // good
   constexpr void set_sso_size_(unsigned char size) noexcept
   {
      data_.sso.size = static_cast<UCharT>(sso_capacity - size) << 2;
   }

   // good
   constexpr size_type sso_size_() const noexcept
   {
      return sso_capacity - ((data_.sso.size >> 2) & 63u);
   }

   constexpr void set_non_sso_data_(size_type size, size_type capacity)
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

   constexpr std::pair<size_type, size_type> read_non_sso_data_() const
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
   static constexpr size_type const high_bit_mask_ = static_cast<size_type>(1)
                                                     << (sizeof(size_type) * CHAR_BIT - 1);
   static constexpr size_type const sec_high_bit_mask_ = static_cast<size_type>(1)
                                                         << (sizeof(size_type) * CHAR_BIT - 2);

   template<typename T> static constexpr unsigned char& most_sig_byte_(T& obj)
   {
      return *(std::bit_cast<unsigned char*>(&obj) + sizeof(obj) - 1);
   }

   template<int N> static constexpr bool lsb_(unsigned char byte) { return byte & (1u << N); }

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
constexpr auto operator<=>(const basic_string<CharT, Traits>& lhs,
                           const std::basic_string_view<CharT, Traits>& rhs) noexcept
{
   return lhs <=> rhs;
}

template<typename CharT, typename Traits>
constexpr auto operator<=>(const basic_string<CharT, Traits>& lhs, const CharT* rhs) noexcept
{
   return lhs <=> rhs;
}

template<typename CharT, typename Traits>
constexpr auto operator<=>(const basic_string<CharT, Traits>& lhs,
                           const basic_string<CharT, Traits>& rhs) noexcept
{
   return lhs <=> rhs;
}

using string = basic_string<char>;

static_assert(sizeof(string) == 3 * sizeof(std::size_t));
static_assert(std::is_nothrow_default_constructible<string>::value);
static_assert(std::is_nothrow_move_constructible<string>::value);
static_assert(std::is_nothrow_move_assignable<string>::value);
static_assert(std::is_nothrow_swappable<string>::value);

template<typename T> struct hash
{
   std::size_t operator()(const T& o) const { return o.hash(); }
};

} // namespace sso23
