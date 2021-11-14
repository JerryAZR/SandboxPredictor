#ifndef __STATIC_H_
#define __STATIC_H_

#include "Predictor.hpp"
#include "Instruction.hpp"

/**
 * Static forward-not-taken backward-taken branch predictor
 * 
 */
class Static : public Predictor
{
private:
    /* data */
public:
    Prediction predict(uint32_t pc, Instruction instr);
};

#endif
