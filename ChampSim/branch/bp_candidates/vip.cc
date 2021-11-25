#include "bp_candidates.h"

VIP::VIP(Predictor* defaultBP, MissCache* mCache, unsigned nEntries)
: defaultBP(defaultBP), mCache(mCache), mCacheSize(nEntries)
{
    mCache->resize(nEntries);
    privateBP = (Predictor**) malloc(nEntries * sizeof(Predictor*));
    for (unsigned i = 0; i < nEntries; i++) {
        privateBP[i] = new Gshare(8, 1);
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
    lastPrediction = defaultPred.taken;
    return pred;
}

void VIP::update(uint64_t pc, bool taken) {
    if (taken != lastPrediction) {
        mCache->access(pc);
    }
    int privateIdx = mCache->get_idx(pc);
    if (privateIdx == -1) {
        defaultBP->update(pc, taken);
    } else {
        privateBP[privateIdx]->update(pc, taken);
    }
}

void VIP::reset() {
    defaultBP->reset();
    mCache->reset();
    for (unsigned i = 0; i < mCacheSize; i++) {
        privateBP[i]->reset();
    }
}
