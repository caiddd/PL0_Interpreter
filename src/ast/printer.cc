#include "ast/printer.h"

namespace pl0::ast {

// declaration visitor methods
void AstPrinter::VisitConstantDeclaration(ConstantDeclaration *node) {
  out_ << "constant declaration [ ";
  for (auto *sym : node->constants()) {
    out_ << sym->name() << ' ';
  }
  out_.put(']');
}

void AstPrinter::VisitVariableDeclaration(VariableDeclaration *node) {
  out_ << "variable declaration [ ";
  for (auto *sym : node->variables()) {
    out_ << sym->name() << ' ';
  }
  out_.put(']');
}

void AstPrinter::VisitProcedureDeclaration(ProcedureDeclaration *node) {
  out_ << "procedure declaration " << node->symbol()->name();
  BeginBlock();
  EndLine();
  VisitBlock(node->main_block());
  EndBlock();
}

// statement visitor methods

void AstPrinter::VisitAssignStatement(AssignStatement *node) {
  out_ << "assign statement";
  BeginBlock();
  EndLine();
  out_ << "target =  ";
  VisitVariableProxy(node->target());
  EndLine();
  Visit(node->expr());
  EndBlock();
}

void AstPrinter::VisitIfStatement(IfStatement *node) {
  out_ << "if";
  BeginBlock();
  EndLine();
  out_ << "condition = ";
  Visit(node->condition());
  EndLine();
  out_ << "consequence = ";
  Visit(node->then_statement());
  if (node->has_else_statement()) {
    EndLine();
    out_ << "alternation = ";
    Visit(node->else_statement());
  }
  EndBlock();
}

void AstPrinter::VisitWhileStatement(WhileStatement *node) {
  out_ << "while";
  BeginBlock();
  EndLine();
  out_ << "condition = ";
  Visit(node->cond());
  EndLine();
  out_ << "body = ";
  Visit(node->body());
  EndBlock();
}

void AstPrinter::VisitCallStatement(CallStatement *node) {
  out_ << "invoke " << node->callee();
}

void AstPrinter::VisitBlock(Block *node) {
  out_ << "block";
  BeginBlock();
  EndLine();
  if (node->const_declaration()) {
    out_ << "constants = ";
    VisitConstantDeclaration(node->const_declaration());
    EndLine();
  }
  if (node->var_declaration()) {
    out_ << "variables =  ";
    VisitVariableDeclaration(node->var_declaration());
    EndLine();
  }
  if (!node->sub_procedures().empty()) {
    out_ << "procedures:";
    BeginBlock();
    int index = 0;
    for (auto *method : node->sub_procedures()) {
      EndLine();
      out_ << '[' << index++ << "] = ";
      VisitProcedureDeclaration(method);
    }
    EndBlock();
    EndLine();
  }
  out_ << "body =  ";
  Visit(node->body());
  EndBlock();
}

void AstPrinter::VisitStatementList(StatementList *node) {
  out_ << "statement list";
  BeginBlock();
  int index = 0;
  for (auto *statement : node->statements()) {
    EndLine();
    out_ << '[' << index++ << "] = ";
    Visit(statement);
  }
  EndBlock();
}

void AstPrinter::VisitReadStatement(ReadStatement *node) {
  out_ << "read statement";
}

void AstPrinter::VisitWriteStatement(WriteStatement *node) {
  out_ << "write statement";
}

void AstPrinter::VisitReturnStatement(ReturnStatement *node) {
  out_ << "return statement";
}

// expression visitor methods

void AstPrinter::VisitUnaryOperation(UnaryOperation *node) {
  out_ << "unary operation";
  BeginBlock();
  EndLine();
  out_ << "operator = " << *node->op();
  EndLine();
  out_ << "expression = ";
  Visit(node->expr());
  EndBlock();
}

void AstPrinter::VisitBinaryOperation(BinaryOperation *node) {
  out_ << "binary operation";
  BeginBlock();
  EndLine();
  out_ << "operator = '" << *node->op() << '\'';
  EndLine();
  out_ << "left = ";
  Visit(node->left());
  EndLine();
  out_ << "right = ";
  Visit(node->right());
  EndBlock();
}

void AstPrinter::VisitVariableProxy(VariableProxy *node) {
  Symbol *sym = node->target();
  if (sym->IsConstant()) {
    out_ << "constant ";
  } else if (sym->IsVariable()) {
    out_ << "variable ";
  } else if (sym->IsProcedure()) {
    out_ << "procedure ";
  }
  out_ << node->target()->name();
}

void AstPrinter::VisitLiteral(Literal *node) {
  out_ << "literal " << node->value();
}

} // namespace pl0::ast