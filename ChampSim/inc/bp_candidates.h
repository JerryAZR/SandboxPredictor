#ifndef __PREDICTOR_H_
#define __PREDICTOR_H_

#include <stdint.h>
#include <unordered_map>
#include "miss_cache.h"

typedef struct Prediction
{
    bool taken; // true for taken, false for not taken
    unsigned confidence; // confidence (high value -> very confident)

    Prediction(bool taken = false, int confidence = 0)
    : taken(taken), confidence(confidence) {}
    virtual ~Prediction() {}

    Prediction operator!() {return Prediction(!taken, confidence);}
    Prediction operator~() {return Prediction(!taken, confidence);}
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
        virtual ~Predictor() {}
        virtual Prediction predict(uint64_t pc) {return Prediction(true, 0);}
        virtual Prediction predict(uint64_t pc, uint64_t addon,
            uint32_t addon_len = 0) {return predict(pc);}
        virtual void update(uint64_t pc, bool taken) {}
        virtual void updateHistory(bool taken) {}
        virtual void reset() {}
        virtual unsigned getThreshold() {return 0;}
        virtual uint64_t sizeB() {return 0;}
        virtual std::string debug_info() {return "";}
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
    Perceptron(const Perceptron& other);
    ~Perceptron();

    Prediction predict(uint64_t pc);
    Prediction predict(uint64_t pc, uint64_t addon, uint32_t addon_len = 0);
    void update(uint64_t pc, bool taken);
    void reset();
    unsigned getThreshold() {return GHRLen * 2;}
    uint64_t sizeB() {
        return ((GHRLen + 1) * weightLen * tableSize / 8) + (GHRLen / 8);
    }
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
    unsigned counterLen;
    int counterMax, counterMin;
    int* table;

    unsigned get_idx(uint64_t pc);
public:
    Gshare(unsigned GHRLen = GSHARE_GHR_LEN,
           unsigned counterLen = GSHARE_COUNTER_LEN);
    ~Gshare();

    Prediction predict(uint64_t pc);
    Prediction predict(uint64_t pc, uint64_t addon, uint32_t addon_len = 0);
    void update(uint64_t pc, bool taken);
    void updateHistory(bool taken) {history = (history << 1) | (taken ? 1 : 0);}
    void reset();
    uint64_t sizeB() {return (1 << GHRLen) * counterLen / 8;}
};

class NestLoop : public Predictor
{
private:
    int32_t loop_counts;
    int32_t cur_counts;
    uint64_t local_pc;
    uint32_t total_pred;
    uint32_t correct_pred;
    bool last_pred;
    bool count_ready;
public:
    NestLoop();
    NestLoop(const NestLoop& other);
    virtual ~NestLoop();

    Prediction predict(uint64_t pc);
    void update(uint64_t pc, bool taken);
    void reset();
    unsigned getThreshold() {return 90;}
    std::string debug_info();
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
    virtual ~Tournament();

    Prediction predict(uint64_t pc);
    void update(uint64_t pc, bool taken);
    void reset();
    uint64_t sizeB() {return bp1->sizeB() + bp2->sizeB();}
};

template <class T>
class VIP : public Predictor
{
    private:
        Predictor* defaultBP;
        MissCache* mCache;
        unsigned mCacheSize;
        Predictor** privateBP;
        unsigned snapInterval;
        unsigned GHRLen;
        unsigned currCount;
        uint64_t history;
        int* scoreboard;
        bool lastDefault;
        bool lastPrivate;

        void updateScoreBoard(int idx, bool dPred, bool pPred, bool real);
    public:
        VIP(Predictor* defaultBP, T& prototypeBP, MissCache* mCache,
            unsigned mCacheSize = 8, unsigned snapInterval = 2048,
            unsigned GHRLen = 8);

        Prediction predict(uint64_t pc);
        void update(uint64_t pc, bool taken);
        void reset();
        uint64_t sizeB();
        std::string debug_info();
};

typedef struct tage_entry_t
{
    uint64_t tag;
    int counter;
    int useful;

    tage_entry_t(uint64_t t = 0, int c = 0, int u = 0) :
    tag(t), counter(c), useful(u) {}
    bool valid() {return useful >= 0;}
    bool weak() {return useful == 0 && (counter == 0 || counter == -1);}
    bool taken() {return counter >= 0;}
    Prediction toPrediction() {
        unsigned raw_conf = (counter < 0) ? (-1 * counter) : (counter + 1);
        bool taken = (counter >= 0);
        return Prediction(taken, raw_conf + useful);
    }
} tage_entry_t;

class TagTable
{
private:
    unsigned idxLen;
    unsigned tagLen;
    unsigned nEntries;
    tage_entry_t* content;
    int counterMax;
    int counterMin;
    int usefulMax;
public:
    TagTable(unsigned idxLen, unsigned tagLen);
    virtual ~TagTable();

    uint64_t get_idx(uint64_t pc, uint64_t history);
    uint64_t get_tag(uint64_t pc, uint64_t history);
    tage_entry_t lookup(uint64_t pc, uint64_t history);
    bool add(uint64_t pc, uint64_t history);
    void update(uint64_t pc, uint64_t history, bool taken, int useful);
    void decay();
    void decay(uint64_t pc, uint64_t history);
    void soft_reset(unsigned mask = 0);
    void hard_reset();
    std::string debug_info();
    uint64_t sizeB() {return nEntries * (tagLen+4) / 8;}
};

class TAGE : public Predictor
{
private:
    Predictor* baseBP;
    unsigned nTables;
    TagTable** tables;
    uint64_t history;
    unsigned GHRLen;
    tage_entry_t mainPred;
    int mainProvider;
    tage_entry_t altPred;
    int altProvider;
    bool basePred;
    bool overallPred;
    uint64_t resetPeriod;
    uint64_t branchCount;
    unsigned resetMask;
public:
    TAGE(Predictor* base, unsigned nTables, unsigned idxLen);
    virtual ~TAGE();

    Prediction predict(uint64_t pc);
    void update(uint64_t pc, bool taken);
    void reset();
    uint64_t sizeB();
};

class LTAGE : public Predictor
{
private:
    TAGE* tageBP;
    NestLoop* loopBP;
    unsigned loopThreshold;
public:
    LTAGE(TAGE* tageBP, unsigned loopTH = 90);
    virtual ~LTAGE();

    Prediction predict(uint64_t pc);
    void update(uint64_t pc, bool taken);
    void reset();
    std::string debug_info();
    uint64_t sizeB();
};

#endif
