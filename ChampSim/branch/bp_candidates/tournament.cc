#include "bp_candidates.h"
#include <stdint.h>
#include <stdlib.h>
#include <cstring>

Tournament::Tournament(Predictor* bp1, Predictor* bp2, unsigned nbuckets)
: bp1(bp1), bp2(bp2), numBuckets(nbuckets) {
    realHistory = (uint64_t*) malloc(numBuckets * sizeof(uint64_t));
    bp1History = (uint64_t*) malloc(numBuckets * sizeof(uint64_t));
    bp2Histpry = (uint64_t*) malloc(numBuckets * sizeof(uint64_t));
    preference = (uint32_t*) malloc(numBuckets * sizeof(uint32_t));

    reset();
}

Tournament::~Tournament() {
    free(realHistory);
    free(bp1History);
    free(bp2Histpry);

    bp1 = NULL;
    bp2 = NULL;
    realHistory = NULL;
    bp1History = NULL;
    bp2Histpry = NULL;
}

Prediction Tournament::predict(uint64_t pc) {
    Prediction pred1 = bp1->predict(pc);
    Prediction pred2 = bp2->predict(pc);

    bool final_prediction;
    unsigned idx = pc % numBuckets;

    // Update prediction history
    bp1History[idx] = (bp1History[idx] << 1) | (pred1.taken ? 1 : 0);
    bp2Histpry[idx] = (bp2Histpry[idx] << 1) | (pred2.taken ? 1 : 0);

    // Make the final decision
    final_prediction = preference[idx] > 0 ? pred1.taken : pred2.taken;

    return final_prediction;
}

void Tournament::update(uint64_t pc, bool taken) {
    bp1->update(pc, taken);
    bp2->update(pc, taken);

    unsigned idx = pc % numBuckets;

    // Update real history
    realHistory[idx] = (realHistory[idx] << 1) | (taken ? 1 : 0);
    bool bp1Correct = (bp1History[idx] & 1) == (realHistory[idx] & 1);
    bool bp2Correct = (bp2Histpry[idx] & 1) == (realHistory[idx] & 1);
    if (bp1Correct) preference[idx]++;
    if (bp2Correct) preference[idx]--;

    // Not using reset score right now because it doesn't increase accuracy
}

void Tournament::reset() {
    memset(realHistory, 0, numBuckets * sizeof(uint64_t));
    memset(bp1History, 0, numBuckets * sizeof(uint64_t));
    memset(bp2Histpry, 0, numBuckets * sizeof(uint64_t));
    memset(preference, 0, numBuckets * sizeof(uint32_t));
    bp1->reset();
    bp2->reset();
}