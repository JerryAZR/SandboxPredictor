#include "miss_cache.h"
#include "stdlib.h"

FixedMCache::FixedMCache(uint64_t* pcs, unsigned n) : numEntries(n) {
    savedState = (uint64_t*) malloc(n * sizeof(uint64_t));
    for (unsigned i = 0; i < n; i++) {
        savedState[i] = pcs[i];
    }
}

FixedMCache::~FixedMCache() {
    free(savedState);
}

int FixedMCache::get_idx(uint64_t pc) {
    for (unsigned i = 0; i < numEntries; i++) {
        if (savedState[i] == pc) {
            return i;
        }
    }
    return -1;
}
