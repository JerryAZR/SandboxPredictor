#include "bp_candidates.h"

template class VIP<Perceptron>;
template class VIP<NestLoop>;

/**
 * @brief Construct a new VIP<T>::VIP object
 * 
 * @tparam T Class for private branch predictor. Mush be derived from class
 * Predictor, or implement equivalent public member functions
 * 
 * @param defaultBP The default branch predictor
 * @param prototypeBP Prototype for private predictor
 * @param mCache Miss cache
 * @param nEntries Number of entries in miss cache. Also the number of private BPs
 * @param snapInterval The number of predictions between each mCache snapshot
 */
template <class T>
VIP<T>::VIP(Predictor* defaultBP, T& prototypeBP, MissCache* mCache,
               unsigned nEntries, unsigned snapInterval) : 
               defaultBP(defaultBP), mCache(mCache), mCacheSize(nEntries),
               snapInterval(snapInterval)
{
    mCache->resize(nEntries);
    privateBP = (Predictor**) malloc(nEntries * sizeof(Predictor*));
    for (unsigned i = 0; i < nEntries; i++) {
        privateBP[i] = new T(prototypeBP);
    }
    reset();
}

template <class T>
Prediction VIP<T>::predict(uint64_t pc) {
    Prediction pred, defaultPred;
    int privateIdx = mCache->get_idx(pc);
    defaultPred = defaultBP->predict(pc);
    if (privateIdx == -1) {
        pred = defaultPred;
    } else {
        pred = privateBP[privateIdx]->predict(pc, history, 8);
    }
    // Always access cache with default prediction
    lastPrediction[pc] = defaultPred.taken;
    return pred;
}

template <class T>
void VIP<T>::update(uint64_t pc, bool taken) {
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

    // Update history
    history = (history << 1) | (taken ? 1 : 0);
}

template <class T>
void VIP<T>::reset() {
    defaultBP->reset();
    mCache->reset();
    for (unsigned i = 0; i < mCacheSize; i++) {
        privateBP[i]->reset();
    }
    currCount = 0;
    history = 0;
}

template <class T>
std::string VIP<T>::debug_info(){
    std::string ret = "-----predictor debug info-----\n";
    for (unsigned i = 0; i < mCacheSize; i++) {
        ret += privateBP[i]->debug_info();
    }
    return ret += mCache->debug_info();
}

template <class T>
uint64_t VIP<T>::sizeB() {
    uint64_t privateSize = mCacheSize * privateBP[0]->sizeB();
    uint64_t cacheSize = mCache->sizeB();
    uint64_t defaultSize = defaultBP->sizeB();
    return privateSize + cacheSize + defaultSize;
}