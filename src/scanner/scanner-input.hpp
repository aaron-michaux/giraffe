
#pragma once

namespace giraffe
{
class ScannerInputInterface
{
 public:
   virtual ~ScannerInputInterface() = default;

   /// How should this input be referred to?
   virtual string_view name() const noexcept = 0;

   /// Reads up to `max_size` bytes, placing them in `buffer`, returning amount
   virtual size_t read(char* buffer, size_t max_size) noexcept = 0;
};

// -------------------------------------------------------------- read from FILE

class FILE_ScannerInput final : public ScannerInputInterface
{
 private:
   string name_      = ""s;
   FILE* fp_         = nullptr;
   bool interactive_ = false; // FALSE => reading block by block
   bool close_fp_    = false;

   void trace_stmt_()
   {
      TRACE(format("input source = '{}', interactive = {}, close-fp = {}",
                   name_,
                   interactive_,
                   close_fp_));
   }

 public:
   FILE_ScannerInput(string_view name, bool interactive = false) noexcept(false)
   {
      name_        = name;
      fp_          = fopen(name.data(), "rb");
      interactive_ = interactive;
      close_fp_    = true;
      if(fp_ == nullptr)
         throw std::runtime_error(format("could not open file '{}'", name));
      // trace_stmt_();
   }

   FILE_ScannerInput(string_view name, FILE* fp)
       : FILE_ScannerInput(name, fp, isatty(fileno(fp)))
   {}

   FILE_ScannerInput(string_view name, FILE* fp, bool interactive)
       : name_(begin(name), end(name))
       , fp_(fp)
       , interactive_(interactive)
   {
      // trace_stmt_();
   }

   ~FILE_ScannerInput()
   {
      if(close_fp_ && fp_ != nullptr) fclose(fp_);
   }

   string_view name() const noexcept override { return name_; }

   /// Reads up to `max_size` bytes, placing them in `buffer`, returning amount
   size_t read(char* buffer, size_t max_size) noexcept override
   {
      assert(max_size > 0);
      if(interactive_) {
         int c = fgetc_unlocked(fp_);
         return (c == EOF) ? 0 : (buffer[0] = char(c), 1);
      }
      return fread(buffer, 1, max_size, fp_);
   }
};

// ---------------------------------------------------------- read from a string

class StringScannerInput final : public ScannerInputInterface
{
 private:
   string name_   = ""s;
   string buffer_ = ""s;
   size_t pos_    = 0;

   void trace_stmt_()
   {
      TRACE(format("input source = '{}', size = {}", name_, buffer_.size()));
   }

 public:
   StringScannerInput(string_view name, string_view buffer)
       : name_(begin(name), end(name))
       , buffer_(begin(buffer), end(buffer))
   {
      // trace_stmt_();
   }
   StringScannerInput(string_view name, string&& buffer)
       : name_(begin(name), end(name))
       , buffer_(std::move(buffer))
   {
      // trace_stmt_();
   }

   string_view name() const noexcept override { return name_; }

   /// Reads up to `max_size` bytes, placing them in `buffer`, returning amount
   size_t read(char* buffer, size_t max_size) noexcept override
   {
      assert(max_size > 0);
      assert(pos_ <= buffer_.size());
      const size_t remainder   = buffer_.size() - pos_;
      const size_t amount_read = std::min(remainder, max_size);
      memcpy(buffer, &buffer_[pos_], amount_read);
      pos_ += amount_read;
      return amount_read;
   }
};

} // namespace giraffe
