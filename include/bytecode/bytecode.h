#ifndef BYTECODE_BYTECODE_H
#define BYTECODE_BYTECODE_H

#include <vector>

#include "../parsing/token.h"

namespace pl0 {

#define OPCODE_LIST(T) T(LIT) T(LOD) T(STO) T(CAL) T(INT) T(JMP) T(JPC) T(OPR)

#define T(x) x,
enum class opcode : int { OPCODE_LIST(T) };
#undef T

#define T(name) #name,
const char *const opcode_name[] = {OPCODE_LIST(T)};
#undef T

inline const char *operator*(opcode opc) {
  return opcode_name[static_cast<int>(opc)];
}

enum class opt : int {
  RET = 0,
  SUB,
  ADD,
  DIV,
  MUL,
  LE,
  LEQ,
  GE,
  GEQ,
  EQ,
  NEQ,
  ODD,
  WRITE = 14,
  READ = 16
};

inline constexpr int operator*(opt x) {
  return static_cast<int>(x);
}

#define OPERATOR(name, string) {Token::name, opt::name},
const std::unordered_map<Token, opt> token2opt = {
    TOKEN_LIST(IGNORE_TOKEN, OPERATOR, IGNORE_TOKEN)};
#undef OPERATOR

struct Instruction {
  opcode op;
  int level;
  int address;
};

using bytecode = std::vector<Instruction>;

class Backpatcher {
  int pos_;
  bytecode *code_;

 public:
  Backpatcher(bytecode &code, int pos) : code_(&code), pos_(pos) {}
  int get_level() { return (*code_)[pos_].level; }
  void set_level(int level) { (*code_)[pos_].level = level; }
  void set_address(int address) { (*code_)[pos_].address = address; }
};

} // namespace pl0

#endif
