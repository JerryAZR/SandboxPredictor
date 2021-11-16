L1I_PRE=no
L1D_PRE=no
L2_PRE=no
LLC_PRE=no
LLC_REP=lru

N_WARM=1
N_SIM=10
TRACE=600.perlbench_s-210B.champsimtrace.xz

.phony: all clean

all:
	echo "Please refer to the README.md in ChampSim/ for detailed instructions"

sandbox:
	cd ChampSim && \
	bash build_champsim.sh \
	sandbox $(L1I_PRE) $(L1D_PRE) $(L2_PRE) $(LLC_PRE) $(LLC_REP) 1

run_sandbox:
	cd ChampSim && \
	bash run_champsim.sh \
	sandbox-$(L1I_PRE)-$(L1D_PRE)-$(L2_PRE)-$(LLC_PRE)-$(LLC_REP)-1core \
	$(N_WARM) $(N_SIM) $(TRACE)
	tail -13 ChampSim/results_$(N_SIM)M/$(TRACE)-\
	sandbox-$(L1I_PRE)-$(L1D_PRE)-$(L2_PRE)-$(LLC_PRE)-$(LLC_REP)-1core.txt

clean:
	cd ChampSim && make clean
