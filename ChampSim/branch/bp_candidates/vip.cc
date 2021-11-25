#include "bp_candidates.h"

VIP::VIP(Predictor* defaultBP, MissCache* mCache, unsigned nEntries,
        unsigned snapInterval) : defaultBP(defaultBP), mCache(mCache),
        mCacheSize(nEntries), snapInterval(snapInterval)
{
    mCache->resize(nEntries);
    privateBP = (Predictor**) malloc(nEntries * sizeof(Predictor*));
    for (unsigned i = 0; i < nEntries; i++) {
        privateBP[i] = new Gshare(12, 1);
    }
    reset();
}

Prediction VIP::predict(uint64_t pc) {
    Prediction pred, defaultPred;
    int privateIdx = mCache->get_idx(pc);
    defaultPred = defaultBP->predict(pc);
    if (privateIdx == -1) {
        pred = defaultPred;
    } else {
        pred = privateBP[privateIdx]->predict(pc);
    }
    // Always access cache with default prediction
    lastPrediction[pc] = defaultPred.taken;
    return pred;
}

void VIP::update(uint64_t pc, bool taken) {
    if (taken != lastPrediction[pc]) {
        mCache->access(pc);
    }
    int privateIdx = mCache->get_idx(pc);
    // Always update the default predictor
    // to consume the prediction made earlier
    defaultBP->update(pc, taken);
    // Conditionally update the private predictor
    if (privateIdx != -1) {
        privateBP[privateIdx]->update(pc, taken);
    }

    // Take a snapshot on miss cache on fixed interval
    currCount++;
    if (currCount == snapInterval) {
        currCount = 0;
        mCache->snapshot();
    }
}

void VIP::reset() {
    defaultBP->reset();
    mCache->reset();
    for (unsigned i = 0; i < mCacheSize; i++) {
        privateBP[i]->reset();
    }
    currCount = 0;
}
