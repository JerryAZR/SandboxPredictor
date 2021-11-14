#include <iostream>
#include "Testbench.hpp"
#include "../src/Predictor.hpp"
#include "../src/Static.hpp"

int main(int argc, char *argv[]) {
    Static bp;
    Testbench tb;

    if (argc < 2) {
        std::cout << "Missing command line argument" << std::endl;
        return 1;
    }

    tb.load_trace(argv[1]);
    tb.evaluate(&bp);
    return 0;
}
