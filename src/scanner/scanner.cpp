#include "stdinc.hpp"

#include "scanner.hpp"

#include "lex-bridge.h"
#include "scanner-input.hpp"
#include "source-location.hpp"

#include <cstdio>
#include <deque>

#define This Scanner

namespace giraffe
{
// ----------------------------------------------------------- token has newline

static size_t count_newlines(string_view text) noexcept
{
   size_t sum = 0;
   for(auto ch : text)
      if(ch == '\n') ++sum;
   return sum;
}

// -------------------------------------------------------- is token of interest
// We often want to skip white space, etc.
static bool is_token_of_interest(const Token& token, const ScannerOptions& opts) noexcept
{
   if(opts.skip_comments && token.is_comment()) return false;
   if(opts.skip_whitespace && token.is_whitespace()) return false;
   if(opts.skip_newlines && token.id() == TNEWLINE) return false;
   if(opts.skip_start_symbol && token.id() == TSTART) return false;
   return true;
}

// ---------------------------------------------------------------------- Worker

struct Scanner::Worker
{
 private:
   Scanner* parent_                          = nullptr;
   unique_ptr<ScannerInputInterface> source_ = nullptr;
   void* lex_                                = nullptr;
   ScannerOptions opts_                      = {};
   std::deque<Token> tokens_                 = {};
   vector<Token> token_buffer_               = {};
   size_t position_                          = 0; //!< The position in the token stack
   int last_non_ws_token_                    = TSTART;
   bool last_token_was_delim_                = true;

   // This is where we store the memory for parsed token text
   string line_buffer_ = ""s;
   struct InternalLineData
   {
      string line              = {}; // All tokens ultimately point to a 'line'
      uint32_t offset          = 0;  //
      uint32_t logical_line_no = 0;  // Escaped newlines to increase this
      bool is_newline_escaped  = false;
      uint32_t end() const noexcept { return uint32_t(line.size()) + offset; }
   };
   std::deque<InternalLineData> lines_ = {}; // lines are parsed one at a time

   uint32_t current_offset_() const noexcept;

   SourceLocation store_lines_(const lex_params_t& lex) noexcept;
   void fill_token_buffer_(vector<Token>& buffer) noexcept;

 public:
   Worker(Scanner* parent,
          unique_ptr<ScannerInputInterface>&& source,
          ScannerOptions opts)
       : parent_(parent)
       , source_(std::move(source))
       , lex_(init_lex(this))
       , opts_(opts)
   {
      tokens_.push_back(Token::make_start_token());
      last_token_was_delim_ = true; // the first token is space delimited
      if(opts.skip_start_symbol) position_ = 1;
   }
   ~Worker() { destroy_lex(lex_); }

   /// Flex uses YY_INPUT to read up to `max_size` characters into `buffer`
   /// This macro calls `worker_read_input`, and extern "C" funciton, that
   /// passes `yyextra->scanner_worker` as its first instance.
   /// `Scanner_worker` is a pointer to a This::Worker object. Okay, I had
   /// to fight the type system, but we get back here, where we produce
   /// input for Flex.
   size_t read_input_callback(char* buffer, size_t max_size) noexcept;

   size_t position() const noexcept { return position_; }

   void set_position(size_t new_position) noexcept { position_ = new_position; }

   /// Returns TRUE if one or more tokens were added
   bool produce_tokens() noexcept;

   auto& tokens() noexcept { return tokens_; }

   string_view name() const noexcept { return source_->name(); }

   LineData line_data(uint32_t line_no) const noexcept;

   uint32_t total_lines() const noexcept;

