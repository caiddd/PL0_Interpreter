#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include <ostream>

#include "ast.h"

namespace pl0::ast {
  
class AstPrinter : public AstVisitor<AstPrinter> {
 public:
  explicit AstPrinter(std::ostream &out, int indent_size = 2)
      : out_(out), indent_size_(indent_size) {}

  DECLARE_VISIT_METHODS

 private:
  DEFINE_AST_VISITOR_SUBCLASS_MEMBERS

  std::ostream &out_;
  int indent_level_{0};
  const int indent_size_;

  void IncreaseIndent() {
    indent_level_ += indent_size_;
  }

  void WriteIndent() {
    for (int i = 0; i < indent_level_; ++i) {
      out_.put(' ');
    }
  }

  void EndLine() {
    out_.put('\n');
    WriteIndent();
  }

  void BeginBlock() {
    indent_level_ += indent_size_;
    out_ << " {";
  }

  void EndBlock() {
    indent_level_ -= indent_size_;
    EndLine();
    out_ << "}";
  }
};

} // namespace pl0::ast

#endif // AST_PRINTER_H
