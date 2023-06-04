#include "bytecode/compiler.h"

namespace pl0::code {

void Compiler::VisitVariableDeclaration(ast::VariableDeclaration *node) {
}

void Compiler::VisitConstantDeclaration(ast::ConstantDeclaration *node) {
}

void Compiler::VisitProcedureDeclaration(ast::ProcedureDeclaration *node) {
  entry_points_[node->symbol()] = assembler_.GetNextAddress();
  VisitBlock(node->main_block());
}

void Compiler::VisitBlock(ast::Block *node) {
  top_scope_ = node->belonging_scope();
  assembler_.Enter(top_scope_->variable_count() + 3);
  Visit(node->body());
  assembler_.leave();
  for (auto *method : node->sub_procedures()) {
    VisitProcedureDeclaration(method);
  }
  top_scope_ = top_scope_->enclosing_scope();
}

void Compiler::VisitUnaryOperation(ast::UnaryOperation *node) {
  Visit(node->expr());
  assembler_.Operation(node->op());
}

void Compiler::VisitBinaryOperation(ast::BinaryOperation *node) {
  Visit(node->left());
  Visit(node->right());
  assembler_.Operation(node->op());
}

void Compiler::VisitLiteral(ast::Literal *node) {
  assembler_.Load(node->value());
}

void Compiler::VisitVariableProxy(ast::VariableProxy *node) {
  VisitRvalue(node);
}

void Compiler::VisitLvalue(ast::VariableProxy *node) {
  auto *sym = node->target();
  if (sym->IsVariable()) {
    auto *var = dynamic_cast<Variable *>(sym);
    assembler_.Store(top_scope_->level() - var->level(), var->index());
  } else if (sym->IsConstant()) {
    throw GeneralError("constant " + sym->name() + " is not assignable");
  } else {
    throw GeneralError("procedure " + sym->name() + " is not assignable");
  }
}

void Compiler::VisitRvalue(ast::VariableProxy *node) {
  auto *sym = node->target();
  if (sym->IsVariable()) {
    auto *var = dynamic_cast<Variable *>(sym);
    assembler_.Load(top_scope_->level() - var->level(), var->index());
  } else if (sym->IsConstant()) {
    auto *var = dynamic_cast<Constant *>(sym);
    assembler_.Load(var->value());
  } else {
    throw GeneralError(
        sym->name() + " is a procedure so that cannot be used in expression");
  }
}

void Compiler::VisitAssignStatement(ast::AssignStatement *node) {
  Visit(node->expr());
  VisitLvalue(node->target());
}

void Compiler::VisitCallStatement(ast::CallStatement *node) {
  auto *sym = top_scope_->Resolve(node->callee());
  if (sym == nullptr) {
    throw GeneralError(
        "no procedure named \"" + node->callee() + "\" to be called");
  }
  if (!sym->IsProcedure()) {
    throw GeneralError(node->callee() + " is not a procedure");
  }
  auto *method = dynamic_cast<Procedure *>(sym);
  patch_list_[method].push_back(assembler_.Call(top_scope_->level()));
}

void Compiler::VisitWriteStatement(ast::WriteStatement *node) {
  for (auto *expr : node->expressions()) {
    Visit(expr);
    assembler_.Write();
  }
}

void Compiler::VisitWhileStatement(ast::WhileStatement *node) {
  auto beginning = assembler_.GetNextAddress();
  Visit(node->cond());
  auto goto_end = assembler_.BranchIfFalse();
  Visit(node->body());
  assembler_.Branch(beginning);
  goto_end.set_address(assembler_.GetNextAddress());
}

void Compiler::VisitReturnStatement(ast::ReturnStatement * /*node*/) {
  assembler_.leave();
}

void Compiler::VisitReadStatement(ast::ReadStatement *node) {
  for (auto *var : node->targets()) {
    assembler_.Read();
    VisitLvalue(var);
  }
}

void Compiler::VisitIfStatement(ast::IfStatement *node) {
  Visit(node->condition());
  if (node->has_else_statement()) {
    auto goto_else = assembler_.BranchIfFalse();
    Visit(node->then_statement());
    auto goto_end = assembler_.Branch();
    goto_else.set_address(assembler_.GetNextAddress());
    Visit(node->else_statement());
    goto_end.set_address(assembler_.GetNextAddress());
  } else {
    auto goto_end = assembler_.BranchIfFalse();
    Visit(node->then_statement());
    goto_end.set_address(assembler_.GetNextAddress());
  }
}

void Compiler::VisitStatementList(ast::StatementList *node) {
  for (auto *stmt : node->statements()) { Visit(stmt); }
}

void Compiler::Generate(ast::Block *program) {
  VisitBlock(program);
  for (const auto &kv : patch_list_) {
    for (auto patch : kv.second) {
      patch.set_level(patch.get_level() - kv.first->level());
      if (entry_points_.find(kv.first) == entry_points_.end()) {
        throw GeneralError("unexpected error");
      }
      patch.set_address(entry_points_[kv.first]);
    }
  }
}

} // namespace pl0::code