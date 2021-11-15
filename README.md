# Sandbox Predictor

## Updates

### 11.15

* Merged [evaluation](https://github.com/JerryAZR/SandboxPredictor/tree/evaluation) branch to [master](https://github.com/JerryAZR/SandboxPredictor/tree/master)
* The original master branch is moved to [backup](https://github.com/JerryAZR/SandboxPredictor/tree/backup) branch
* Added header file ([bp_candidates.h](ChampSim/inc/bp_candidates.h)) for predictors used by the sandbox predictor
* Implemented perceptron predictor in [perceptron.cc](ChampSim/branch/bp_candidates/perceptron.cc)
* Added skeleton for sandbox preidctor in [sandbox.bpred](ChampSim/branch/sandbox.bpred)
* Changed [static predictor](ChampSim/branch/static.bpred) to predict always-taken (because only the pc is available during branch prediction)

## Useful Links

[Progress Report](https://docs.google.com/document/d/1Kbm3lSivZxMg2NZwRxZXS4C-Lj2yRoAvK5f_yQeKkyU/edit?usp=sharing)

[Final Report](https://docs.google.com) (to be added)

## Evaluation

We will use [ChampSim](ChampSim) to further evaluate our branch predictors

More execution traces can be downloaded [here](https://www.dropbox.com/sh/hh09tt8myuz0jbp/AACAS5zMWHL7coVuS-RbpUksa?dl=0)

To use these traces, simply put the `.trace.xz` file under the [dpc3_traces](ChampSim/dpc3_traces) folder and run ChampSim accordingly. Detailed instruction can be found [here](ChampSim/README.md)

## TODO

* Implement one or more simple predictors (e.g. 1-bit bimodal)
* Implement one or more complex predictors (e.g. [perceptron](ChampSim/branch/bp_candidates/perceptron.cc))
* Implement the [sandbox predictor](ChampSim/branch/sandbox.bpred)
* (Optional) Write a run script to simplify miscellaneous tasks
* More to be added later
