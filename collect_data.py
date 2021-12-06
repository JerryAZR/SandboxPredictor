import subprocess
import re
import os
import sys

MAKE="make"
NUM_PATTERN=re.compile("\d+[.]\d+%?")
TRACE_DIR="./ChampSim/dpc3_traces"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Missing branch predictor name")
        exit(1)

    predictor = sys.argv[1]
    trymake = subprocess.run([MAKE, predictor])
    if (trymake.returncode != 0):
        exit(1)
    outf = open(f"{predictor}_data.csv", "w")
    outf.write(f"Benchmark,IPC,Accuracy\n")
    for trace in os.listdir(TRACE_DIR):
        if not trace.endswith(".xz"):
            continue
        cmd = [MAKE, "run_perceptron", f"TRACE_ID={trace}"]
        output = subprocess.run(cmd, stdout=subprocess.PIPE)
        outstr = "".join([chr(value) for value in output.stdout])
        stats = re.findall(NUM_PATTERN, outstr)
        ipc = stats[0]
        accuracy = stats[1]
        csvEntry = f"{trace}, {ipc}, {accuracy}"
        print(f"{trace}: IPC {ipc}; accuracy {accuracy}.")
        outf.write(f"{trace},{ipc},{accuracy}\n")
    outf.close()
