#include "bp_candidates.h"

LTAGE::LTAGE(TAGE* tageBP, unsigned loopTH)
: tageBP(tageBP), loopThreshold(loopTH)
{
    loopBP = new NestLoop();
}

LTAGE::~LTAGE() {
    delete loopBP;
}

Prediction LTAGE::predict(uint64_t pc) {
    Prediction tagePred = tageBP->predict(pc);
    Prediction loopPred = loopBP->predict(pc);
    return loopPred.confidence > loopThreshold ? loopPred : tagePred;
}

void LTAGE::update(uint64_t pc, bool taken) {
    tageBP->update(pc, taken);
    loopBP->update(pc, taken);
}

void LTAGE::reset() {
    tageBP->reset();
    loopBP->reset();
}

std::string LTAGE::debug_info() {
    std::stringstream ss;
    ss << "==== TAGE Predictor Info: ====" << std::endl;
    ss << tageBP->debug_info();
    ss << "==== Loop Predictor Info: ====" << std::endl;
    ss << loopBP->debug_info();
    ss << std::endl;
    return ss.str();
}

uint64_t LTAGE::sizeB() {
    return loopBP->sizeB() + tageBP->sizeB();
}
