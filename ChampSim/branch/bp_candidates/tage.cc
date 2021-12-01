#include "bp_candidates.h"
#include <stdlib.h>

TAGE::TAGE(Predictor* base, unsigned nTables, unsigned idxLen) :
baseBP(base), nTables(nTables)
{
    GHRLen = 1 << nTables;
    tables = (TagTable**) malloc(nTables * sizeof(TagTable*));
    for (unsigned i = 0; i < nTables; i++) {
        tables[i] = new TagTable(idxLen, 2 << i);
    }
    resetPeriod = 1 << 18;
    reset();
}

TAGE::~TAGE() {
    for (unsigned i = 0; i < nTables; i++) {
        delete tables[i];
    }
}

void TAGE::reset() {
    for (unsigned i = 0; i < nTables; i++) {
        tables[i]->hard_reset();
    }
    baseBP->reset();
    history = 0;
    mainPred = tage_entry_t(0, 0, -1);
    altPred = tage_entry_t(0, 0, -1);
    mainProvider = -1;
    altProvider = -1;
    branchCount = 0;
    resetMask = 1;
}

Prediction TAGE::predict(uint64_t pc) {
    mainPred.useful = -1;
    altPred.useful = -1;
    mainProvider = -1;
    altProvider = -1;
    for (int i = nTables-1; i >= 0; i--) {
        // look for main provider if it is not found yet
        mainPred = tables[i]->lookup(pc, history);
        if (mainPred.valid()) {
            mainProvider = i;
            break;
        }
    }
    for (int i = mainProvider-1; i >= 0; i--) {
        // look for alt provider if main privider has been set
        altPred = tables[i]->lookup(pc, history);
        if (altPred.valid()) {
            altProvider = i;
            break;
        }
    }
    // default privider has lowest priority
    Prediction pred = baseBP->predict(pc);
    basePred = pred.taken;
    if (altPred.valid()) pred = altPred.toPrediction();
    // Use main privider only if it's not "weak"
    if (mainPred.valid() && (!mainPred.weak())) pred = mainPred.toPrediction();
    overallPred = pred.taken;
    return pred;
}

void TAGE::update(uint64_t pc, bool taken) {
    bool altResult = altPred.valid() ? altPred.taken() : basePred;
    int useful = 0;
    // Update useful counter if main and alt results are different
    if (mainPred.valid() && (mainPred.taken() != altResult)) {
        useful = (mainPred.taken() == taken) ? 1 : -1;
    }
    // Update main provider
    if (mainPred.valid()) {
        tables[mainProvider]->update(pc, history, taken, useful);
    }
    // Update alt provider
    if (altPred.valid() && mainPred.useful == 0) {
        useful = (altPred.taken() == taken) ? 1 : -1;
        tables[altProvider]->update(pc, history, taken, useful);
    }
    // Update base predictor
    if (!(mainPred.valid() && altPred.valid())) {
        baseBP->update(pc, taken);
    } else {
        baseBP->updateHistory(taken);
    }
    
    // if overall prediction is wrong, try to allocate new entry
    long randNum = random();
    bool added = false;
    bool requestNewEntry = (overallPred != taken);
    // But if main prediction is weak (new) and correct, don't allocate
    if (mainPred.valid() && mainPred.weak()) {
        requestNewEntry = (mainPred.taken() == taken);
    }

    if (requestNewEntry && (mainProvider < (int)(nTables-1))) {
        // If there are 3 tables available, the chance of adding to one is
        // 0.5, 0.25, 0.125 respectively. Note that there's a chance that the
        // entry is not added even if there is empty space. We'll handle this
        // case later.
        for (unsigned i = mainProvider + 1; i < nTables; i++) {
            // Add to the next table with 0.5 probability
            if (randNum & 1) {
                added = tables[i]->add(pc, history);
                if (added) break;
            }
            randNum = randNum >> 1; // Get a new random bit
        }
        if (!added) {
            // The overall probability of adding to a table when there are 3
            // available is 0.625, 0.25, 0.125
            for (unsigned i = mainProvider + 1; i < nTables; i++) {
            // Add to the next free table with 1 probability
                added = tables[i]->add(pc, history);
                if (added) break;
            }
        }
        if (!added) {
            // If the entry is still not added, that means all available spaces
            // are occupied. In this case we decrease the useful counter of
            // the existing entries
            for (unsigned i = mainProvider + 1; i < nTables; i++) {
                tables[i]->decay(pc, history);
            }
        }
    }
    // Finally we can update the history
    history = (history << 1) | (taken ? 1 : 0);
    uint64_t histMask = ((uint64_t) 1 << GHRLen) - 1;
    history = history & histMask;
    // Check soft reset condition
    branchCount++;
    if (branchCount == resetPeriod) {
        for (unsigned i = 0; i < nTables; i++) {
            tables[i]->soft_reset(resetMask);
        }
        resetMask ^= 3;
    }
}

uint64_t TAGE::sizeB() {
    uint64_t size = baseBP->sizeB();
    for (unsigned i = 0; i < nTables; i++) {
        size += tables[i]->sizeB();
    }
    return size;
}
