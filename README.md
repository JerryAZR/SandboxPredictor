# Sandbox Predictor

## Updates

### 11.27

* Implemented Segment LRU miss cache in [nru.cc](ChampSim/branch/mcache_candidates/slru.cc)
* Changed private predictor type of VIP predictor to perceptron
* **TODO:** Try VIP predictor with LTAGE as default predictor

### 11.26

* Add NestLoop predictor in [nestloop.cc](ChampSim/branch/bp_candidates/nestloop.cc)
* Add debug info for predictors in [bp_candidates.h](ChampSim/inc/bp_candidates.h)
* Change mcache.log to debug_info.log and change debug info format

### 11.25

* Randomized NRU choice
* Implemented a simple NRU miss cache in [nru.cc](ChampSim/branch/mcache_candidates/nru.cc)
* Deployed NRU miss cache in [bplog.bpred](ChampSim/branch/bplog.bpred)

### 11.24

* Added miss cache in [miss_cache.h](ChampSim/inc/miss_cache.h)
* Implemented a simple LRU miss cache in [lru.cc](ChampSim/branch/mcache_candidates/lru.cc)
* Implemented a simple cache-based predictor in [vip.cc](ChampSim/branch/bp_candidates/vip.cc)
* **TODO:** Try different cache insertion & eviction policies

### 11.23

* Moved original sandbox predictor to class `Tournament` in
[tournament.cc](ChampSim/branch/bp_candidates/tournament.cc)
* Moved log generation from [sandbox.bpred](ChampSim/branch/sandbox.bpred) to
[bplog.bpred](ChampSim/branch/bplog.bpred). The original sandbox predictor no
longer generates branch prediction log.
* Removed branch.csv from git tree because the file becomes too large after
including global and local history,

### 11.22

* Modified [sandbox.bpred](ChampSim/branch/sandbox.bpred) to write branch
prediction log to [branch.csv](ChampSim/branch.csv)
* Added python script for branch prediction analysis ([analyze.py](ChampSim/analyze.py))
* **NOTE:** The python script depends on plotille, which is a command-line
alternative to matplotlib and can be installed using `pip`.
* **TODO:** I've noticed that the majority of mispredictions happen on a small
group of branches. Maybe we should give these "hard-to-predict" branches some
special treatment? (For example, use both local and global history and/or use an
actual neural network instead of a single perceptron for each one of them)

### 11.21

* Added gshare predictor in [gshare.cc](ChampSim/branch/bp_candidates/gshare.cc)
* Added reset logic in [sandbox.bpred](ChampSim/branch/sandbox.bpred)
* **TODO:** Experiment with more reset & update policies, including both local
and global ones

### 11.15

* Merged [evaluation](https://github.com/JerryAZR/SandboxPredictor/tree/evaluation)
branch to [master](https://github.com/JerryAZR/SandboxPredictor/tree/master)
* The original master branch is moved to
[backup](https://github.com/JerryAZR/SandboxPredictor/tree/backup) branch
* Added header file ([bp_candidates.h](ChampSim/inc/bp_candidates.h))for
predictors used by the sandbox predictor
* Implemented perceptron predictor in
[perceptron.cc](ChampSim/branch/bp_candidates/perceptron.cc)
* Added skeleton for sandbox preidctor in
[sandbox.bpred](ChampSim/branch/sandbox.bpred)
* Changed [static predictor](ChampSim/branch/static.bpred) to predict
always-taken (because only the pc is available during branch prediction)
* **TODO:** (Optional) Write a run script to simplify miscellaneous tasks

## Useful Links

[Progress Report](https://docs.google.com/document/d/1Kbm3lSivZxMg2NZwRxZXS4C-Lj2yRoAvK5f_yQeKkyU/edit?usp=sharing)

[Final Report](https://docs.google.com) (to be added)

## Evaluation

We will use [ChampSim](ChampSim) to further evaluate our branch predictors

More execution traces can be downloaded [here](https://www.dropbox.com/sh/hh09tt8myuz0jbp/AACAS5zMWHL7coVuS-RbpUksa?dl=0)

To use these traces, simply put the `.trace.xz` file under the [dpc3_traces](ChampSim/dpc3_traces) folder and run ChampSim accordingly. Detailed instruction can be found [here](ChampSim/README.md)

To compile and run tests using the default parameters, execute `make ${PREDICTOR}`
and `make run_${PREDICTOR}` at the [root directory](./). For example:
```
make sandbox
make run_sandbox
```
To run simulation with a selected trace in the default trace directory ([dpc3_traces](ChampSim/dpc3_traces)), run `make run_${PREDICTOR} TRACD_ID=${ID}`.
For example:
```
make run_sandbox TRACE_ID=600
```
would start the simulation using the trace file `600.perlbench_s-210B.champsimtrace.xz` if it is in the default trace directory.
