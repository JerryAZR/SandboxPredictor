#ifndef __PREDICTOR_H_
#define __PREDICTOR_H_

#include <stdint.h>
#include "Instruction.hpp"

typedef struct Prediction
{
    bool taken; // true for taken, false for not taken
    uint32_t target; // target address (ignored if predicted not-taken)
    int confidence; // confidence (exact definition not yet decided)

    Prediction(bool p = false, uint32_t t = 0) : taken(p), target(t) {}
} Prediction;

/**
 * Branch predictor base class
 * Static not-taken predictor
 */
class Predictor
{
private:
    /* data */
public:
    Prediction predict(uint32_t pc, Instruction instr) {return Prediction();}
    void update(uint32_t pc, bool taken) {}
    void reset() {}
};

#endif
