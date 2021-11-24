#!/usr/bin/env python3

from os import access
import sys
import numpy as np
import plotille as pl
from collections import Counter

def parse_line(line):
    line_array = line.split(",")
    id = int(line_array[0])
    pc = int(line_array[1], 16)
    pred = int(line_array[2])
    fact = int(line_array[3])
    ghist = int(line_array[4], 16)
    lhist = int(line_array[5], 16)
    return id, pc, pred, fact, ghist, lhist

if __name__ == "__main__":

    if (len(sys.argv) < 2):
        print("Missing branch log file")
    bp_log = open(sys.argv[1])
    bp_log.readline() # Skip the header

    tmp = np.empty(shape=0, dtype=int)

    line = bp_log.readline()
    while (line != ""):
        try:
            line = bp_log.readline().strip()
            id, pc, pred, fact, _, _ = parse_line(line)
            if (pred != fact):
                tmp = np.append(tmp, pc)
        except ValueError:
            break
    bp_log.close()

    cnt = Counter(tmp)
    total = len(tmp)
    fig = pl.Figure()
    fig.histogram(tmp)
    print(fig.show())
    print(f"Total mispredictions: {total}")
    print("Top 10 mispredicted branches:")
    for entry in cnt.most_common(10):
        pc = entry[0]
        num = entry[1]
        print("PC {:08x}: {:6d} ({:.2%})".format(pc, num, num / total))

    # Analyze the branch with the most mispredicitions
    target_pc = cnt.most_common(1)[0][0]
    num_mispred = cnt.most_common(1)[0][1]
    print("Analyzing branch at {:08x}".format(target_pc))
    # Reset the file pointer
    bp_log = open(sys.argv[1])
    bp_log.readline() # Skip the header

    # Prepare output file
    outfname = "branch{:08x}.csv".format(target_pc)
    out_log = open(outfname, "w")
    out_log.write("Global History,Local History,Prediction,Fact\n")

    # Because the number of mispredictions is known, we can simply declare an
    # array with enough space
    mispredctions = np.empty(shape=num_mispred, dtype=int)
    local_id = 0 # branch id (local to this branch).
    idx = 0  # index in the misprediction array

    line = bp_log.readline()
    while (line != ""):
        try:
            line = bp_log.readline().strip()
            id, pc, pred, fact, ghist, lhist = parse_line(line)
            if (pc == target_pc):
                if (pred != fact):
                    mispredctions[idx] = local_id
                    idx = idx + 1
                local_id = local_id + 1
                log_entry = "{:b},{:b},{},{}\n".format(ghist, lhist, pred, fact)
                out_log.write(log_entry)
        except ValueError:
            break
    bp_log.close()
    out_log.close()

    fig = pl.Figure()
    fig.histogram(mispredctions, bins=10)
    print(fig.show())
    accuracy = (local_id - idx) / local_id
    print("Accuracy of branch {:08x}: {:.2%}".format(target_pc, accuracy))
    print("Log of branch {:08x} written to {}".format(target_pc, outfname))
