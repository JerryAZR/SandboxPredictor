#include "Sandbox.hpp"
#include "Predictor.hpp"
#include <cmath>

/**
 * @brief Predict branch outcome based on pc and instruction
 * NOTE: Need to implement BTB if instruction is not available
 * 
 * @param pc Program counter, or address of input instruction
 * @param instr Input instruction
 * @return Prediction 
 * 
 * TODO:    Try different decision algorithms
 */
Prediction Sandbox::predict(uint32_t pc, Instruction instr)
{
    Prediction pred1 = complexBP->predict(pc, instr);
    if (abs(pred1.confidence) > 10) return pred1;
    else return basicBP->predict(pc, instr);
}

void Sandbox::update(uint32_t pc, bool taken)
{
    basicBP->update(pc, taken);
    complexBP->update(pc, taken);
}

void Sandbox::reset()
{
    basicBP->reset();
    complexBP->reset();
}
