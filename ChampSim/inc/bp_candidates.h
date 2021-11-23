#ifndef __PREDICTOR_H_
#define __PREDICTOR_H_

#include <stdint.h>

typedef struct Prediction
{
    bool taken; // true for taken, false for not taken
    unsigned confidence; // confidence (high value -> very confident)

    Prediction(bool taken = false, int confidence = 0)
    : taken(taken), confidence(confidence) {}
} Prediction;

/**
 * @brief Base class for branch predictors
 *        This base class implements a static taken predictor
 * 
 */
class Predictor
{
    private:
        static uint64_t get_reset_score(uint64_t realHistory,
            uint64_t bpHistory, uint64_t baseScore, double decayRate)
        {
            uint64_t diffHistory = realHistory ^ bpHistory;
            uint64_t totalScore = 0;
            for (uint64_t curr = baseScore; curr > 0; curr *= decayRate) {
                totalScore += (diffHistory & 1) ? curr : 0;
                diffHistory >>= 1;
            }
            return totalScore;
        }
    public:
        virtual Prediction predict(uint64_t pc) {return Prediction(true, 0);}
        virtual void update(uint64_t pc, bool taken) {}
        virtual void reset() {}
};

inline uint32_t clog2(uint32_t x) {
    uint32_t leading_zero = __builtin_clz(x - 1);
    return 1 << (32 - leading_zero);
}

typedef struct pstate
{
    uint64_t history;
    int sum;

    pstate(uint64_t h = 0, int s = 0) : history(h), sum(s) {}
} pstate;


#define QUEUE_SIZE 4
class Pstate_queue
{
    private:
        unsigned head; // dequeue from head
        unsigned tail; // enqueue at tail
        pstate storage[QUEUE_SIZE];
    public:
        Pstate_queue() : head(0), tail(0) {}
        pstate dequeue() {
            pstate retval = storage[head];
            head = (head + 1) % QUEUE_SIZE;
            return retval;
        }
        void enqueue(pstate state) {
            storage[tail] = state;
            tail = (tail + 1) % QUEUE_SIZE;
        }
        void reset() {
            head = 0;
            tail = 0;
        }
};


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
    int weightMax, weightMin;
    uint64_t real_history, spec_history;
    Pstate_queue pending_bp;
    unsigned GHRLen, tableSize, weightLen;
    int threshold;

    unsigned get_idx(uint64_t pc);
public:
    Perceptron(unsigned GHRLen = PERCEPTRON_GHR_LEN,
               unsigned tableSize = PERCEPTRON_TABLE_SIZE,
               unsigned weightLen = PERCEPTRON_WEIGHT_LEN);
    ~Perceptron();

    Prediction predict(uint64_t pc);
    void update(uint64_t pc, bool taken);
    void reset();
};

#define GSHARE_GHR_LEN 8
#define GSHARE_COUNTER_LEN 2

/**
 * @brief Class for gshare predictor
 * 
 */
class Gshare : public Predictor
{
private:
    unsigned GHRLen;
    uint64_t history;
    int counterMax, counterMin;
    int* table;

    unsigned get_idx(uint64_t pc);
public:
    Gshare(unsigned GHRLen = GSHARE_GHR_LEN,
           unsigned counterLen = GSHARE_COUNTER_LEN);
    ~Gshare();

    Prediction predict(uint64_t pc);
    void update(uint64_t pc, bool taken);
    void reset();
};

#define NUM_BUCKET 8

class Tournament : public Predictor
{
private:
    Predictor* bp1;
    Predictor* bp2;
    unsigned numBuckets;
    uint64_t* realHistory;
    uint64_t* bp1History;
    uint64_t* bp2Histpry;
    int32_t* preference;
    
public:
    Tournament(Predictor* bp1, Predictor* bp2, unsigned nbuckets = NUM_BUCKET);
    ~Tournament();

    Prediction predict(uint64_t pc);
    void update(uint64_t pc, bool taken);
    void reset();
};

#endif
