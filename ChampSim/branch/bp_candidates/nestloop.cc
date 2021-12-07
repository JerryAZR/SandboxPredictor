#include "bp_candidates.h"
#include <cstring>
#include <stdlib.h>

NestLoop::NestLoop(unsigned counter_len, unsigned idx_len) : 
counter_len(counter_len), idx_len(idx_len)
{
    max_count = (1 << counter_len) - 1;
    num_entries = 1 << idx_len;
    max_confidence = 3;
    max_age = 255;
    assoc = 4;
    history = 0;

    content = (nestloop_entry_t*) malloc(num_entries * assoc * sizeof(nestloop_entry_t));

    reset();
}

NestLoop::NestLoop(const NestLoop& other)
{
    counter_len = other.counter_len;
    idx_len = other.idx_len;
    max_count = other.max_count;
    num_entries = other.num_entries;
    content = other.content;
    assoc = other.assoc;
    max_confidence = other.max_confidence;
    max_age = other.max_age;
    history = other.history;
}

NestLoop::~NestLoop()
{
    free(content);
}

void NestLoop::reset(){
    for (unsigned i = 0; i < assoc * num_entries; i++) {
        content[i].tag = 0;
        content[i].age = 0;
        content[i].confidence = 0;
        content[i].loop_counts = 0;
        content[i].cur_counts = 0;
        content[i].last_pred = true;
        content[i].count_ready = false;
    }
}

unsigned NestLoop::get_idx(uint64_t pc) {
    uint64_t mask1 = (1 << (idx_len + 2)) - 4;
    uint64_t mask2 = (1 << idx_len) - 1;

    return (pc & mask1) ^ (history & mask2);
    
    // return ((pc << (62 - idx_len)) >> (64 - idx_len)) ^ (history & mask2);
}

unsigned NestLoop::get_tag(uint64_t pc) {
    return pc >> (idx_len + 2);
}

int NestLoop::lookup(uint64_t pc){
    unsigned idx = get_idx(pc);
    unsigned targetTag = get_tag(pc);
    for(unsigned i = 0; i < assoc; i++){
        if (targetTag == content[idx + i].tag){
            return idx + i;  
        } 
    }
    return -1;
}

bool NestLoop::add(uint64_t pc){
    unsigned idx = get_idx(pc);
    unsigned newTag = get_tag(pc);
    
    for (unsigned i = 0; i < assoc; i++){
        if (content[idx + i].age == 0) {
            reset_entry(idx, i);
            content[idx + i].tag = newTag;
            return true;
        }
    }

    decay(idx);
    return false;
}

void NestLoop::reset_entry(unsigned idx, unsigned set)
{
    content[idx + set].confidence = 0;
    content[idx + set].loop_counts = 0;
    content[idx + set].cur_counts = 0;
    content[idx + set].tag = 0;
    content[idx + set].last_pred = true;
    content[idx + set].count_ready = false;
    content[idx + set].age = 255;
}

void NestLoop::decay(unsigned idx){
    for (unsigned i = 0; i < assoc; i++){
        content[idx + i].age = (content[idx + i].age > 0) ? 
                            content[idx + i].age - 1 : content[idx + i].age;
    }
}

Prediction NestLoop::predict(uint64_t pc)
{
    // If it is a new branch, reset the counters and
    // update the pc, also take the loop
    int entry = lookup(pc);
    if(entry == -1){
        return Prediction(true, 0);
    }

    // If loop count is ready, we use it to predict when
    // to exit the loop
    if(content[entry].count_ready && content[entry].age > 0){
        if(content[entry].cur_counts < content[entry].loop_counts){
            content[entry].cur_counts = (content[entry].cur_counts < max_count) ? 
                                content[entry].cur_counts + 1 : content[entry].cur_counts;
            content[entry].last_pred = true;
            return Prediction(true, content[entry].confidence);
        } else {
            content[entry].cur_counts = 0;
            content[entry].last_pred = false;
            return Prediction(false, content[entry].confidence);
        }
    // If loop count is not ready, we take the loop
    } else {
        content[entry].last_pred = true;
        return Prediction(true, 0);
    }
}

void NestLoop::update(uint64_t pc, bool taken)
{
    int entry = lookup(pc);
    history = (history << 1) & taken;
    if(entry == -1){
        if(add(pc)){
            entry = lookup(pc);
        } else {
            return;
        }
    }

    // If last prediction was correct, increment
    // correct prediction count
    if(taken == content[entry].last_pred){
        if(!taken){
            content[entry].confidence = (content[entry].confidence < max_confidence) ? 
                                    content[entry].confidence + 1 : content[entry].confidence;
        }
        content[entry].age = (content[entry].age > max_age) ? content[entry].age + 1 : content[entry].age;
    }

    // If loop count is not ready, we update the expected
    // number of iterations for the loop
    if(!content[entry].count_ready){
        // If the loop was taken, increment loop_counts
        if(taken){
            content[entry].loop_counts = (content[entry].loop_counts < max_count) ? 
                                    content[entry].loop_counts + 1 : content[entry].loop_counts;
        // If the loop was not taken, mark the loop count as ready
        } else {
            content[entry].count_ready = true;
        }
    }

    // If loop count is ready and accuracy is low and it is not
    // because of small loops, reset the predictor and recount loop count
    if (content[entry].count_ready && taken != content[entry].last_pred){
        content[entry].age = 0;
    }
}

uint64_t NestLoop::sizeB()
{
    uint64_t entry_size = 10 + counter_len * 2 + (62 - idx_len);
    return assoc * num_entries * entry_size / 8;
}

std::string NestLoop::debug_info()
{
    std::stringstream ss;
    for (unsigned i = 0; i < assoc * num_entries; i++) {
        ss << "TAG: " << std::hex << content[i].tag;
        ss << "; LOOP CNT: " << std::dec << content[i].loop_counts;
        ss << "; CUR CNT: " << std::dec << content[i].cur_counts;
        ss << "; AGE: " << std::dec << (uint64_t)content[i].age;
        ss << "; CONF: " << std::dec << (uint64_t)content[i].confidence;
        ss << "; PREV PRED: " << content[i].last_pred << std::endl;
    }
    ss << std::endl;
    return ss.str();
}
