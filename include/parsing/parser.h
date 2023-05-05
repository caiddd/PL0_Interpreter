#ifndef PARSING_PARSER_H
#define PARSING_PARSER_H

#include "../ast/ast.h"
#include "lexer.h"
#include "scope.h"

namespace pl0 {

class Parser {
 public:
  explicit Parser(Lexer &lex) : lexer_(lex), top_(nullptr) {}
  ast::Block *Program();

 private:
  Lexer &lexer_;
  Scope *top_;

  // scope control
  void EnterScope();
  void LeaveScope();

  // lexical helper functions
  int Number();
  void Expect(Token tk);
  std::string Identifier();

  // program
  ast::Block *SubProgram();
  // declarations
  ast::VariableDeclaration *VariableDecl();
  ast::ConstantDeclaration *ConstantDecl();
  ast::ProcedureDeclaration *ProcedureDecl();
  // statements
  ast::StatementList *StatementList();
  ast::IfStatement *IfStatement();
  ast::WhileStatement *WhileStatement();
  ast::CallStatement *CallStatement();
  ast::Statement *Statement();
  ast::ReadStatement *ReadStatement();
  ast::WriteStatement *WriteStatement();
  ast::AssignStatement *AssignStatement();
  ast::ReturnStatement *ReturnStatement();
  // expressions
  ast::VariableProxy *LocalVariable();
  ast::Expression *Condition();
  ast::Expression *Expression();
  ast::Expression *Term();
  ast::Expression *Factor();
};

} // namespace pl0

#endif // PARSING_PARSER_H
