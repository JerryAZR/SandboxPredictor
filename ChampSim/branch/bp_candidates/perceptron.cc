#include "bp_candidates.h"
#include <cmath>
#include <cstring>

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
    idxMask = clog2(tableSize) - 1;
    threshold = ceil(1.93 * GHRLen) + 14;

    weights = (int*) malloc(tableSize * GHRLen * sizeof(int));
    memset(weights, 0, tableSize * GHRLen * sizeof(int));
    bias = (int*) malloc(tableSize * sizeof(int));
    memset(bias, 0, tableSize * sizeof(int));
    history = 0;
}

Perceptron::~Perceptron()
{
    free(weights);
    free(bias);
    weights = NULL;
    bias = NULL;
}

/**
 * @brief Compute the vector product of branch history and weights, then make 
 *        prediction based on the sign of the product
 * 
 * @param pc Program Counter (address) of input branch instruction
 * @return Prediction (taken/not-taken and confidence of preidction)
 */
Prediction Perceptron::predict(uint32_t pc)
{
    unsigned idx = pc & idxMask;
    int sum = bias[idx];
    for (unsigned i = 0; i < GHRLen; i++)
    {
        if ((history >> i) & 1) sum += weights[idx * GHRLen + i];
        else sum -= weights[idx * GHRLen + i];
    }
    
    return Prediction(sum >= 0, sum);
}

/**
 * @brief Update weights and bias based on history and previous branch outcome
 * 
 * @param pc Program Counter (address) of input branch instruction
 * @param taken true if this branch is known to be taken, false otherwise
 */
void Perceptron::update(uint32_t pc, bool taken)
{
    unsigned idx = pc & idxMask;
    int sum = bias[idx];
    for (unsigned i = 0; i < GHRLen; i++)
    {
        if ((history >> i) & 1) sum += weights[idx * GHRLen + i];
        else sum -= weights[idx * GHRLen + i];
    }
    if ((sum >= 0) != taken || abs(sum) < threshold)
    {
        for (unsigned i = 0; i < GHRLen; i++)
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
 * @brief Set weights and bias to 0
 * 
 */
void Perceptron::reset()
{
    memset(weights, 0, tableSize * GHRLen * sizeof(int));
    memset(bias, 0, tableSize * sizeof(int));
    history = 0;
}