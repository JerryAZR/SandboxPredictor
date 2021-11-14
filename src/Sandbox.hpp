#ifndef __SANDBOX_H_
#define __SANDBOX_H_

#include "Predictor.hpp"
#include "Instruction.hpp"

class Sandbox : public Predictor
{
private:
    Predictor* basicBP;
    Predictor* complexBP;
public:
    Sandbox(Predictor* basic, Predictor* complex)
    : basicBP(basic), complexBP(complex) {}

    Prediction predict(uint32_t pc, Instruction instr);
    void update(uint32_t pc, bool taken);
    void reset();
};



#endif
