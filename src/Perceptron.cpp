#include "Perceptron.hpp"
#include "Predictor.hpp"
#include "Instruction.hpp"

Perceptron::Perceptron(unsigned GHRLen, unsigned tableSize, unsigned weightLen)
{
    this->GHRLen = GHRLen;
    this->tableSize = tableSize;
    this->weightLen = weightLen;


}

Perceptron::~Perceptron()
{
}

Prediction Perceptron::predict(uint32_t pc, Instruction instr)
{
    return Prediction();
}

void Perceptron::update(uint32_t pc, bool taken)
{

}

void Perceptron::reset()
{

}
