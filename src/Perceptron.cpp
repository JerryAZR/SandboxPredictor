#include "Perceptron.hpp"
#include "Predictor.hpp"
#include "Instruction.hpp"
#include <cstring>
#include <cmath>

/**
 * TODO:    Allocate memory and initialize tables
 */
Perceptron::Perceptron(unsigned GHRLen, unsigned tableSize, unsigned weightLen)
{
    this->GHRLen = GHRLen;
    this->tableSize = tableSize;
    this->weightLen = weightLen;
    idxMask = clog2(tableSize) - 1;
    threshold = ceil(1.93 * GHRLen) + 14;

    weights = (int*) malloc(tableSize * GHRLen * sizeof(int));
    memset(weights, 0, tableSize * GHRLen * sizeof(int));
    bias = (int*) malloc(tableSize * sizeof(int));
    memset(bias, 0, tableSize * sizeof(int));
    history = 0;
}

/**
 * TODO:    Deallocate table memory
 */
Perceptron::~Perceptron()
{
    free(weights);
    free(bias);
    weights = NULL;
    bias = NULL;
}

/**
 * TODO:    Compute vector product of weights and history
 *          and make prediction based on the result.
 * 
 *      Let weights[idx][0] and history LSB be the most recent history.
 */
Prediction Perceptron::predict(uint32_t pc, Instruction instr)
{
    unsigned idx = pc & idxMask;
    int sum = bias[idx];
    for (int i = 0; i < GHRLen; i++)
    {
        if ((history >> i) & 1) sum += weights[idx * GHRLen + i];
        else sum -= weights[idx * GHRLen + i];
    }
    
    return Prediction(sum >= 0, pc + instr.branch_offset(), sum);
}

/**
 * TODO:    Update predictor states using the input branch and its outcome
 */
void Perceptron::update(uint32_t pc, bool taken)
{
    unsigned idx = pc & idxMask;
    int sum = bias[idx];
    for (int i = 0; i < GHRLen; i++)
    {
        if ((history >> i) & 1) sum += weights[idx * GHRLen + i];
        else sum -= weights[idx * GHRLen + i];
    }
    if ((sum >= 0) != taken || abs(sum) < threshold)
    {
        for (int i = 0; i < GHRLen; i++)
        {
            if (taken == ((history >> i) & 1)) weights[idx * GHRLen + i]++;
            else weights[idx * GHRLen + i]--;
        }
        if (taken) bias[idx]++;
        else bias[idx]--;
    }

    history = (history << 1) | (taken ? 1 : 0);
}

/**
 * TODO:    Reset the predictor to initial state
 */
void Perceptron::reset()
{
    memset(weights, 0, tableSize * GHRLen * sizeof(int));
    memset(bias, 0, tableSize * sizeof(int));
    history = 0;
}
