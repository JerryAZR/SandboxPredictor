# Sandbox Predictor

## Updates

### 11.21

* Added gshare predictor in [gshare.cc](ChampSim/branch/bp_candidates/gshare.cc)
* Added reset logic in [sandbox.bpred](ChampSim/branch/sandbox.bpred)
* **TODO:** Experiment with more reset & update policies, including both local
and global ones
* **TODO:** Analyze (mis)prediction patterns and try to find the bottlenecks

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
* **TODO:** Implement one or more simple predictors (e.g. 1-bit bimodal)
* **TODO:** Implement one or more complex predictors
(e.g. [perceptron](ChampSim/branch/bp_candidates/perceptron.cc))
* **TODO:** Implement the [sandbox predictor](ChampSim/branch/sandbox.bpred)
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
