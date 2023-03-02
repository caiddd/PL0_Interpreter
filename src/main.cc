#include "parsing/lexer.h"
#include <fstream>
#include <iostream>

struct options {
  bool show_tokens;
  std::string input_file;
} option;

[[noreturn]] void PrintTokens(pl0::Lexer &lex) {
  while (true) {
    auto token = lex.peek();
    std::cout << lex.loc().to_string() << '\t' << *token << '\t'
              << lex.literal_buffer() << '\n';
    if (token == pl0::Token::EOS or token == pl0::Token::ILLEGAL) { break; }
    lex.Advance();
  }
  exit(0);
}

void parse_args(int argc, const char *argv[]) {
  // TODO : Write a class to parse the arguments later
  if (argc != 2) {
    std::cerr << "Error: Wrong number of parameters \n";
    exit(-1);
  }
  option.show_tokens = true;
  option.input_file = argv[1];
}

int main(int argc, const char *argv[]) {
  parse_args(argc, argv);

  std::ifstream fin(option.input_file);
  if (fin.fail()) {
    std::cerr << "Error: failed to open file: \"" << option.input_file
              << "\"\n";
    return -1;
  }

  pl0::Lexer lex(fin);
  if (option.show_tokens) { PrintTokens(lex); }
  return 0;
}