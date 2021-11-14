#include "Instruction.hpp"

bool Instruction::is_branch() {
    unsigned opcode = instr & 0x7F;
    if (opcode == OPCODE_BRANCH) return true;
    bool cb_1 = (instr & 0x3) == 1;
    bool cb_2 = (instr >> 13) & 0x7 >= 6;
    return cb_1 && cb_2;
}

uint32_t Instruction::opcode() {
    return instr & 0x7F;
}

// This function is not verified yet!
// Hope I got all the bits right.
int32_t Instruction::branch_offset() {
    int32_t imm4_1 = (instr >> 8) & 0xF;
    int32_t imm11 = (instr >> 7) & 1;
    int32_t imm10_5 = (instr >> 25) & 0x3F;
    int32_t offset = ((int) instr < 0) ? 0xFFFFF000 : 0;
    offset = offset + (imm4_1 << 1) + (imm11 << 11) + (imm10_5 << 5);
    return offset;
}
