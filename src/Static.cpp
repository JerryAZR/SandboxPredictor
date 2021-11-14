#include "Static.hpp"
#include "Instruction.hpp"

Prediction Static::predict(uint32_t pc, Instruction instr) {
    Prediction pred;
    pred.target = pc + instr.branch_offset();
    pred.taken = instr.branch_offset() < 0;
    pred.confidence = pred.taken ? 1 : -1;
    return pred;
}
