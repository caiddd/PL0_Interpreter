#include "parsing/lexer.h"

namespace {

bool IsIdentifierStart(int ch) {
  return isalpha(ch) or ch == '_';
}

bool IsIdentifierPart(int ch) {
  return isalnum(ch) or ch == '_';
}

}; // namespace

namespace pl0 {

void Lexer::Advance() {
  if (peek_ == Token::EOS or peek_ == Token::ILLEGAL) { return; }
  while (isspace(input_stream_.peek())) {
    if (Get() == '\n') {
      loc_.line++;
      loc_.column = 1;
    }
  }
  if (input_stream_.peek() == std::char_traits<char>::eof()) {
    peek_ = Token::EOS;
    return;
  }
  literal_buffer_.clear();
  // identifier or keyword
  if (IsIdentifierStart(input_stream_.peek())) {
    do {
      literal_buffer_.push_back(Get());
    } while (IsIdentifierPart(input_stream_.peek()));
    auto iter = keyword_map.find(literal_buffer_);
    peek_ = (iter == keyword_map.end() ? Token::IDENTIFIER : iter->second);
    return;
  }
  // number
  if (isdigit(input_stream_.peek())) {
    do {
      literal_buffer_.push_back(Get());
    } while (isdigit(input_stream_.peek()));
    peek_ = Token::NUMBER;
    return;
  }
  // punctuator, operator or unrecognized character
  switch (Get()) {
    case '+':
      peek_ = Token::ADD;
      break;
    case '-':
      peek_ = Token::SUB;
      break;
    case '*':
      peek_ = Token::MUL;
      break;
    case '/':
      peek_ = Token::DIV;
      break;
    case ':':
      peek_ = Select('=', Token::ASSIGN, Token::ILLEGAL);
      break;
    case '(':
      peek_ = Token::LPAREN;
      break;
    case ')':
      peek_ = Token::RPAREN;
      break;
    case ';':
      peek_ = Token::SEMICOLON;
      break;
    case '.':
      peek_ = Token::PERIOD;
      break;
    case ',':
      peek_ = Token::COMMA;
      break;
    case '=':
      peek_ = Token::EQ;
      break;
    case '#':
      peek_ = Token::NEQ;
      break;
    case '<':
      peek_ = Select('=', Token::LEQ, Token::LE);
      break;
    case '>':
      peek_ = Select('=', Token::GEQ, Token::GE);
      break;
    default:
      peek_ = Token::ILLEGAL;
      break;
  }
}

} // namespace pl0