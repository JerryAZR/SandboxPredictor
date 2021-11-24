#!/usr/bin/env python3

import sys
import numpy as np

def parse_line(line):
    line_array = line.split(",")
    ghist = int(line_array[0], 2)
    lhist = int(line_array[1], 2)
    pred = bool(int(line_array[2]))
    fact = bool(int(line_array[3]))
    return ghist, lhist, pred, fact

class Bimodal:
    def __init__(self, ncounters=16, nbits=2) -> None:
        self.counters = np.zeros(ncounters)
        self.counterMax = (1 << (nbits - 1)) - 1
        self.counterMin = 0 - (1 << (nbits - 1))
        self.history = 0
        self.mask = (1 << ncounters.bit_length()) - 1
        

    def get_idx(self, ghist):
        ncounters = self.counters.size
        # ghlen = ((ncounters.bit_length() - 1) >> 1)
        ghlen = 0
        ghmask = (1 << ghlen) - 1
        idx = (self.history << ghlen) | (ghist & ghmask)
        idx = idx % ncounters
        return idx

    def predict(self, ghist):
        idx = self.get_idx(ghist)
        return self.counters[idx] >= 0

    def update(self, fact, ghist):
        idx = self.get_idx(ghist)
        if (fact): self.counters[idx] += 1
        else: self.counters[idx] -= 1

        self.history = (self.history << 1 | fact) & self.mask


class Perceptron:
    def __init__(self, lhistlen=8, ghistlen=8) -> None:
        self.nweights = ghistlen + lhistlen
        self.gweights = np.zeros(ghistlen, dtype=int)
        self.lweights = np.zeros(lhistlen, dtype=int)
        self.bias = 0
        self.history = 0
        self.threshold = np.ceil(1.93 * self.nweights + 14)
        self.sum = 0

    def predict(self, ghist) -> bool:
        self.sum = self.bias
        lhistlen = self.lweights.size
        ghistlen = self.gweights.size
        for i in range(lhistlen):
            if (((self.history >> i) & 1) == 1): self.sum += self.lweights[i]
            else: self.sum -= self.lweights[i]
        for i in range(ghistlen):
            if (((ghist >> i) & 1) == 1): self.sum += self.gweights[i]
            else: self.sum -= self.gweights[i]
        return self.sum >= 0

    def update(self, fact, ghist):
        pred = self.sum >= 0
        training = abs(self.sum) < self.threshold
        lhistlen = self.lweights.size
        ghistlen = self.gweights.size

        if (pred != fact or training):
            if (fact): self.bias += 1
            else: self.bias -= 1
            for i in range(lhistlen):
                if ((self.history >> i) & 1) == fact: self.lweights[i] += 1
                else: self.lweights[i] -= 1
            for i in range(ghistlen):
                if ((ghist >> i) & 1) == fact: self.gweights[i] += 1
                else: self.gweights[i] -= 1
        self.history = (self.history << 1 | fact) & ((1 << lhistlen) - 1)
    

if __name__ == "__main__":
    if (len(sys.argv) < 2):
        print("Missing branch log file")
    bp_log = open(sys.argv[1])
    bp_log.readline() # Skip the header

    mispred = 0
    total = 0

    entry = bp_log.readline().strip()
    bp = Perceptron(16, 16)
    while (entry != ""):
        ghist, _, _, fact = parse_line(entry)
        pred = bp.predict(ghist)
        bp.update(fact, ghist)
        if (pred != fact):
            mispred += 1

        total += 1
        entry = bp_log.readline().strip()

    acc = (total - mispred) / total
    print("Misprediction: {}/{}. Accuracy: {:.2%}".format(mispred, total, acc))
