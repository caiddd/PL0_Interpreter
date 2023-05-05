#ifndef PARSING_SYMBOL_H
#define PARSING_SYMBOL_H

#include <string>

namespace pl0 {

class Symbol {
 public:
  explicit Symbol(std::string name) : name_(std::move(name)) {}

  [[nodiscard]] const std::string &name() const { return name_; }

  [[nodiscard]] virtual bool IsVariable() const { return false; }

  [[nodiscard]] virtual bool IsConstant() const { return false; }

  [[nodiscard]] virtual bool IsProcedure() const { return false; }

 private:
  std::string name_;
};

class Variable : public Symbol {
 public:
  Variable(std::string name, int level, int index)
      : Symbol(std::move(name)), level_(level), index_(index) {}

  [[nodiscard]] int level() const { return level_; }

  [[nodiscard]] int index() const { return index_; }

  [[nodiscard]] bool IsVariable() const override { return true; }

 private:
  int level_;
  int index_;
};

class Constant : public Symbol {
 public:
  Constant(std::string name, int value)
      : Symbol(std::move(name)), value_(value) {}

  [[nodiscard]] int value() const { return value_; }

 private:
  int value_;
};

class Procedure : public Symbol {
 public:
  const static int kInvalidAddress = -1;
  
  Procedure(std::string name, int level)
      : Symbol(std::move(name)), level_(level) {}

  [[nodiscard]] int level() const { return level_; }

  [[nodiscard]] int entry_address() const { return entry_address_; }

  void set_entry_address(int en) { entry_address_ = en; }

  [[nodiscard]] bool IsProcedure() const override { return true; }

 private:
  int level_;
  int entry_address_{kInvalidAddress};
};

} // namespace pl0

#endif // PARSING_SYMBOL_H
