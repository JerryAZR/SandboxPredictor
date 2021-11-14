#ifndef __PERCEPTRON_H_
#define __PERCEPTRON_H_

#include <stdint.h>
#include "Predictor.hpp"
#include "parameters.hpp"
#include "Instruction.hpp"

/**
 * Perceptron predictor
 */
class Perceptron : public Predictor
{
private:
    int* weights;
    int* bias;
    uint64_t* history;
    unsigned GHRLen;
    unsigned tableSize;
    unsigned weightLen;
public:
    Perceptron(unsigned GHRLen = PERCEPTRON_GHR_LEN,
               unsigned tableSize = PERCEPTRON_TABLE_SIZE,
               unsigned weightLen = PERCEPTRON_WEIGHT_LEN);
    ~Perceptron();

    Prediction predict(uint32_t pc, Instruction instr);
    void update(uint32_t pc, bool taken);
    void reset();
};

#endif
