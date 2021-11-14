all:
	g++ test/*.cpp src/*.cpp -o run_test

.PHONY: clean
clean:
	rm run_test