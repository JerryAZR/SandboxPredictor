#include "miss_cache.h"

SLRUMCache::SLRUMCache(unsigned nEntries) {
    protectedSeg = new Segment(nEntries);
    probationarySeg = new Segment(nEntries);
    numEntries = nEntries;
    savedState = (uint64_t*) malloc(nEntries * sizeof(uint64_t));
    reset();
}

SLRUMCache::~SLRUMCache() {
    if (protectedSeg) delete protectedSeg;
    if (probationarySeg) delete probationarySeg;
    if (savedState) free(savedState);
}

/**
 * @brief Perform a branch miss cache access. Insertion and eviction are handled
 * silently.
 * 
 * @param pc PC (address) of mispredicted branch
 * @return int Index of pc in cache saved state, or -1 if DNE
 * Can also be used as the index for assigned private branch predictor.
 */
int SLRUMCache::access(uint64_t pc) {
    uint64_t evicted;
    // If in protected segment, bump activity
    if (protectedSeg->contains(pc)) protectedSeg->bump(pc);
    // If in probationary segment, move to protected
    else if (probationarySeg->contains(pc)) {
        probationarySeg->remove(pc);
        evicted = protectedSeg->add(pc);
        // Add evicted pc (if exists) back to probationary segment
        if (evicted) probationarySeg->add(evicted);
    }
    // If not found in either segment, add to probationary
    else probationarySeg->add(pc);

    return get_idx(pc);
}

/**
 * @brief Get the index while keeping the internal states unchanged
 * 
 * @param pc target PC
 * @return int index, or -1 if not found
 */
int SLRUMCache::get_idx(uint64_t pc) {
    int retIdx = -1;
    for (unsigned i = 0; i < numEntries; i++) {
        if (savedState[i] == pc) {
            retIdx = i;
            break;
        }
    }
    return retIdx;
}

void SLRUMCache::reset() {
    if (protectedSeg) protectedSeg->reset();
    if (probationarySeg) probationarySeg->reset();
    for (unsigned i = 0; i < numEntries; i++) {
        savedState[i] = 0;
    }
}

/**
 * @brief Resize the tables. Previous states are not preserved. States are not
 * reset after resize (need to call explicitly)
 * 
 * @param numEntries The new size (Used by both protected and probationary segment)
 */
void SLRUMCache::resize(unsigned numEntries) {
    this->numEntries = numEntries;

    if (protectedSeg) protectedSeg->resize(numEntries);
    else protectedSeg = new Segment(numEntries);

    if (probationarySeg) probationarySeg->resize(numEntries);
    else probationarySeg = new Segment(numEntries);

    if (savedState) free(savedState);
    savedState = (uint64_t*) malloc(numEntries * sizeof(uint64_t));
}

/**
 * @brief Get all PC's in the protected segment
 * 
 * @param pcs 
 * @param count 
 * @return unsigned 
 */
unsigned SLRUMCache::get_all(uint64_t* pcs, unsigned count) {
    return protectedSeg->get_all(pcs, count);
}

void SLRUMCache::snapshot() {
    protectedSeg->get_all(savedState, numEntries);
}

std::string SLRUMCache::debug_info() {
    std::stringstream ss;
    ss << "-------mcache entry------" << std::endl;

    ss << "*** Protected Segment ***" << std::endl;
    ss << protectedSeg->debug_info();
    
    ss << "** Probationary Segment **" << std::endl;
    ss << probationarySeg->debug_info();

    ss << "-----mcache snapshot-----" << std::endl;
    for (unsigned i = 0; i < numEntries; i++) {
        ss << std::dec << i << ": " << std::hex << savedState[i] << std::endl;
    }
    ss << std::endl;
    return ss.str();
}