   std::pair<string_view, uint32_t> find_line_offset(SourceLocation loc) const noexcept;
};

size_t This::Worker::read_input_callback(char* buffer, size_t max_size) noexcept
{
   return source_->read(buffer, max_size);
}

uint32_t This::Worker::current_offset_() const noexcept
{
   return uint32_t(line_buffer_.size()) + (lines_.size() == 0 ? 0u : lines_.back().end());
}

SourceLocation This::Worker::store_lines_(const lex_params_t& lex) noexcept
{
   const auto loc = SourceLocation(current_offset_(), uint32_t(lines_.size()));

   // Ensure we've got enough space
   if(line_buffer_.capacity() < line_buffer_.size() + lex.text_len) {
      const size_t min_cap      = 1024; // 1k
      const size_t required_cap = 3 * (line_buffer_.size() + lex.text_len);
      line_buffer_.reserve(std::max(min_cap, required_cap));
   }

   { // Copy in the data
      const char* end = lex.text + lex.text_len;
      for(auto ii = lex.text; ii != end; ++ii) line_buffer_.push_back(*ii);
   }

   if(false) {
      TRACE(format("store {} ({}) {}",
                   token_id_to_str(lex.token_id),
                   token_id_is_newline(lex.token_id),
                   encode_string(string_view(lex.text, lex.text_len))));
   }

   // If we've got a new line, then push into `lines_`
   if(token_id_is_newline(lex.token_id)) {
      assert(count_newlines(string_view(lex.text, lex.text_len)) == 1);
      assert(string_view(lex.text, lex.text_len).back() == '\n');
      const auto ldat = line_data(uint32_t(lines_.size()));
      lines_.emplace_back();
      lines_.back().line               = ldat.line;
      lines_.back().offset             = ldat.offset;
      lines_.back().logical_line_no    = ldat.logical_line_no;
      lines_.back().is_newline_escaped = (lex.token_id == TNEWLINE_ESC);
      line_buffer_.clear();
   } else {
      assert(count_newlines(string_view(lex.text, lex.text_len)) == 0);
   }

   return loc;
}

void This::Worker::fill_token_buffer_(vector<Token>& buffer) noexcept
{
   buffer.clear();
   while(true) { // Keep lexxing until we hit a NEWLINE or EOF
      const bool is_eof = (yylex(lex_) == TEOF); /// lex the token

      if(is_eof) {
         const auto loc = SourceLocation(current_offset_(), uint32_t(lines_.size()));
         buffer.emplace_back(TEOF, loc, string_view(""), last_token_was_delim_);
         break;
      }

      // Retrieve what we just lexed
      const auto& lex_params = *get_lex_params(lex_);

      // Store the current token text... so we can report errors line-by-line
      const auto location = store_lines_(lex_params);

      // Store the token in temporary buffer
      buffer.emplace_back(lex_params.token_id,
                          location,
                          string_view(string_view{lex_params.text, lex_params.text_len}),
                          last_token_was_delim_);

      const bool is_newline = lex_params.token_id == TNEWLINE;
      const bool is_delim   = (lex_params.token_id == TWHITESPACE)
                            || (lex_params.token_id == TNEWLINE_ESC)
                            || (token_id_is_comment(lex_params.token_id));

      last_token_was_delim_ = (is_delim && last_non_ws_token_ != TNEWLINE);
      if(!is_delim) last_non_ws_token_ = lex_params.token_id;

      // We're done the line
      if(is_newline) break;
   }

   if(false) {
      for(const auto& token : buffer)
         TRACE(format("Fill: {}", token_info(*parent_, token)));
   }
}

bool This::Worker::produce_tokens() noexcept
{
   // Early fail if we've already found EOF
   if(tokens_.size() > 0 && tokens_.back().is_eof()) return false;

   // Store this so that we know when we've pushed a token
   const size_t size0 = tokens_.size();

   // Loop until we return at least _one_ token, including EOF
   while(tokens_.size() == size0) {
      // Fill the token buffer with an entire line worth of data
      fill_token_buffer_(token_buffer_);

      // We should at least add an EOF
      assert(token_buffer_.size() != 0);

      size_t pos = 0;
      for(auto& token : token_buffer_)
         if(is_token_of_interest(token, opts_)) tokens_.push_back(std::move(token));

      token_buffer_.clear();
   }

   return true;
}

This::LineData This::Worker::line_data(uint32_t line_no) const noexcept
{
   if(line_no < lines_.size()) {
      const auto& data = lines_[line_no];
      return {data.line, data.offset, data.logical_line_no};
   } else if(line_no == 0) {
      return {string_view(line_buffer_), 0, 0};
   } else if(line_no == lines_.size()) {
      return {string_view(line_buffer_),
              lines_.back().end(),
              lines_.back().logical_line_no + (lines_.back().is_newline_escaped ? 0 : 1)};
   }
   return This::LineData{"", 0, 0};
}

uint32_t This::Worker::total_lines() const noexcept
{
   return uint32_t(lines_.size()) + (line_buffer_.empty() ? 0 : 1);
}

std::pair<string_view, uint32_t>
This::Worker::find_line_offset(SourceLocation loc) const noexcept
{
   if(loc.line_no > lines_.size()) return {"", 0};
   const auto ldat = line_data(loc.line_no);
   assert(loc.offset >= ldat.offset);
   return {ldat.line, loc.offset - ldat.offset};
}

// ----------------------------------------------------------------- Constructor

This::This() { initialize(make_unique<FILE_ScannerInput>("{stdin}", stdin)); }

This::This(unique_ptr<ScannerInputInterface>&& source, ScannerOptions opts) noexcept
{
   initialize(std::move(source), opts);
}

This::This(string_view name, string_view data, ScannerOptions opts) noexcept
{
   initialize(make_unique<StringScannerInput>(name, data), opts);
}

This::This(string_view name, FILE* fp, ScannerOptions opts) noexcept
{
   initialize(make_unique<FILE_ScannerInput>(name, fp), opts);
}

This::This(This&&) noexcept = default;

This::~This() = default;

This& This::operator=(This&&) noexcept = default;

// ------------------------------------------------------------------ initialize
/// Initialize from a single text... memory not stored here
void This::initialize(unique_ptr<ScannerInputInterface>&& source,
                      ScannerOptions opts) noexcept
{
   worker_.release();
   worker_ = make_unique<Worker>(this, std::move(source), opts);
}

// -------------------------------------------------------------------- Scanning

bool This::found_eof() const noexcept
{
   return worker_->tokens().size() > 0 && worker_->tokens().back().is_eof();
}

size_t This::position() const noexcept { return worker_->position(); }

bool This::has_next() noexcept { return !peek(0).is_eof(); }

void This::set_position(size_t new_position) noexcept
{
   worker_->set_position(new_position);
}

uint16_t This::next_token_id() noexcept { return peek(1).id(); }

const Token& This::at(size_t pos) noexcept
{
   const auto pos0 = position();
   set_position(pos);
   const auto& ret = peek(0);
   set_position(pos0);
   return ret;
}

const Token& This::peek(int n) noexcept
{
   auto calc_ultimate = [&](int n) {
      const auto new_pos = int64_t(position()) + n;
      return size_t(std::max(int64_t(0), new_pos));
   };

   const auto& tokens = worker_->tokens();

   const size_t ultimate_n = calc_ultimate(n);
   while(ultimate_n >= tokens.size()) // while looking beyond `tokens.size()`
      if(!worker_->produce_tokens())  // grab more tokens
         break;                       // but bail if we reach end of input

   const size_t final_index = std::min(ultimate_n, tokens.size());
   assert(final_index < tokens.size()); // there should be a START and EOF
   return tokens[final_index];
}

const Token& This::consume() noexcept
{
   const auto& ret = peek(0); // get the current token
   if(ret.is_eof())
      set_position(worker_->tokens().size() - 1);
   else
      set_position(position() + 1);
   return ret;
}

bool This::reached_eof() const noexcept
{
   return found_eof() && position() + 1 >= worker_->tokens().size();
}

// ------------------------------------------------------------- Recovering Text
string_view This::name() const noexcept { return worker_->name(); }

This::LineData This::line_data(uint32_t line_no) const noexcept
{
   return worker_->line_data(line_no);
}

uint32_t This::total_lines() const noexcept { return worker_->total_lines(); }

std::pair<string_view, uint32_t> This::find_line_offset(SourceLocation loc) const noexcept
{
   return worker_->find_line_offset(loc);
}

// ------------------------------------------------------------------ token-info

string token_info(const Scanner& scanner, const Token& token) noexcept
{
   static constexpr auto k_newline_sv = string_view("{newline}");

   const auto [line, col_no] = scanner.find_line_offset(token.location());

   return format("{:19s} {:5d} {:5d} {:5d} {:5d} {:c} '{}'",
                 token_id_to_str(token.id()),
                 token.line_no(),
                 col_no,
                 token.offset(),
                 token.length(),
                 (token.is_space_delimited() ? '.' : ' '),
                 encode_string(token.text()));
}

// -----------------------------------------------------------------------------
// Fighting the type system... this funciton is a `friend` of Scanner
// and can thus cast `worker` to `Scanner::Worker`, and call
// `read-input-callback`, which produces text data for flex
size_t read_input_callback_(void* worker, char* buffer, size_t max_size)
{
   return reinterpret_cast<Scanner::Worker*>(worker)->read_input_callback(buffer,
                                                                          max_size);
}

} // namespace giraffe

// -----------------------------------------------------------------------------
// This funciton is a bridge that allows the "C" flex scanner to call back
// to the Scanner::Worker, to get some text
extern "C" {
size_t worker_read_input(void* worker, char* buffer, size_t max_size)
{
   return giraffe::read_input_callback_(worker, buffer, max_size);
}
}

#undef This
