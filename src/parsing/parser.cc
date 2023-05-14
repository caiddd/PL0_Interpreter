#include "parsing/parser.h"

namespace pl0 {

ast::Block *Parser::Program() {
  EnterScope();
  auto *block = SubProgram();
  LeaveScope();
  Expect(Token::PERIOD);
  Expect(Token::EOS);
  return block;
}

void Parser::EnterScope() {
  top_ = new Scope(top_);
}

void Parser::LeaveScope() {
  // TODO delete need to think
  // auto *inner = top_;
  top_ = top_->enclosing_scope();
  // delete inner;
}

ast::Block *Parser::SubProgram() {
  auto *constants = lexer_.Peek(Token::CONST) ? ConstantDecl() : nullptr;
  auto *variables = lexer_.Peek(Token::VAR) ? VariableDecl() : nullptr;
  std::vector<ast::ProcedureDeclaration *> sub_methods;
  while (lexer_.Peek(Token::PROCEDURE)) {
    sub_methods.emplace_back(ProcedureDecl());
  }
  auto *body = Statement();
  return new ast::Block(
      top_, variables, constants, std::move(sub_methods), body);
}

// declarations
ast::ConstantDeclaration *Parser::ConstantDecl() {
  ast::ConstantDeclaration::ListType consts;
  Expect(Token::CONST);
  do {
    auto id = Identifier();
    Expect(Token::EQ);
    auto *sym = new Constant(id, Number());
    consts.push_back(sym);
    top_->Define(sym);
  } while (lexer_.Match(Token::COMMA));
  Expect(Token::SEMICOLON);
  return new ast::ConstantDeclaration{std::move(consts)};
}

ast::VariableDeclaration *Parser::VariableDecl() {
  ast::VariableDeclaration::ListType vars;
  Expect(Token::VAR);
  do {
    auto id = Identifier();
    auto *sym = new Variable(id, top_->level(), top_->variable_count());
    vars.push_back(sym);
    top_->Define(sym);
  } while (lexer_.Match(Token::COMMA));
  Expect(Token::SEMICOLON);
  return new ast::VariableDeclaration{std::move(vars)};
}

ast::ProcedureDeclaration *Parser::ProcedureDecl() {
  Expect(Token::PROCEDURE);
  auto name = Identifier();
  auto *sym = new Procedure(name, top_->level());
  top_->Define(sym);
  Expect(Token::SEMICOLON);
  EnterScope();
  auto *block = SubProgram();
  LeaveScope();
  Expect(Token::SEMICOLON);
  return new ast::ProcedureDeclaration{sym, block};
}

ast::Statement *Parser::Statement() {
  switch (lexer_.peek()) {
    case Token::READ:
      return ReadStatement();
    case Token::WRITE:
      return WriteStatement();
    case Token::IF:
      return IfStatement();
    case Token::BEGIN:
      return StatementList();
    case Token::WHILE:
      return WhileStatement();
    case Token::CALL:
      return CallStatement();
    case Token::RETURN:
      return ReturnStatement();
    default:
      return AssignStatement();
  }
}

ast::ReadStatement *Parser::ReadStatement() {
  ast::ReadStatement::ListType targets;
  Expect(Token::READ);
  do {
    targets.emplace_back(LocalVariable());
  } while (lexer_.Match(Token::COMMA));
  return new ast::ReadStatement(targets);
}

ast::WriteStatement *Parser::WriteStatement() {
  ast::WriteStatement::ListType expressions;
  Expect(Token::WRITE);
  do {
    expressions.emplace_back(Expression());
  } while (lexer_.Match(Token::COMMA));
  return new ast::WriteStatement(expressions);
}

ast::IfStatement *Parser::IfStatement() {
  Expect(Token::IF);
  auto *cond = Condition();
  Expect(Token::THEN);
  auto *then = Statement();
  return new ast::IfStatement(
      cond, then, lexer_.Match(Token::ELSE) ? Statement() : nullptr);
}

ast::StatementList *Parser::StatementList() {
  ast::StatementList::ListType statements;
  Expect(Token::BEGIN);
  do {
    statements.emplace_back(Statement());
  } while (lexer_.Match(Token::SEMICOLON));
  Expect(Token::END);
  return new ast::StatementList(std::move(statements));
}

ast::WhileStatement *Parser::WhileStatement() {
  Expect(Token::WHILE);
  auto *cond = Condition();
  Expect(Token::DO);
  return new ast::WhileStatement(cond, Statement());
}

ast::CallStatement *Parser::CallStatement() {
  Expect(Token::CALL);
  auto callee = Identifier();
  auto *sym = top_->Resolve(callee);
  if (sym->IsProcedure()) { return new ast::CallStatement(callee); }
  throw GeneralError("cannot call non-procedure \"", callee, '"');
}

ast::ReturnStatement *Parser::ReturnStatement() {
  Expect(Token::RETURN);
  return new ast::ReturnStatement();
}

ast::AssignStatement *Parser::AssignStatement() {
  auto *var = LocalVariable();
  Expect(Token::ASSIGN);
  return new ast::AssignStatement(var, Expression());
}

ast::VariableProxy *Parser::LocalVariable() {
  auto id = Identifier();
  auto *sym = top_->Resolve(id);
  if (sym->IsVariable()) {
    return new ast::VariableProxy(dynamic_cast<Variable *>(sym));
  }
  if (sym == nullptr) {
    throw GeneralError("undeclared idetifier \"", id, '"');
  }
  throw GeneralError("cannot assign value to a non-variable \"", id, '"');
}

ast::Expression *Parser::Condition() {
  if (lexer_.Match(Token::ODD)) {
    return new ast::UnaryOperation(Token::ODD, Expression());
  }
  auto *left = Expression();
  auto cmp_op = lexer_.Next();
  if (!is_compare_operator(cmp_op)) {
    throw GeneralError("expect a compare operator instead of ", *cmp_op);
  }
  return new ast::BinaryOperation(cmp_op, left, Expression());
}

ast::Expression *Parser::Expression() {
  auto *lhs = Term();
  while (lexer_.Peek(Token::ADD) or lexer_.Peek(Token::SUB)) {
    auto op = lexer_.Next();
    lhs = new ast::BinaryOperation(op, lhs, Term());
  }
  return lhs;
}

ast::Expression *Parser::Term() {
  auto *lhs = Factor();
  while (lexer_.Peek(Token::MUL) or lexer_.Peek(Token::DIV)) {
    auto op = lexer_.Next();
    lhs = new ast::BinaryOperation(op, lhs, Factor());
  }
  return lhs;
}

ast::Expression *Parser::Factor() {
  if (lexer_.Peek(Token::IDENTIFIER)) {
    auto id = lexer_.literal_buffer();
    lexer_.Advance();
    auto *sym = top_->Resolve(id);
    if (sym == nullptr) {
      throw GeneralError("undeclared identifier \"", id, '"');
    }
    return new ast::VariableProxy(sym);
  }
  if (lexer_.Peek(Token::NUMBER)) { return new ast::Literal(Number()); }
  if (lexer_.Match(Token::LPAREN)) {
    auto *expr = Expression();
    Expect(Token::RPAREN);
    return expr;
  }
  throw GeneralError(
      "expect an identifier, a number or a expression instead of ",
      *lexer_.peek());
}

void Parser::Expect(Token tk) {
  if (!lexer_.Match(tk)) {
    throw GeneralError("expect ", *tk, " instead of ", *lexer_.peek());
  }
}

std::string Parser::Identifier() {
  if (lexer_.Peek(Token::IDENTIFIER)) {
    auto result = lexer_.literal_buffer();
    lexer_.Advance();
    return result;
  }
  throw GeneralError("expect an identifier instead of ", *lexer_.peek());
}

int Parser::Number() {
  if (lexer_.Peek(Token::NUMBER)) {
    auto num = std::stoi(lexer_.literal_buffer());
    lexer_.Advance();
    return num;
  }
  throw GeneralError("expect a number instead of ", *lexer_.peek());
}

} // namespace pl0