#include "bp_candidates.h"
#include <stdlib.h>
#include <sstream>

uint64_t fold(uint64_t x, unsigned length)
{
    uint64_t out = 0;
    uint64_t mask = (1 << length) - 1;
    while (x)
    {
        out = out ^ x;
        x = x >> length;
    }
    return out & mask;
}

TagTable::TagTable(unsigned idxLen, unsigned tagLen) :
idxLen(idxLen), tagLen(tagLen)
{
    nEntries = 1 << idxLen;
    content = (tage_entry_t*) malloc(nEntries * sizeof(tage_entry_t));
    counterMax = 1;
    counterMin = -1 * counterMax - 1;
    usefulMax = 3;
    hard_reset();
}

TagTable::~TagTable() {
    free(content);
}

uint64_t TagTable::get_idx(uint64_t pc, uint64_t history) {
    uint64_t rawIdx = (pc >> idxLen) ^ pc ^ fold(history, idxLen);
    uint64_t mask = (1 << idxLen) - 1;
    return rawIdx & mask;
}

uint64_t TagTable::get_tag(uint64_t pc, uint64_t history) {
    uint64_t rawTag = pc ^ fold(history, tagLen) ^ (2*fold(history, tagLen-1));
    uint64_t mask = (1 << tagLen) - 1;
    return rawTag & mask;
}

tage_entry_t TagTable::lookup(uint64_t pc, uint64_t history) {
    uint64_t idx = get_idx(pc, history);
    uint64_t targetTag = get_tag(pc, history);
    tage_entry_t tmpEntry = content[idx];
    if (targetTag == tmpEntry.tag) return tmpEntry;
    else return tage_entry_t(0, 0, -1); // useful = -1 indicates not found
}

bool TagTable::add(uint64_t pc, uint64_t history) {
    uint64_t idx = get_idx(pc, history);
    uint64_t newTag = get_tag(pc, history);
    if (content[idx].useful == 0) {
        content[idx].counter = 0;
        content[idx].tag = newTag;
        return true;
    } else {
        return false;
    }
}

void TagTable::update(uint64_t pc, uint64_t history, bool taken, int useful) {
    uint64_t idx = get_idx(pc, history);
    uint64_t targetTag = get_tag(pc, history);
    if (targetTag != content[idx].tag) return;
    if (taken && content[idx].counter < counterMax) content[idx].counter++;
    if (!taken && content[idx].counter > counterMin) content[idx].counter--;
    if (useful > 0 && content[idx].useful < usefulMax) content[idx].useful++;
    if (useful < 0 && content[idx].useful > 0) content[idx].useful--;
}

void TagTable::decay() {
    for (unsigned i = 0; i < nEntries; i++) {
        if (content[i].useful > 0) content[i].useful--;
    }
}

void TagTable::decay(uint64_t pc, uint64_t history) {
    uint64_t i = get_idx(pc, history);
    if (content[i].useful > 0) content[i].useful--;
}

void TagTable::soft_reset(unsigned mask) {
    for (unsigned i = 0; i < nEntries; i++) {
        content[i].useful &= mask;
    }
}

void TagTable::hard_reset() {
    for (unsigned i = 0; i < nEntries; i++) {
        content[i].counter = 0;
        content[i].tag = 0;
        content[i].useful = 0;
    }
}

std::string TagTable::debug_info() {
    std::stringstream ss;
    for (unsigned i = 0; i < nEntries; i++) {
        ss << "TAG: " << std::hex << content[i].tag;
        ss << "; CNT: " << std::dec << content[i].counter;
        ss << "; USEFUL: " << std::dec << content[i].useful << std::endl;
    }
    ss << std::endl;
    return ss.str();
}
