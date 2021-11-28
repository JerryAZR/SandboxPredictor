#include "bp_candidates.h"
#include <cmath>
#include <cstring>
#include <stdlib.h>

/**
 * @brief Construct a new Perceptron object
 * 
 * @param GHRLen Length of global history to be considered
 * @param tableSize Number of entries in the weights/bias table
 * @param weightLen Bit width of weights and bias
 */
Perceptron::Perceptron(unsigned GHRLen, unsigned tableSize, unsigned weightLen)
{
    this->GHRLen = GHRLen;
    this->tableSize = tableSize;
    this->weightLen = weightLen;
    threshold = ceil(1.93 * GHRLen) + 14;
    weightMax = ((1<<(weightLen-1))-1);
    weightMin = (-(weightMax+1));

    weights = (int*) malloc(tableSize * GHRLen * sizeof(int));
    bias = (int*) malloc(tableSize * sizeof(int));

    reset();
}

Perceptron::~Perceptron()
{
    free(weights);
    free(bias);
    weights = NULL;
    bias = NULL;
}

unsigned Perceptron::get_idx(uint64_t pc)
{
    return pc % tableSize;
}

/**
 * @brief Compute the vector product of branch history and weights, then make 
 *        prediction based on the sign of the product
 * 
 * @param pc Program Counter (address) of input branch instruction
 * @return Prediction (taken/not-taken and confidence of preidction)
 */
Prediction Perceptron::predict(uint64_t pc)
{
    unsigned idx = get_idx(pc);
    int sum = bias[idx];
    for (unsigned i = 0; i < GHRLen; i++)
    {
        if ((spec_history >> i) & 1) sum += weights[idx * GHRLen + i];
        else sum -= weights[idx * GHRLen + i];
    }
    pending_bp.enqueue(pstate(spec_history, sum));
    spec_history = (spec_history << 1) | ((sum >= 0) ? 1 : 0);
    return Prediction(sum >= 0, abs(sum));
}

Prediction Perceptron::predict(uint64_t pc, uint64_t addon, uint32_t addon_len)
{
    unsigned idx = get_idx(pc);
    int sum = bias[idx];
    uint64_t addonMask = (1 << addon_len) - 1;
    uint64_t tmpHistory = (spec_history << addon_len) | (addon & addonMask);

    for (unsigned i = 0; i < GHRLen; i++)
    {
        if ((tmpHistory >> i) & 1) sum += weights[idx * GHRLen + i];
        else sum -= weights[idx * GHRLen + i];
    }
    pending_bp.enqueue(pstate(tmpHistory, sum));

    spec_history = (spec_history << 1) | ((sum >= 0) ? 1 : 0);
    return Prediction(sum >= 0, abs(sum));
}

/**
 * @brief Update weights and bias based on history and previous branch outcome
 * 
 * @param pc Program Counter (address) of input branch instruction
 * @param taken true if this branch is known to be taken, false otherwise
 */
void Perceptron::update(uint64_t pc, bool taken)
{
    unsigned idx = get_idx(pc);
    pstate state = pending_bp.dequeue();
    int sum = state.sum;
    uint64_t prev_history = state.history;
    if ((sum >= 0) != taken || abs(sum) < threshold)
    {
        for (unsigned i = 0; i < GHRLen; i++)
        {
            if (taken == ((prev_history >> i) & 1))
            {
                if (weights[idx * GHRLen + i] < weightMax)
                {
                    weights[idx * GHRLen + i]++;
                }
            }
            else if(weights[idx * GHRLen + i] > weightMin) {
                weights[idx * GHRLen + i]--;
            }
        }
        if (taken)
        {
            if (bias[idx] < weightMax) bias[idx]++;
        }
        else if (bias[idx] > weightMin)
        {
            bias[idx]--;
        }
    }

    real_history = (real_history << 1) | (taken ? 1 : 0);
    if ((sum >= 0) != taken) spec_history = real_history;
}

/**
 * @brief Set weights and bias to 0
 * 
 */
void Perceptron::reset()
{
    memset(weights, 0, tableSize * GHRLen * sizeof(int));
    memset(bias, 0, tableSize * sizeof(int));
    real_history = 0;
    spec_history = 0;
    pending_bp.reset();
}