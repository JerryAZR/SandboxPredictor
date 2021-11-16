#include "bp_candidates.h"
#include <cmath>
#include <cstring>

#define MAX_WEIGHT		((1<<(PERCEPTRON_WEIGHT_LEN-1))-1)
#define MIN_WEIGHT		(-(MAX_WEIGHT+1))

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

/**
 * @brief Compute the vector product of branch history and weights, then make 
 *        prediction based on the sign of the product
 * 
 * @param pc Program Counter (address) of input branch instruction
 * @return Prediction (taken/not-taken and confidence of preidction)
 */
Prediction Perceptron::predict(uint32_t pc)
{
    unsigned idx = pc % tableSize;
    int sum = bias[idx];
    for (unsigned i = 0; i < GHRLen; i++)
    {
        if ((spec_history >> i) & 1) sum += weights[idx * GHRLen + i];
        else sum -= weights[idx * GHRLen + i];
    }
    prev_history = real_history;
    prev_sum = sum;
    spec_history = (spec_history << 1) | ((sum >= 0) ? 1 : 0);
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
    unsigned idx = pc % tableSize;
    int sum = prev_sum;
    if ((sum >= 0) != taken || abs(sum) < threshold)
    {
        for (unsigned i = 0; i < GHRLen; i++)
        {
            if (taken == ((prev_history >> i) & 1)) weights[idx * GHRLen + i]++;
            else weights[idx * GHRLen + i]--;
        }
        if (taken) bias[idx]++;
        else bias[idx]--;
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
    prev_history = 0;
    prev_sum = 0;
}