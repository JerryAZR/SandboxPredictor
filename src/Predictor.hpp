#ifndef __PREDICTOR_H_
#define __PREDICTOR_H_

#include <stdint.h>
#include "Instruction.hpp"

typedef struct Prediction
{
    bool taken; // true for taken, false for not taken
    uint32_t target; // target address (ignored if predicted not-taken)
    int confidence; // confidence (exact definition not yet decided)

    Prediction(bool taken = false, uint32_t target = 0, int confidence = 0)
    : taken(taken), target(target), confidence(confidence) {}
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
    virtual Prediction predict(uint32_t pc, Instruction instr) {return Prediction();}
    virtual void update(uint32_t pc, bool taken) {}
    virtual void reset() {}
};

inline uint32_t clog2(uint32_t x) {
    uint32_t leading_zero = __builtin_clz(x - 1);
    return 1 << (32 - leading_zero);
}

#endif
