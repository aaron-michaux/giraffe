
#pragma once

#include "operators.hpp"
#include "scanner-input.hpp"
#include "scanner-options.hpp"
#include "token.hpp"

namespace giraffe
{
class Scanner final
{
 public:
   struct LineData
   {
      string_view line         = {};
      uint32_t offset          = 0;
      uint32_t logical_line_no = 0; // Escaped newlines don't increase this
   };

 private:
   struct Worker;
   unique_ptr<Worker> worker_;

   friend size_t read_input_callback_(void*, char* buffer, size_t max_size);

 public:
   /// Initialize a scanner -- i.e., Token producer.
   /// Text memory is STORED here. Tokens don't store text
   Scanner(); //!< reads from stdin
   Scanner(unique_ptr<ScannerInputInterface>&& source,
           ScannerOptions opts = {}) noexcept; //!<
   Scanner(string_view name,      //!< How should we refer to this text?
           string_view text_data, //!< text to scan
           ScannerOptions opts = {}) noexcept; //!< reads `text_data`
   Scanner(string_view name, //!< How should we refer to the FILE*?
           FILE* fp,         //!< Reads from FILE*
           ScannerOptions opts = {}) noexcept;
   Scanner(const Scanner&) = delete;
   Scanner(Scanner&&) noexcept;
   ~Scanner();
   Scanner& operator=(const Scanner&) = delete;
   Scanner& operator                  =(Scanner&&) noexcept;

   /// Initialize from a single text...
   void initialize(unique_ptr<ScannerInputInterface>&& source,
                   ScannerOptions opts = {}) noexcept;

   //@{ Scanning
   /// EOF is appened when the end of all `text-data` is processed
   bool found_eof() const noexcept;

   /// Index to the current token
   size_t position() const noexcept;

   /// TRUE if the next token exists, and it isn't EOF
   bool has_next() noexcept;

   /// Skip to a specific token position
   void set_position(size_t new_position) noexcept;

   /// Get the token-id of the NEXT token
   uint16_t next_token_id() noexcept;

   /// Look at the token at `pos` without changing any positions
   const Token& at(size_t pos) noexcept;

   /// Look ahead `n` tokens, and return it, _without_ changing current position
   const Token& peek(int n = 1) noexcept;

   /// Consume the current token, and move to the next
   const Token& consume() noexcept;

   /// The current token
   const Token& current() noexcept { return peek(0); }

   /// NON-BLOCKING method to tell if we're pointing at the EOF token
   bool reached_eof() const noexcept;
   //@}

   //@{ Recovering token text
   /// Name of the input source
   string_view name() const noexcept;

   /// Return the full text of the line the line location sits on,
   /// as well as the column number of the location, and it's
   /// "logical" line-number, which considers escaped newlines
   LineData line_data(uint32_t line_no) const noexcept;

   /// Returns the total number of available lines
   uint32_t total_lines() const noexcept;

   std::pair<string_view, uint32_t>
   find_line_offset(SourceLocation loc) const noexcept;
   //@}
};

string token_info(const Scanner&, const Token&) noexcept;

} // namespace giraffe
