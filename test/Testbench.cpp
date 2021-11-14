#include <stdint.h>
#include <string>
#include "Testbench.hpp"
#include "../src/Predictor.hpp"

Testbench::Testbench(/* args */)
{
}

Testbench::~Testbench()
{
    if (instrFile.is_open()) instrFile.close();
    if (pcFile.is_open()) pcFile.close();
    if (takenFile.is_open()) takenFile.clear();
}

uint32_t Testbench::hex2uint(std::string hexString)
{
    uint32_t x;   
    std::stringstream ss;
    ss << std::hex << hexString;
    ss >> x;
    return x;
}

bool Testbench::load_trace(std::string traceDir)
{
    instrFile.open(traceDir + "/instr.txt");
    if (!instrFile.is_open()) return false;
    pcFile.open(traceDir + "/pc.txt");
    if (!pcFile.is_open()) return false;
    takenFile.open(traceDir + "/taken.txt");
    if (!takenFile.is_open()) return false;
    return true;
}

double Testbench::evaluate(Predictor* predictor)
{
    unsigned totalCount = 0;
    unsigned correctCount = 0;
    std::string instrString;
    std::string pcString;
    std::string takenString;
    Instruction instr;
    uint32_t pc;
    bool taken;
    Prediction pred;
    double accuracy;

    std::getline(instrFile, instrString);
    std::getline(pcFile, pcString);
    std::getline(takenFile, takenString);

    predictor->reset();

    while (instrString.length() > 0)
    {
        instr = Instruction(hex2uint(instrString));
        pc = hex2uint(pcString);
        taken = (hex2uint(takenString) == 1);

        // Make prediction
        pred = predictor->predict(pc, instr);
        totalCount++;
        if (pred.taken == taken) correctCount++;

        // Update predictor state
        predictor->update(pc, taken);

        // Get lines for next iteration
        std::getline(instrFile, instrString);
        std::getline(pcFile, pcString);
        std::getline(takenFile, takenString);
    }
    
    accuracy = (double) correctCount / (double) totalCount;
    std::cout << "Summary (correct / total): ";
    std::cout << correctCount << " / " << totalCount << std::endl;
    std::cout << "Accuracy: " << accuracy * 100 << "%" << std::endl;
    return accuracy; 
}