#include "Perceptron.hpp"
#include "Predictor.hpp"
#include "Instruction.hpp"

/**
 * TODO:    Allocate memory and initialize tables
 */
Perceptron::Perceptron(unsigned GHRLen, unsigned tableSize, unsigned weightLen)
{
    this->GHRLen = GHRLen;
    this->tableSize = tableSize;
    this->weightLen = weightLen;


}

/**
 * TODO:    Deallocate table memory
 */
Perceptron::~Perceptron()
{
}

/**
 * TODO:    Compute vector product of weights and history
 *          and make prediction based on the result.
 */
Prediction Perceptron::predict(uint32_t pc, Instruction instr)
{
    return Prediction();
}

/**
 * TODO:    Update predictor states using the input branch and its outcome
 */
void Perceptron::update(uint32_t pc, bool taken)
{

}

/**
 * TODO:    Reset the predictor to initial state
 */
void Perceptron::reset()
{

}
