# SandboxPredictor

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
./run_test ./trace/mp2
```

## TODO
1. Finalize helper classes (e.g. The [instruction](src/Instruction.hpp) class)
2. Collect more execution traces
3. Implement one or more simple predictors (e.g. forward-taken-backward-nt)
4. Implement one or more complex predictors (e.g. [perceptron](src/Perceptron.hpp))
5. Implement the sandbox predictor
6. (Optional) Write a run script to simplify miscellaneous tasks
7. More to be added later
