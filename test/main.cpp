#include <iostream>
#include "Testbench.hpp"
#include "../src/Predictor.hpp"

int main(int argc, char *argv[]) {
    Predictor basicBP;
    Testbench tb;

    if (argc < 2) {
        std::cout << "Missing command line argument" << std::endl;
        return 1;
    }

    tb.load_trace(argv[1]);
    tb.evaluate(&basicBP);
    return 0;
}
