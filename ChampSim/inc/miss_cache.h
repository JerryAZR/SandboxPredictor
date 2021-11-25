#ifndef __MISS_CACHE_H_
#define __MISS_CACHE_H_

#include <stdint.h>
#include <iostream>
#include <sstream>

/**
 * @brief Base class for branch misprediction cache
 * 
 */
class MissCache
{
private:
    /* data */
public:
    virtual int access(uint64_t pc) {return -1;}
    virtual int get_idx(uint64_t pc) {return -1;}
    virtual void reset() {}
    virtual void resize(unsigned numEntries) {}
    virtual unsigned get_all(uint64_t* pcs, unsigned count = -1) {return 0;}
    virtual void snapshot() {}
    virtual std::string debug_info() {
        std::stringstream ss;
        ss << "The base class has nothing to show you." << std::endl;
        return ss.str();
    }
    virtual void debug_print() {std::cout << debug_info();}
};

typedef struct lru_entry_t
{
    bool valid;
    uint64_t pc;
    unsigned age;

    bool operator<(const lru_entry_t& other) {return age < other.age;}
} lru_entry_t;


#define DEFAULT_LRU_NWAYS 8
/**
 * @brief Miss cache using LRU policy
 * 
 */
class LRUMCache : public MissCache
{
private:
    /* data */
    unsigned numEntries;
    lru_entry_t* entries;
    uint64_t* savedState;
    unsigned lru;
public:
    LRUMCache() : numEntries(0), entries(NULL), savedState(NULL) {}
    LRUMCache(unsigned nways);
    ~LRUMCache();

    int access(uint64_t pc);
    int get_idx(uint64_t pc);
    void reset();
    void resize(unsigned numEntries);
    unsigned get_all(uint64_t* pcs, unsigned count = -1);
    void snapshot();
    std::string debug_info();
};


#endif
