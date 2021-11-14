#include "Instruction.hpp"

bool Instruction::is_branch() {
    unsigned opcode = instr & 0x7F;
    if (opcode == OPCODE_BRANCH) return true;
    bool cb_1 = (instr & 0x3) == 1;
    bool cb_2 = (instr >> 13) & 0x7 >= 6;
    return cb_1 && cb_2;
}
