#include "../ChampSim/inc/miss_cache.h"
#include "../ChampSim/branch/mcache_candidates/lru.cc"
#include <cstring>
#include <sstream>

uint64_t hex2int(std::string hex) {
    uint64_t x;   
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> x;
    return x;
}

int main() {
    MissCache* dut = new LRUMCache();
    while (1)
    {
        std::string userInput;
        dut->debug_print();
        std::cout << "Next Access: ";
        std::cin >> userInput;
        int idx;
        if (userInput.compare("snapshot") == 0 || userInput.compare("s") == 0) {
            std::cout << "Snapshot created" << std::endl;
            dut->snapshot();
        } else {
            idx = dut->access(hex2int(userInput));
            std::cout << "Found at idx: " << idx << std::endl << std::endl;
        }
    }
}
