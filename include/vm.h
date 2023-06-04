#ifndef VM_H
#define VM_H

#include <functional>

#include "bytecode/bytecode.h"

namespace pl0 {

class StackFrame {
  int return_address_;
  StackFrame *static_link_;
  StackFrame *dynamic_link_;
  std::vector<int> intermediates_;
  std::vector<std::pair<std::string, int>> locals_;

 public:
  StackFrame(int ret_address, StackFrame *dyn_link, StackFrame *static_link)
      : return_address_(ret_address)
      , dynamic_link_(dyn_link)
      , static_link_(static_link) {}

  /**
   * Destroy and immediately return to enclosing stack frame
   * @param return_address
   * @param frame_pointer
   */
  void Leave(int &return_address, StackFrame *&frame_pointer) {
    return_address = return_address_;
    frame_pointer = dynamic_link_;
    delete this;
  }

  StackFrame *Resolve(int level_dist) {
    StackFrame *target_frame = this;
    while (level_dist > 0) {
      target_frame = target_frame->static_link_;
      level_dist--;
    }
    return target_frame;
  }

  void Allocate(int count) {
    for (int i = 0; i < count; i++)
      locals_.emplace_back(std::pair{std::string{}, 0});
  }

  int &Local(int level_dist, int index) {
    return Resolve(level_dist)->locals_[index].second;
  }

  void Push(int value) { intermediates_.push_back(value); }

  int Pop() {
    int result = intermediates_.back();
    intermediates_.pop_back();
    return result;
  }
};

const std::unordered_map<opt, std::function<int(int, int)>> opt2functor = {
    {opt::ADD, std::plus<>()},    {opt::SUB, std::minus<>()},
    {opt::DIV, std::divides<>()}, {opt::MUL, std::multiplies<>()},
    {opt::LE, std::less<>()},     {opt::LEQ, std::less_equal<>()},
    {opt::GE, std::greater<>()},  {opt::GEQ, std::greater_equal<>()},
    {opt::EQ, std::equal_to<>()}, {opt::NEQ, std::not_equal_to<>()}};

void Execute(const bytecode &code);

} // namespace pl0

#endif
