#ifndef __TESTBENCH_H_
#define __TESTBENCH_H_

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdint.h>
#include "../src/Predictor.hpp"

class Testbench
{
private:
    std::ifstream instrFile;
    std::ifstream pcFile;
    std::ifstream takenFile;

    uint32_t hex2uint(std::string hexString);

public:
    Testbench(/* args */);
    ~Testbench();
    bool load_trace(std::string traceDir);
    double evaluate(Predictor* predictor);
};

#endif
