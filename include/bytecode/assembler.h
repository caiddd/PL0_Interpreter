#ifndef BYTECODE_ASSEMBLER_H
#define BYTECODE_ASSEMBLER_H

#include "bytecode.h"
#include "../util.h"

namespace pl0 {

class assembler {
    bytecode code_;

    void Emit(opcode op, int level, int address);
public:
    int  GetNextAddress();
    int  GetLastAddress();
    void Load(int value);
    void Load(int distance, int index);
    void Store(int distance, int index);
    void        Call(int distance, int entry);
    Backpatcher Call(int caller_level);
    void        Branch(int target);
    Backpatcher Branch();
    void        BranchIfFalse(int target);
    Backpatcher BranchIfFalse();
    void Enter(int scope_var_count);
    void leave();
    void Read();
    void Write();
    void Operation(Token tk);

    const bytecode &code();
};

} // namespace pl0

#endif
