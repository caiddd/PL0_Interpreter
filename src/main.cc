#include <fstream>
#include <iostream>

#include "argparser.h"
#include "ast/printer.h"
#include "bytecode/compiler.h"
#include "parsing/parser.h"
#include "vm.h"

struct options {
  bool show_ast = false;
  bool show_tokens = false;
  bool compile_only = false;
  bool show_bytecode = false;
  std::string input_file;
};

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

void PrintBytecode(const pl0::bytecode &code) {
  std::cout << "Bytecode Generate:\n";
  for (size_t i = 0; i < code.size(); i++) {
    std::cout << i << '\t' << *code[i].op << '\t' << code[i].level << '\t'
              << code[i].address << '\n';
  }
  std::cout << '\n';
}

options parse_args(int argc, const char *argv[]) {
  try {
    options option;
    std::vector<std::string> rest;
    pl0::ArgumentParser<options> parser{"Yet Another PL/0 Interpreter"};
    parser.Flags(
        {"--show-tokens", "-l"}, "Print all tokens.", &options::show_tokens);
    parser.Flags(
        {"--show-ast", "-t"}, "Print abstract syntax tree.",
        &options::show_ast);
    parser.Flags(
        {"--show-bytecode", "-s"}, "Print bytecode after code generation",
        &options::show_bytecode);
    parser.Flags(
        {"--compile-only", "-c"},
        "If specified, bytecode will not be executed.", &options::compile_only);
    parser.Parse(argc, argv, option, rest);

    if (rest.empty()) { parser.ShowHelp(); }

    option.input_file = rest[0];
    return option;
  } catch (pl0::BasicError &error) {
    std::cout << "Error: " << error.what() << '\n';
    exit(EXIT_FAILURE);
  }
}

int main(int argc, const char *argv[]) {
  auto option = parse_args(argc, argv);

  std::ifstream fin(option.input_file);
  if (fin.fail()) {
    std::cerr << "Error: failed to open file: \"" << option.input_file
              << "\"\n";
    return -1;
  }

  pl0::Lexer lex(fin);
  if (option.show_tokens) { PrintTokens(lex); }

  pl0::Parser parser(lex);
  pl0::ast::Block *program = nullptr;

  try {
    program = parser.Program();
  } catch (pl0::GeneralError &error) {
    pl0::Location const loc = lex.loc();
    std::cout << "Error(" << loc.to_string() << "): " << error.what() << '\n';
    return EXIT_FAILURE;
  }

  pl0::code::Compiler compiler{};

  try {
    compiler.Generate(program);
  } catch (pl0::GeneralError &error) {
    pl0::Location const loc = lex.loc();
    std::cout << "Error(" << loc.to_string() << "): " << error.what() << '\n';
    return EXIT_FAILURE;
  }

  if (option.show_ast) {
    pl0::ast::AstPrinter printer(std::cout);
    printer.VisitBlock(program);
  }

  if (option.show_bytecode) { PrintBytecode(compiler.code()); }

  if (!option.compile_only) { pl0::Execute(compiler.code()); }

  return 0;
}