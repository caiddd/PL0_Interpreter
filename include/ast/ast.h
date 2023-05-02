#ifndef AST_AST_H
#define AST_AST_H

#include <utility>
#include <vector>

#include "../parsing/scope.h"
#include "../parsing/symbol.h"
#include "../parsing/token.h"

namespace pl0::ast {

#define DECLARATION_NODE_LIST(V) \
  V(VariableDeclaration)         \
  V(ConstantDeclaration)         \
  V(ProcedureDeclaration)

#define EXPRESSION_NODE_LIST(V) \
  V(BinaryOperation)            \
  V(UnaryOperation)             \
  V(Literal)                    \
  V(VariableProxy)

#define STATEMENT_NODE_LIST(V) \
  V(StatementList)             \
  V(Block)                     \
  V(IfStatement)               \
  V(WhileStatement)            \
  V(CallStatement)             \
  V(ReadStatement)             \
  V(WriteStatement)            \
  V(AssignStatement)           \
  V(ReturnStatement)

#define AST_NODE_LIST(V)   \
  DECLARATION_NODE_LIST(V) \
  EXPRESSION_NODE_LIST(V)  \
  STATEMENT_NODE_LIST(V)

#define DEF_FORWARD_DECLARATION(type) class type;
AST_NODE_LIST(DEF_FORWARD_DECLARATION)
#undef DEF_FORWARD_DECLARATION

#define DEF_ENUM_AST_NODE_TYPE(type) k##type,
enum class AstNodeType : int { AST_NODE_LIST(DEF_ENUM_AST_NODE_TYPE) };
#undef DEF_ENUM_AST_NODE_TYPE

#define PROPERTY_GETTER(field) \
  decltype(field##_) field() const { return field##_; }

#define PROPERTY_CONST_REF_GETTER(field) \
  const decltype(field##_) &field() const { return field##_; }

class AstNode {
 public:
  explicit AstNode(AstNodeType type) : type_(type) {}

  [[nodiscard]] AstNodeType type() const { return type_; }

  virtual ~AstNode() = default;

 private:
  AstNodeType type_;
};

class Declaration : AstNode {
 public:
  explicit Declaration(AstNodeType type) : AstNode(type) {}
  ~Declaration() override = default;
};

class Expression : AstNode {
 public:
  explicit Expression(AstNodeType type) : AstNode(type) {}
  ~Expression() override = default;
};

class Statement : AstNode {
 public:
  explicit Statement(AstNodeType type) : AstNode(type) {}
  ~Statement() override = default;
};

class VariableDeclaration : Declaration {
 public:
  using ListType = std::vector<Variable *>;

 private:
  const ListType variables_;

 public:
  explicit VariableDeclaration(ListType variables)
      : Declaration(AstNodeType::kVariableDeclaration)
      , variables_(std::move(variables)) {}

  ~VariableDeclaration() override = default;

  PROPERTY_CONST_REF_GETTER(variables)
};

class ConstantDeclaration : Declaration {
 public:
  using ListType = std::vector<Constant *>;

 private:
  const ListType constants_;

 public:
  explicit ConstantDeclaration(ListType constants)
      : Declaration(AstNodeType::kConstantDeclaration)
      , constants_(std::move(constants)) {}

  ~ConstantDeclaration() override = default;

  PROPERTY_CONST_REF_GETTER(constants);
};

class ProcedureDeclaration final : Declaration {
  Procedure *symbol_;
  Block *main_block_;

 public:
  ProcedureDeclaration(Procedure *symbol, Block *main_block)
      : Declaration(AstNodeType::kProcedureDeclaration)
      , symbol_(symbol)
      , main_block_(main_block) {}

  ~ProcedureDeclaration() final = default;

  PROPERTY_GETTER(symbol)

  PROPERTY_GETTER(main_block)
};

class Block final : Statement {
  Scope *belonging_scope_;
  VariableDeclaration *var_declaration_;
  ConstantDeclaration *const_declaration_;
  std::vector<ProcedureDeclaration *> sub_procedures_;
  Statement *body_;

 public:
  Block(
      Scope *belonging_scope,
      VariableDeclaration *var_declaration,
      ConstantDeclaration *const_declaration,
      std::vector<ProcedureDeclaration *> sub_procedures,
      Statement *body)
      : Statement(AstNodeType::kBlock)
      , belonging_scope_(belonging_scope)
      , var_declaration_(var_declaration)
      , const_declaration_(const_declaration)
      , sub_procedures_(std::move(sub_procedures))
      , body_(body) {}

  ~Block() final {
    delete var_declaration_;
    delete const_declaration_;
    for (auto *dec : sub_procedures_) { delete dec; }
    delete body_;
  }

  PROPERTY_GETTER(belonging_scope)

  PROPERTY_GETTER(var_declaration)

  PROPERTY_GETTER(const_declaration)

  PROPERTY_CONST_REF_GETTER(sub_procedures)

  PROPERTY_GETTER(body)
};

class StatementList final : Statement {
  const std::vector<Statement *> statements_;

 public:
  using ListType = std::vector<Statement *>;

  explicit StatementList(ListType statements)
      : Statement(AstNodeType::kStatementList)
      , statements_(std::move(statements)) {}

  ~StatementList() final = default;

  PROPERTY_CONST_REF_GETTER(statements)
};

class IfStatement final : Statement {
  Expression *condition_;
  Statement *then_statement_;
  Statement *else_statement_;

 public:
  IfStatement(
      Expression *condition,
      Statement *then_statement,
      Statement *else_statement)
      : Statement(AstNodeType::kIfStatement)
      , condition_(condition)
      , then_statement_(then_statement)
      , else_statement_(else_statement) {}

  ~IfStatement() final {
    delete condition_;
    delete then_statement_;
    delete else_statement_;
  }

  [[nodiscard]] bool has_else_statement() const {
    return else_statement_ != nullptr;
  }

  PROPERTY_GETTER(condition)

  PROPERTY_GETTER(then_statement)

  PROPERTY_GETTER(else_statement)
};

class WhileStatement final : Statement {
  Expression *cond_;
  Statement *body_;

 public:
  WhileStatement(Expression *cond, Statement *body)
      : Statement(AstNodeType::kWhileStatement), cond_(cond), body_(body) {}

  ~WhileStatement() final {
    delete cond_;
    delete body_;
  }

  PROPERTY_GETTER(cond)

  PROPERTY_GETTER(body)
};

class CallStatement final : Statement {
  std::string callee_;

 public:
  explicit CallStatement(std::string callee)
      : Statement(AstNodeType::kCallStatement), callee_(std::move(callee)) {}

  ~CallStatement() final = default;

  PROPERTY_CONST_REF_GETTER(callee)
};

class ReadStatement final : Statement {
  const std::vector<VariableProxy *> targets_;

 public:
  using ListType = std::vector<VariableProxy *>;

  explicit ReadStatement(ListType targets)
      : Statement(AstNodeType::kReadStatement), targets_(std::move(targets)) {}

  ~ReadStatement() final = default;

  PROPERTY_CONST_REF_GETTER(targets)
};

class WriteStatement final : Statement {
  const std::vector<Expression *> expressions_;

 public:
  using ListType = std::vector<Expression *>;

  explicit WriteStatement(ListType expressions)
      : Statement(AstNodeType::kWriteStatement)
      , expressions_(std::move(expressions)) {}

  ~WriteStatement() final = default;

  PROPERTY_CONST_REF_GETTER(expressions)
};

class VariableProxy final : Expression {
  Symbol *target_;

 public:
  explicit VariableProxy(Symbol *target)
      : Expression(AstNodeType::kVariableProxy), target_(target) {}

  ~VariableProxy() final = default; // TODO add a proper destructor here

  PROPERTY_GETTER(target)
};

class AssignStatement final : Statement {
  VariableProxy *target_;
  Expression *expr_;

 public:
  AssignStatement(VariableProxy *target, Expression *expr)
      : Statement(AstNodeType::kAssignStatement)
      , target_(target)
      , expr_(expr) {}

  ~AssignStatement() final {
    delete target_;
    delete expr_;
  };

  PROPERTY_GETTER(target)

  PROPERTY_GETTER(expr)
};

class ReturnStatement final : Statement {
 public:
  ReturnStatement() : Statement(AstNodeType::kReturnStatement) {}

  ~ReturnStatement() final = default;
};

class UnaryOperation final : Expression {
  Token op_;
  Expression *expr_;

 public:
  UnaryOperation(Token op, Expression *expr)
      : Expression(AstNodeType::kUnaryOperation), op_(op), expr_(expr) {}

  ~UnaryOperation() final { delete expr_; }

  PROPERTY_GETTER(op)

  PROPERTY_GETTER(expr)
};

class BinaryOperation final : Expression {
  Token op_;
  Expression *left_;
  Expression *right_;

 public:
  BinaryOperation(Token op, Expression *left, Expression *right)
      : Expression(AstNodeType::kBinaryOperation)
      , op_(op)
      , left_(left)
      , right_(right) {}

  ~BinaryOperation() final {
    delete left_;
    delete right_;
  }

  PROPERTY_GETTER(op)

  PROPERTY_GETTER(left)

  PROPERTY_GETTER(right)
};

class Literal final : Expression {
  int value_;

 public:
  explicit Literal(int value)
      : Expression(AstNodeType::kLiteral), value_(value) {}

  ~Literal() final = default;

  PROPERTY_GETTER(value)
};

template<class Visitor>
class AstVisitor {
 protected:
  Visitor *impl() { return static_cast<Visitor *>(this); }

 public:
  void visit(AstNode *node) { impl()->visit(node); }
};

} // namespace pl0::ast

#endif // AST_AST_H
