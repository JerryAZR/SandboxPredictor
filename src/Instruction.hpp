#ifndef __INSTRUCTION_H_
#define __INSTRUCTION_H_

#include <stdint.h>
#include <string>

#define OPCODE_BRANCH 0x63

class Instruction
{
private:
    uint32_t instr;
public:
    Instruction(uint32_t data = 0) : instr(data) {}

    bool is_branch();
};

#endif
