#include "miss_cache.h"
#include <stdint.h>


Segment::Segment(unsigned nEntries) {
    resize(nEntries);
    reset();
}

Segment::~Segment() {
    if (entries) {
        free(entries);
        entries = NULL;
    }
}

/**
 * @brief Check if this segment contains the given pc
 * 
 * @param pc the pc to look for
 * @return true if the pc is found in a valid entry
 * @return false otherwise
 */
bool Segment::contains(uint64_t pc){
    for (size_t i = 0; i < numEntries; i++) {
        if (entries[i].valid && entries[i].pc == pc) {
            return true;
        }
    }
    return false;
}

void Segment::reset() {
    for (size_t i = 0; i < numEntries; i++) {
        entries[i].age = i;
        entries[i].pc = 0;
        entries[i].valid = false;
    }
    lru = numEntries - 1;
}

void Segment::resize(unsigned numEntries) {
    this->numEntries = numEntries;
    if (entries) free(entries);
    entries = (lru_entry_t*) malloc(numEntries * sizeof(lru_entry_t));
}

/**
 * @brief set the given pc to MRU. Does nothing if not found
 * 
 * @param pc the MRU pc
 */
void Segment::bump(uint64_t pc) {
    unsigned pcAge = 0;
    unsigned pcIdx = 0;

    // Look for the given pc
    for (unsigned i = 0; i < numEntries; i++) {
        if (entries[i].valid && entries[i].pc == pc) {
            pcIdx = i;
            pcAge = entries[i].age;
            break;
        }
    }

    // Nothing to do if pc is already MRU, or if pc does not exist
    if (pcAge) {
        // Increment age of younger entries
        for (unsigned i = 0; i < numEntries; i++) {
            if (entries[i].age < pcAge) {
                entries[i].age++;
                // Update lru if necessary
                if (entries[i].age == numEntries - 1) {
                    lru = i;
                }
            }
        }
        entries[pcIdx].age = 0;
    }
}

/**
 * @brief Add a pc to this segment. Also mark it as MRU
 * Note that this function does not check if pc already exists. It is the
 * caller's responsibility to check with contains(pc) before calling this function
 * 
 * @param pc the pc to add
 * @return uint64_t evicted pc, or 0 if nothing is evicted
 */
uint64_t Segment::add(uint64_t pc) {
    lru_entry_t evict = entries[lru];
    unsigned addIdx = lru;
    // Increment age of all other entries
    for (unsigned i = 0; i < numEntries; i++) {
        if (i != addIdx) {
            entries[i].age++;
            if (entries[i].age == numEntries - 1) {
                lru = i;
            }
        }
    }
    // Add pc to the previous LRU entry and set it to MRU
    entries[addIdx].age = 0;
    entries[addIdx].pc = pc;
    entries[addIdx].valid = true;

    // check if eviction is necessary
    return evict.valid ? evict.pc : 0;
}

/**
 * @brief Remove a pc from this segment
 * 
 * @param pc the pc to remove
 */
void Segment::remove(uint64_t pc) {
    unsigned removeIdx = lru;
    for (unsigned i = 0; i < numEntries; i++) {
        if (entries[i].valid && entries[i].pc == pc) {
            removeIdx = i;
            break;
        }
    }
    
    // Decrement age of entries that are older
    for (unsigned i = 0; i < numEntries; i++) {
        if (entries[i].age > entries[removeIdx].age) {
            entries[i].age--;
        }
    }

    // Clear the entry to be removed
    if (entries[removeIdx].pc == pc) {
        entries[removeIdx].age = numEntries - 1;
        entries[removeIdx].pc = 0;
        entries[removeIdx].valid = false;
        lru = removeIdx;
    }
}

/**
 * @brief Get all (up to threhold defined by the caller) PC's in this segment
 * Invalid PC's are added as 0 to minimize the change in index
 * 
 * @param pcs Output array, should be allocated by the caller
 * @param count caller-defined max. Default to -1 (unlimited)
 * @return unsigned number of PC's returned (including invalid ones)
 */
unsigned Segment::get_all(uint64_t* pcs, unsigned count) {
    unsigned i;
    for (i = 0; i < numEntries && i < count; i++) {
        pcs[i] = entries[i].valid ? entries[i].pc : 0;
    }
    return i;
}

std::string Segment::debug_info() {
    std::stringstream ss;
    for (unsigned i = 0; i < numEntries; i++) {
        ss << "PC: " << std::hex << entries[i].pc;
        ss << "; AGE: " << std::dec << entries[i].age;
        ss << "; VALID: " << std::dec << entries[i].valid << std::endl;
    }
    ss << std::endl;
    return ss.str();
}
