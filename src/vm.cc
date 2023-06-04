#include "vm.h"

#include <iostream>

void pl0::Execute(const bytecode &code) {
  int program_counter = 0;
  auto code_length = static_cast<int>(code.size());
  auto *top_frame = new StackFrame{code_length, nullptr, nullptr};

  while (program_counter < code_length) {
    auto ins = code[program_counter++];

    switch (ins.op) {
      case opcode::LIT:
        top_frame->Push(ins.address);
        break;
      case opcode::LOD:
        top_frame->Push(top_frame->Local(ins.level, ins.address));
        break;
      case opcode::STO:
        top_frame->Local(ins.level, ins.address) = top_frame->Pop();
        break;
      case opcode::CAL:
        top_frame = new StackFrame{
            program_counter, top_frame, top_frame->Resolve(ins.level)};
        program_counter = ins.address;
        break;
      case opcode::INT:
        top_frame->Allocate(ins.address - 3);
        break;
      case opcode::JMP:
        program_counter = ins.address;
        break;
      case opcode::JPC:
        if (!top_frame->Pop()) { program_counter = ins.address; }
        break;
      case opcode::OPR:
        if (ins.address == *opt::ODD) {
          int const result = top_frame->Pop() % 2;
          top_frame->Push(result);
        } else if (ins.address == *opt::READ) {
          int tmp;
          std::cin >> tmp;
          top_frame->Push(tmp);
        } else if (ins.address == *opt::WRITE) {
          std::cout << top_frame->Pop() << '\n';
        } else if (ins.address == *opt::RET) {
          top_frame->Leave(program_counter, top_frame);
        } else {
          const int rhs = top_frame->Pop(), lhs = top_frame->Pop();
          auto f = opt2functor.find(opt(ins.address))->second;
          top_frame->Push(f(lhs, rhs));
        }
        break;
    }
  }
}
