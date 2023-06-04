#ifndef BYTECODE_COMPILER_H
#define BYTECODE_COMPILER_H

#include "../ast/ast.h"
#include "../util.h"
#include "assembler.h"

namespace pl0::code {

class Compiler : public ast::AstVisitor<Compiler> {
  std::unordered_map<Procedure *, int> entry_points_;
  std::unordered_map<Procedure *, std::vector<Backpatcher>> patch_list_;
  assembler assembler_;
  Scope *top_scope_{nullptr};

  DECLARE_VISIT_METHODS
  DEFINE_AST_VISITOR_SUBCLASS_MEMBERS

  void VisitRvalue(ast::VariableProxy *node);
  void VisitLvalue(ast::VariableProxy *node);

 public:
  void Generate(ast::Block *program);
  const bytecode &code() { return assembler_.code(); }
};

} // namespace pl0::code

#endif
