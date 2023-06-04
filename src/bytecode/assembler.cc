#include "bytecode/assembler.h"

enum { IGNORE = 0 };

namespace pl0 {

void assembler::Emit(opcode op, int level, int address) {
    code_.push_back({ op, level, address });
}

int assembler::GetNextAddress() {
    return static_cast<int>(code_.size());
}

int assembler::GetLastAddress() {
    return static_cast<int>(code_.size() - 1);
}

void assembler::Load(int value) {
    Emit(opcode::LIT, IGNORE, value);
}

void assembler::Load(int distance, int index) {
    Emit(opcode::LOD, distance, index);
}

void assembler::Store(int distance, int index) {
    Emit(opcode::STO, distance, index);
}

void assembler::Call(int distance, int entry) {
    Emit(opcode::CAL, distance, entry);
}

Backpatcher assembler::Call(int caller_level) {
    Emit(opcode::CAL, caller_level, IGNORE);
    return Backpatcher { code_, GetLastAddress() };
}

void assembler::Branch(int target) {
    Emit(opcode::JMP, IGNORE, target);
}

Backpatcher assembler::Branch() {
    Emit(opcode::JMP, IGNORE, IGNORE);
    return Backpatcher { code_, GetLastAddress() };
}

void assembler::BranchIfFalse(int target) {
    Emit(opcode::JPC, IGNORE, target);
}

Backpatcher assembler::BranchIfFalse() {
    Emit(opcode::JPC, IGNORE, IGNORE);
    return Backpatcher { code_, GetLastAddress() };
}

void assembler::Enter(int scope_var_count) {
    Emit(opcode::INT, IGNORE, scope_var_count);
}

void assembler::leave() {
    Emit(opcode::OPR, IGNORE, *opt::RET);
}

void assembler::Read() {
    Emit(opcode::OPR, IGNORE, *opt::READ);
}

void assembler::Write() {
    Emit(opcode::OPR, IGNORE, *opt::WRITE);
}

void assembler::Operation(Token tk) {
    auto iter = token2opt.find(tk);
    if (iter == token2opt.end()) {
        throw GeneralError("token ", *tk, " cannot be used as operator");
    }
    Emit(opcode::OPR, IGNORE, *iter->second);
}

const bytecode &assembler::code() {
    return code_;
}

} // namespace pl0
