#ifndef PARSING_SCOPE_H
#define PARSING_SCOPE_H

#include <string>
#include <unordered_map>

#include "../util.h"
#include "symbol.h"

namespace pl0 {

class Scope {
 public:
  explicit Scope(Scope *enclosing_scope)
      : enclosing_scope_(enclosing_scope)
      , level_(enclosing_scope ? enclosing_scope->level_ + 1 : 0) {}

  ~Scope() {
    for (auto &pair : members_) {
      delete pair.second;
    }
  }

  void Define(Symbol *sym) {
    auto result = members_.emplace(sym->name(), sym);
    if (!result.second) {
      throw GeneralError("duplicated symbol \"", sym->name(), '"');
    }
    if (sym->IsVariable()) { ++variable_count_; }
  }

  Symbol *Resolve(const std::string &name) {
    auto iter = members_.find(name);
    if (iter == members_.end()) {
      return enclosing_scope_ ? enclosing_scope_->Resolve(name) : nullptr;
    }
    return iter->second;
  }

  Scope *enclosing_scope() {
    return enclosing_scope_;
  }

  int level() const {
    return level_;
  }

  int variable_count() const {
    return variable_count_;
  }

 private:
  Scope *enclosing_scope_;
  int level_, variable_count_{0};
  std::unordered_map<std::string, Symbol *> members_;
};

} // namespace pl0

#endif // PARSING_SCOPE_H
