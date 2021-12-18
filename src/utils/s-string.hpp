
#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>

namespace giraffe
{
using String = std::string_view;
}

// namespace giraffe
// {
// class String final
// {
//  private:
//    std::uint64_t A_ = 0; // default to short
//    std::uint64_t B_ = 0;

//    using char_type = unsigned char;

//    static constexpr char_type bit_mask_         = 0x80;
//    static constexpr std::size_t max_short_size_ = 0x0f;
//    static constexpr std::size_t max_long_size_
//        = std::numeric_limits<uint32_t>::max();

//    constexpr bool short_() const noexcept { return !long_(); }
//    constexpr bool long_() const noexcept { return char(*ptr_() & bit_mask_) != 0; }

//    void set_long(bool val) noexcept
//    {
//       if(val)
//          *ptr_() = *ptr_() | bit_mask_;
//       else
//          *ptr_() = *ptr_() & ~bit_mask_;
//    }

//    template<typename pointer> static constexpr pointer ptr_cast_(auto x) noexcept {
//       void * p = const_cast<void*>(static_cast<const void*>(x));
//       return static_cast<pointer>(p);      
//    }
   
//    constexpr const char_type* ptr_() const noexcept
//    {
//       return ptr_cast_<const char_type*>(&A_);
//    }
//    constexpr char_type* ptr_() noexcept { return ptr_cast_<char_type*>(&A_); }
//    constexpr char* ptr0_() noexcept { return ptr_cast_<char*>(&A_) + 1; }
//    constexpr char* ptr1_() noexcept { return ptr_cast_<char*>(B_); }

//    constexpr std::size_t size0_() const noexcept
//    {
//       return std::size_t(ptr_()[0]) & max_short_size_;
//    }

//    std::size_t size1_() const noexcept
//    {
//       return *(reinterpret_cast<const uint32_t*>(&A_) + 1);
//    }

//    void set_size_(std::size_t sz) noexcept(false)
//    {
//       if(sz == size()) return; // we're done

//       char* ptr = nullptr; // for "long" string
//       if(sz > max_long_size_) throw std::bad_alloc{};
//       if(sz > max_short_size_) {
//          ptr = reinterpret_cast<char*>(std::malloc(sz));
//          if(ptr == nullptr) throw std::bad_alloc{};
//       }

//       // Guaranteed no-except from here
//       reset();
//       if(sz <= max_short_size_) {
//          *ptr_() = char_type(sz);
//          assert(short_());
//       } else {
//          set_long(true);
//          *(reinterpret_cast<uint32_t*>(&A_) + 1) = std::uint32_t(sz);
//          B_ = std::uint64_t(reinterpret_cast<std::uintptr_t>(ptr));
//          assert(long_());
//          assert(data() == ptr);
//       }
//       assert(size() == sz);
//    };

//    void copy_(std::string_view s) noexcept(false)
//    {
//       set_size_(s.size());
//       assert(size() == s.size());
//       std::memcpy(data(), s.data(), s.size());
//    }

//  public:
//    constexpr String() = default;
//    String(const String& o) noexcept(false) { *this = o; }
//    constexpr String(String&& o) noexcept { *this = std::move(o); }
//    ~String() { reset(); }

//    String& operator=(const String& o) noexcept(false)
//    {
//       if(this != &o) copy_(o.str());
//       return *this;
//    }

//    constexpr String& operator=(String&& o) noexcept
//    {
//       if(this != &o) {
//          reset();
//          A_   = o.A_;
//          B_   = o.B_;
//          o.A_ = o.B_ = 0; // resets
//          assert(o.short_());
//       }
//       return *this;
//    }

//    explicit String(const char* s) noexcept(false) { copy_(s); }
//    explicit String(const std::string& s) noexcept(false) { copy_(s); }
//    explicit String(const std::string_view& s) noexcept(false) { copy_(s); }

//    String& operator=(std::string_view o) noexcept(false)
//    {
//       *this = String(o);
//       return *this;
//    }

//    String& operator=(const std::string& o) noexcept(false)
//    {
//       *this = String(o);
//       return *this;
//    }

//    String& operator=(const char* o) noexcept(false)
//    {
//       *this = String(o);
//       return *this;
//    }

//    void reset() noexcept
//    {
//       if(long_()) std::free(data());
//       A_ = B_ = 0;
//       assert(short_());
//    }

//    constexpr std::size_t size() const noexcept
//    {
//       return short_() ? size0_() : size1_();
//    }
//    constexpr std::size_t length() const noexcept { return size(); }

//    constexpr bool empty() const noexcept { return size() == 0; }

//    void set_size(std::size_t sz) noexcept(false) { set_size_(sz); }

//    constexpr char* data() noexcept { return short_() ? ptr0_() : ptr1_(); }

//    constexpr const char* data() const noexcept
//    {
//       return const_cast<String*>(this)->data();
//    }

//    char& operator[](std::size_t pos) noexcept { return data()[pos]; }
//    char operator[](std::size_t pos) const noexcept { return data()[pos]; }

//    constexpr std::string_view str() const noexcept
//    {
//       return std::string_view{data(), size()};
//    }

//    bool operator==(const String& o) const noexcept { return str() == o.str(); }
//    bool operator!=(const String& o) const noexcept { return str() != o.str(); }
//    bool operator<=(const String& o) const noexcept { return str() <= o.str(); }
//    bool operator>=(const String& o) const noexcept { return str() >= o.str(); }
//    bool operator<(const String& o) const noexcept { return str() < o.str(); }
//    bool operator>(const String& o) const noexcept { return str() > o.str(); }

//    bool operator==(std::string_view o) const noexcept { return str() == o; }
//    bool operator!=(std::string_view o) const noexcept { return str() != o; }
//    bool operator<=(std::string_view o) const noexcept { return str() <= o; }
//    bool operator>=(std::string_view o) const noexcept { return str() >= o; }
//    bool operator<(std::string_view o) const noexcept { return str() < o; }
//    bool operator>(std::string_view o) const noexcept { return str() > o; }

//    auto begin() noexcept { return data(); }
//    auto end() noexcept { return data() + size(); }
//    auto begin() const noexcept { return data(); }
//    auto end() const noexcept { return data() + size(); }
//    auto cbegin() const noexcept { return data(); }
//    auto cend() const noexcept { return data() + size(); }

//    auto rbegin() noexcept { return std::make_reverse_iterator(begin()); }
//    auto rend() noexcept { return std::make_reverse_iterator(end()); }
//    auto rbegin() const noexcept { return std::make_reverse_iterator(begin()); }
//    auto rend() const noexcept { return std::make_reverse_iterator(end()); }
//    auto crbegin() const noexcept
//    {
//       return std::make_reverse_iterator(cbegin());
//    }
//    auto crend() const noexcept { return std::make_reverse_iterator(cend()); }
// };

// static_assert(sizeof(String) == 16);

// template<class CharT, class Traits>
// inline std::basic_ostream<CharT, Traits>&
// operator<<(std::basic_ostream<CharT, Traits>& os, const String& v)
// {
//    os << v.str();
//    return os;
// }

// } // namespace giraffe

// namespace std
// {
// template<> struct hash<giraffe::String>
// {
//    std::size_t operator()(giraffe::String const& s) const noexcept
//    {
//       std::hash<std::string_view> hash_fun = {};
//       return hash_fun(s.str());
//    }
// };
// } // namespace std
