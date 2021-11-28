#include "bp_candidates.h"
#include <cstring>
#include <stdlib.h>

NestLoop::NestLoop()
{
    reset();
}

NestLoop::NestLoop(const NestLoop& other)
{
    loop_counts = other.loop_counts;
    cur_counts = other.cur_counts;
    local_pc = other.local_pc;
    total_pred = other.total_pred;
    correct_pred = other.correct_pred;
    last_pred = other.last_pred;
    count_ready = other.count_ready;
}

NestLoop::~NestLoop()
{
}

Prediction NestLoop::predict(uint64_t pc)
{
    // Increment total prediction count
    total_pred++;

    // If it is a new branch, reset the counters and
    // update the pc, also take the loop
    if(pc != local_pc){
        local_pc = pc;
        cur_counts = 0;
        loop_counts = 0;
        last_pred = true;
        count_ready = false;
        return Prediction(1, 1);
    }

    // If loop count is ready, we use it to predict when
    // to exit the loop
    if(count_ready){
        if(cur_counts < loop_counts){
            cur_counts++;
            last_pred = true;
            return Prediction(1, 1);
        } else {
            cur_counts = 0;
            last_pred = false;
            return Prediction(0, 1);
        }
    // If loop count is not ready, we take the loop
    } else {
        last_pred = true;
        return Prediction(1, 1);
    }
}

void NestLoop::update(uint64_t pc, bool taken)
{
    // We don't update on different pc
    if(local_pc != pc){
        return;
    }

    // If last prediction was correct, increment
    // correct prediction count
    if(taken == last_pred){
        correct_pred++;
    }

    // If loop count is not ready, we update the expected
    // number of iterations for the loop
    if(!count_ready){
        // If the loop was taken, increment loop_counts
        if(taken){
            loop_counts++;
        // If the loop was not taken, mark the loop count as ready
        } else {
            count_ready = true;
        }
    }

    // Calculate prediction accuracy and number of wrong 
    // predictions so far
    float acc = (float)correct_pred / (float)total_pred;
    uint32_t wrong_pred = total_pred - correct_pred; 

    // If loop count is ready and accuracy is low and it is not
    // because of small loops, reset the predictor and recount loop count
    if (count_ready && acc < 0.875 && wrong_pred > 7){
        total_pred = 0;
        correct_pred = 0;
        loop_counts = 0;
        cur_counts = 0;
        count_ready = false;
    }
}

void NestLoop::reset()
{
    total_pred = 0;
    correct_pred = 0;
    loop_counts = 0;
    cur_counts = 0;
    local_pc = 0;
    last_pred = true;
    count_ready = false;
}

std::string NestLoop::debug_info()
{
    std::stringstream ss;
    ss << "PC: " << std::hex << local_pc;
    ss << " loop count: " << std::dec << loop_counts;
    ss << std::endl;
    return ss.str();
}