#include "bp_candidates.h"

template class VIP<Perceptron>;
template class VIP<NestLoop>;

// range of scoreboard counter
#define MIN_SCORE -2
#define MAX_SCORE 1

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
               unsigned nEntries, unsigned snapInterval, unsigned GHRLen) : 
               defaultBP(defaultBP), mCache(mCache), mCacheSize(nEntries),
               snapInterval(snapInterval), GHRLen(GHRLen)
{
    mCache->resize(nEntries);
    privateBP = (Predictor**) malloc(nEntries * sizeof(Predictor*));
    for (unsigned i = 0; i < nEntries; i++) {
        privateBP[i] = new T(prototypeBP);
    }
    scoreboard = (int*) malloc(nEntries * sizeof(int));
    reset();
}

template <class T>
Prediction VIP<T>::predict(uint64_t pc) {
    Prediction pred, defaultPred, privatePred;
    int privateIdx = mCache->get_idx(pc);
    defaultPred = defaultBP->predict(pc);
    if (privateIdx == -1) {
        pred = defaultPred;
    } else {
        privatePred = privateBP[privateIdx]->predict(pc, history, GHRLen);
        lastPrivate = privatePred.taken;
        // Score / competition based selection
        pred = scoreboard[privateIdx] < 0 ? privatePred : defaultPred;

        // Confidence-based selection
        // pred = privatePred.confidence > privateBP[privateIdx]->getThreshold() ?
        //     privatePred : defaultPred;
        // Only use for Nestloop predictor
        // pred = (privatePred.confidence > 90) ? privatePred : defaultPred;
    }
    // Always access cache with default prediction
    lastDefault = defaultPred.taken;
    return pred;
}

/**
 * @brief Private function. Update scoreboard based on previous outcome.
 * 
 * @tparam T private predictor type
 * @param idx Index of current branch in the private predictor table
 * @param dPred default predictor outcome
 * @param pPred private predictor outcome
 * @param real actual branch outcome
 */
template <class T>
void VIP<T>::updateScoreBoard(int idx, bool dPred, bool pPred, bool real) {

    if ((idx < 0) || (dPred == pPred)) return;
    if (dPred == real && scoreboard[idx] < MAX_SCORE) scoreboard[idx]++;
    else if (scoreboard[idx] > MIN_SCORE) scoreboard[idx]--;
}

template <class T>
void VIP<T>::update(uint64_t pc, bool taken) {
    if (taken != lastDefault) {
        mCache->access(pc);
    }
    int privateIdx = mCache->get_idx(pc);
    // Always update the default predictor
    // to consume the prediction made earlier
    defaultBP->update(pc, taken);
    // Conditionally update the private predictor
    if (privateIdx != -1) {
        privateBP[privateIdx]->update(pc, taken);
        updateScoreBoard(privateIdx, lastDefault, lastPrivate, taken);
    }

    // Take a snapshot of miss cache on fixed interval
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
        scoreboard[i] = 0;
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
    uint64_t scoreboardSize = 2 * mCacheSize / 8;
    return privateSize + cacheSize + defaultSize + scoreboardSize;
}