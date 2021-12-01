#include "bp_candidates.h"
#include <cstring>
#include <stdlib.h>

Gshare::Gshare(unsigned GHRLen, unsigned counterLen) :
GHRLen(GHRLen), counterLen(counterLen)
{
    counterMax = (1 << (counterLen - 1)) - 1;
    counterMin = -1 - counterMax;

    unsigned tableSize = 1 << GHRLen;
    table = (int*) malloc(tableSize * sizeof(int));

    reset();
}

Gshare::~Gshare()
{
    free(table);
    table = NULL;
}

Prediction Gshare::predict(uint64_t pc)
{
    int counter = table[get_idx(pc)];
    bool taken = counter >= 0;
    unsigned confidence = taken ? (counter + 1) : (0 - counter);
    return Prediction(taken, confidence);
}

Prediction Gshare::predict(uint64_t pc, uint64_t addon, uint32_t addon_len)
{
    uint64_t addonMask = (1 << addon_len) - 1;
    uint64_t tmpHistory = (history << addon_len) | (addon & addonMask);
    unsigned idx = (tmpHistory ^ pc) & ((1 << GHRLen) - 1);
    int counter = table[idx];
    bool taken = counter >= 0;
    unsigned confidence = taken ? (counter + 1) : (0 - counter);
    return Prediction(taken, confidence);
}

void Gshare::update(uint64_t pc, bool taken)
{
    unsigned idx = get_idx(pc);
    int counter = table[idx];
    if (taken && counter < counterMax) table[idx] = counter + 1;
    else if (!taken && counter > counterMin) table[idx] = counter - 1;
    history = (history << 1) | (taken ? 1 : 0);
}

void Gshare::reset()
{
    unsigned tableSize = 1 << GHRLen;
    memset(table, 0, tableSize * sizeof(int));
    history = 0;
}

unsigned Gshare::get_idx(uint64_t pc)
{
    return (history ^ pc) & ((1 << GHRLen) - 1);
}
