import subprocess
import re
import os
import sys

MAKE="make"
NUM_PATTERN=re.compile("\d+[.]\d+%")
TRACE_DIR="./ChampSim/dpc3_traces"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Missing branch predictor name")
        exit(1)

    predictor = sys.argv[1]
    trymake = subprocess.run([MAKE, predictor])
    if (trymake.returncode != 0):
        exit(1)
    outf = open(f"{predictor}_branch.csv", "w")
    outf.write(f"Benchmark,Single Branch Accuracy,Miss Contrubution\n")
    for trace in os.listdir(TRACE_DIR):
        if not trace.endswith(".xz"):
            continue
        cmd = [MAKE, f"run_{predictor}", f"TRACE_ID={trace}"]
        subprocess.run(cmd)
        cmd = [MAKE, "analyze"]
        output = subprocess.run(cmd, stdout=subprocess.PIPE)
        outstr = "".join([chr(value) for value in output.stdout])
        stats = re.findall(NUM_PATTERN, outstr)
        print(stats)
        singleAccuracy = stats[-1]
        branchPercentage = stats[1:-1]
        outf.write(f"{trace},{singleAccuracy}")
        for p in branchPercentage:
            outf.write(f",{p}")
        outf.write("\n")
    outf.close()
