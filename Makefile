all:
	g++ test/*.cpp src/*.cpp -o testbench

.PHONY clean
clean:
	rm testbench