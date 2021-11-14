#ifndef __INSTRUCTION_H_
#define __INSTRUCTION_H_

#include <stdint.h>
#include <string>

#define OPCODE_BRANCH 0x63

/**
 * Instruction
 * This class is a decoder for the RV32I ISA.
 * TODO: Implement helper functions such as get_opcode and get_offset
 */
class Instruction
{
private:
    uint32_t instr;
public:
    Instruction(uint32_t data = 0) : instr(data) {}

    uint32_t data() {return instr;}

    bool is_branch();

    int32_t branch_offset();
    uint32_t opcode();
};

#endif
