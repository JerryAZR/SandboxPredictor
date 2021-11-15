# Sandbox Predictor

To compile:  
```
make
```

To run tests:
```
./run_test path/to/trace/dir
```

For example:
```
./run_test ./test_trace
```

## Evaluation

We will use [ChampSim](ChampSim) to further evaluate our branch predictors

More execution traces can be downloaded [here](https://www.dropbox.com/sh/hh09tt8myuz0jbp/AACAS5zMWHL7coVuS-RbpUksa?dl=0)

To use these traces, simply put the `.trace.xz` file under the [dpc3_traces](ChampSim/dpc3_traces) folder and run ChampSim accordingly. Detailed instruction can be found [here](ChampSim/README.md)

## TODO
1. Finalize helper classes (e.g. The [instruction](src/Instruction.hpp) class)
2. Collect more execution traces
3. Implement one or more simple predictors (e.g. [forward-nt-backward-taken](src/Static.hpp))
4. Implement one or more complex predictors (e.g. [perceptron](src/Perceptron.hpp))
5. Implement the sandbox predictor
6. (Optional) Write a run script to simplify miscellaneous tasks
7. More to be added later
