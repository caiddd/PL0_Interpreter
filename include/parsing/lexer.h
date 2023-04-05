#ifndef PARSING_LEXER_H
#define PARSING_LEXER_H

#include "../util.h"
#include "token.h"
#include <cctype>
#include <istream>

namespace pl0 {

class Lexer {
 public:
  explicit Lexer(std::istream &input_stream) : input_stream_(input_stream) {
    Advance();
  }

  Token peek() { return peek_; }
  Location &loc() { return loc_; }
  std::string literal_buffer() { return literal_buffer_; }

  void Advance();
  Token Next() {
    const Token ksave = peek_;
    Advance();
    return ksave;
  }
  bool Match(Token tk) {
    if (peek_ == tk) {
      Next();
      return true;
    }
    return false;
  }

 private:
  Token peek_{Token::UNUSED};
  Location loc_;
  std::istream &input_stream_;
  std::string literal_buffer_;
  char Get() {
    loc_.column++;
    return input_stream_.get();
  }
  Token Select(char cond, Token conseq, Token altern) {
    if (input_stream_.peek() == cond) {
      Get();
      return conseq;
    }
    return altern;
  }
};

} // namespace pl0

#endif // PARSING_LEXER_H