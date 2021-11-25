#include "miss_cache.h"
#include <stdlib.h>

NRUMCache::NRUMCache(unsigned nEntries)
{
    resize(nEntries);
}

NRUMCache::~NRUMCache()
{
    free(entries);
    entries = NULL;
}

void NRUMCache::reset()
{
    for (unsigned i = 0; i < numEntries; i++)
    {
        entries[i].age = i;
        entries[i].pc = 0;
        entries[i].used = false;
        entries[i].valid = false;
        savedState[i] = 0;
    }
    lru = numEntries - 1;
}

void NRUMCache::resize(unsigned nEntries) {
    if (entries) free(entries);
    if(savedState) free(savedState);
    numEntries = nEntries;
    entries = (nru_entry_t*) malloc(nEntries * sizeof(nru_entry_t));
    savedState = (uint64_t*) malloc(nEntries * sizeof(uint64_t));
    reset();
}

/**
 * @brief Perform a branch miss cache access. Insertion and eviction are handled
 * silently.
 * 
 * @param pc PC (address) of mispredicted branch
 * @return int Index of pc in cache saved state, or -1 if DNE
 * Can also be used as the index for assigned private branch predictor.
 */
int NRUMCache::access(uint64_t pc)
{
    bool hit = false;
    unsigned pcIdx = lru; // default to lru
    unsigned pcAge = numEntries - 1; // default to lru

    // Find a unused entry if possible
    unsigned randBase = rand();
    for (unsigned j = 0; j < numEntries; j++){
        unsigned i = (j + randBase) % numEntries;
        if(!entries[i].used && entries[i].valid){
            pcIdx = i;
            break;
        }
    }

    // Find a new entry if possible
    for (unsigned i = 0; i < numEntries; i++){
        if(!entries[i].valid){
            pcIdx = i;
            break;
        }
    }

    // Cache hit has the highest priority
    for (unsigned i = 0; i < numEntries; i++)
    {
        if (entries[i].pc == pc && entries[i].valid)
        {
            pcIdx = i;
            pcAge = entries[i].age;
            hit = true;
            break;
        }
    }

    for (unsigned i = 0; i < numEntries; i++)
    {
        // increase age of all younger entries
        if (entries[i].age < pcAge && entries[i].valid) {
            entries[i].age++;
            // Update lru if necessary
            if (entries[i].age == numEntries - 1) {
                lru = i;
            }
        }
    }

    // Set current entry to most recently used
    entries[pcIdx].age = 0;
    entries[pcIdx].used = hit;
    entries[pcIdx].valid = true;
    entries[pcIdx].pc = pc; // silent eviction

    return get_idx(pc);
}

/**
 * @brief Get the index while keeping the internal states unchanged
 * 
 * @param pc target PC
 * @return int index, or -1 if not found
 */
int NRUMCache::get_idx(uint64_t pc) {
    // Search saved state for the accessed PC
    int retIdx = -1;
    for (unsigned i = 0; i < numEntries; i++) {
        if (savedState[i] == pc) {
            retIdx = i;
            break;
        }
    }
    return retIdx;
}

/**
 * @brief Get all (up to some threhold defined by the user) cached PC's
 * 
 * @param pc_array Output array containing an unsorted list of PC's
 * @param count Max number of PC's to return. Default -1 (return all)
 * @return int Actual number of PC's returned
 */
unsigned NRUMCache::get_all(uint64_t* pc_array, unsigned count)
{
    unsigned idx = 0;
    for (unsigned i = 0; i < numEntries; i++) {
        if (idx >= count) break;
        if (entries[i].age < count) {
            pc_array[idx++] = entries[i].pc; // Using post-increment
        }
    }
    return idx;
}

void NRUMCache::snapshot() {
    for (unsigned i = 0; i < numEntries; i++) {
        savedState[i] = entries[i].pc;
    }
    // Hardcoded values for testing purpose
    // savedState[0] = 0x004bedf9;
    // savedState[1] = 0x004be97a;
    // savedState[2] = 0x004be9b3;
    // savedState[3] = 0x004c5c06;
}

std::string NRUMCache::debug_info() {
    std::stringstream ss;
    for (unsigned i = 0; i < numEntries; i++) {
        ss << "PC: " << std::hex << entries[i].pc;
        ss << "; AGE: " << std::dec << entries[i].age;
        ss << "; USED: " << std::dec << entries[i].used;
        ss << "; VALID: " << std::dec << entries[i].valid << std::endl;
    }
    ss << "Current snapshot: " << std::endl;;
    for (unsigned i = 0; i < numEntries; i++) {
        ss << std::dec << i << ": " << std::hex << savedState[i] << std::endl;
    }
    ss << std::endl;
    return ss.str();
}
