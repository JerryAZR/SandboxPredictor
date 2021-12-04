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
    virtual uint64_t sizeB() {return 0;}
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
    virtual ~LRUMCache();

    int access(uint64_t pc);
    int get_idx(uint64_t pc);
    void reset();
    void resize(unsigned numEntries);
    unsigned get_all(uint64_t* pcs, unsigned count = -1);
    void snapshot();
    std::string debug_info();
    uint64_t sizeB() {return numEntries * 134 / 8;}
};

typedef struct nru_entry_t
{
    bool used;
    bool valid;
    uint64_t pc;
    unsigned age;

    bool operator<(const nru_entry_t& other) {return age < other.age;}
} nru_entry_t;

class NRUMCache : public MissCache
{
private:
    /* data */
    unsigned numEntries;
    nru_entry_t* entries;
    uint64_t* savedState;
    unsigned lru;
public:
    NRUMCache() : numEntries(0), entries(NULL), savedState(NULL) {}
    NRUMCache(unsigned nways);
    virtual ~NRUMCache();

    int access(uint64_t pc);
    int get_idx(uint64_t pc);
    void reset();
    void resize(unsigned numEntries);
    unsigned get_all(uint64_t* pcs, unsigned count = -1);
    void snapshot();
    std::string debug_info();
    uint64_t sizeB() {return numEntries * 130 / 8;}
};

class Segment
{
public:
    unsigned numEntries;
    lru_entry_t* entries;
    unsigned lru;

    Segment() : numEntries(0), entries(NULL) {}
    Segment(unsigned nEntries);
    virtual ~Segment();

    bool contains(uint64_t pc);
    void reset();
    void resize(unsigned numEntries);
    void bump(uint64_t pc);
    uint64_t add(uint64_t pc);
    void remove(uint64_t pc);
    unsigned get_all(uint64_t* pcs, unsigned count = -1);
    std::string debug_info();
    uint64_t sizeB() {return numEntries * 70 / 8;}
};

class SLRUMCache : public MissCache
{
private:
    unsigned numEntries;
    Segment* protectedSeg;
    Segment* probationarySeg;
    uint64_t* savedState;
public:
    SLRUMCache() : numEntries(0), protectedSeg(NULL), probationarySeg(NULL), savedState(NULL) {}
    SLRUMCache(unsigned nEntries);
    virtual ~SLRUMCache();

    int access(uint64_t pc);
    int get_idx(uint64_t pc);
    void reset();
    void resize(unsigned numEntries);
    unsigned get_all(uint64_t* pcs, unsigned count = -1);
    void snapshot();
    std::string debug_info();
    uint64_t sizeB() {
        return protectedSeg->sizeB() + probationarySeg->sizeB() + (numEntries * 8);
    }
};

class FixedMCache : public MissCache
{
private:
    unsigned numEntries;
    uint64_t* savedState;
public:
    FixedMCache(uint64_t* pcs, unsigned n);
    virtual ~FixedMCache();
    int get_idx(uint64_t pc);
    uint64_t sizeB() {return numEntries * 8;}
};

#endif
