#include "bp_candidates.h"
#include <cstring>
#include <stdlib.h>

NestLoop::NestLoop()
{
    reset();
}

NestLoop::~NestLoop()
{
}

Prediction NestLoop::predict(uint64_t pc)
{
    total_pred++;

    if(pc != local_pc){
        local_pc = pc;
        cur_counts = 0;
        loop_counts = 0;
        last_pred = true;
        return Prediction(1, 1);
    }

    if(loop_counts > 0){
        if(cur_counts < loop_counts){
            cur_counts++;
            last_pred = true;
            return Prediction(1, 1);
        } else {
            cur_counts = 0;
            last_pred = false;
            return Prediction(0, 1);
        }
    } else {
        last_pred = true;
        return Prediction(1, 1);
    }
}

void NestLoop::update(uint64_t pc, bool taken)
{
    
    if(local_pc != pc){
        return;
    }

    if(taken == last_pred){
        correct_pred++;
    }

    float acc = (float)correct_pred / (float)total_pred;

    if(loop_counts <= 0){
        if(taken){
            loop_counts--;
        } else {
            loop_counts = -loop_counts;
        }
    }

    if (acc < 0.9){
        total_pred = 0;
        correct_pred = 0;
        loop_counts = 0;
        cur_counts = 0;
    }

}

void NestLoop::reset()
{
    total_pred = 0;
    correct_pred = 0;
    loop_counts = 0;
    cur_counts = 0;
    local_pc = 0;
}

std::string NestLoop::debug_info()
{
    std::stringstream ss;
    ss << "PC: " << std::hex << local_pc;
    ss << " loop count: " << std::dec << loop_counts;
    ss << std::endl;
    return ss.str();
}