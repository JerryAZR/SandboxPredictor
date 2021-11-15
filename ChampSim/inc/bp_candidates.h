#ifndef __PREDICTOR_H_
#define __PREDICTOR_H_

#include <stdint.h>

typedef struct Prediction
{
    bool taken; // true for taken, false for not taken
    int confidence; // confidence (exact definition not yet decided)

    Prediction(bool taken = false, int confidence = 0)
    : taken(taken), confidence(confidence) {}
} Prediction;

/**
 * @brief Base class for branch predictors
 *        This base class implements a static not-taken predictor
 * 
 */
class Predictor
{
public:
    virtual Prediction predict(uint32_t pc) {return Prediction();}
    virtual void update(uint32_t pc, bool taken) {}
    virtual void reset() {}
};

inline uint32_t clog2(uint32_t x) {
    uint32_t leading_zero = __builtin_clz(x - 1);
    return 1 << (32 - leading_zero);
}


#define PERCEPTRON_GHR_LEN 8
#define PERCEPTRON_TABLE_SIZE 128
#define PERCEPTRON_WEIGHT_LEN 9

/**
 * @brief Class for perceptron predictor
 *        To be used inside the sandbox predictor
 * 
 */
class Perceptron : public Predictor
{
private:
    int* weights;
    int* bias;
    uint64_t history;
    unsigned GHRLen;
    unsigned tableSize;
    unsigned weightLen;
    unsigned idxMask;
    int threshold;
public:
    Perceptron(unsigned GHRLen = PERCEPTRON_GHR_LEN,
               unsigned tableSize = PERCEPTRON_TABLE_SIZE,
               unsigned weightLen = PERCEPTRON_WEIGHT_LEN);
    ~Perceptron();

    Prediction predict(uint32_t pc);
    void update(uint32_t pc, bool taken);
    void reset();
};


#endif
