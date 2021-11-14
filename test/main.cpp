#include <iostream>
#include "Testbench.hpp"
#include "../src/Predictor.hpp"
#include "../src/Static.hpp"
#include "../src/Perceptron.hpp"
#include "../src/Sandbox.hpp"

int main(int argc, char *argv[]) {
    Perceptron perceptronBP;
    Static staticBP;
    Sandbox bp(&staticBP, &perceptronBP);
    Testbench tb;

    if (argc < 2) {
        std::cout << "Missing command line argument" << std::endl;
        return 1;
    }

    tb.load_trace(argv[1]);
    tb.evaluate(&bp);
    return 0;
}
