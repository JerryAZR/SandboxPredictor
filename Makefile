L1I_PRE=no
L1D_PRE=no
L2_PRE=no
LLC_PRE=no
LLC_REP=lru

N_WARM=1
N_SIM=10
TRACE_ID=$(shell ls ChampSim/dpc3_traces/ | head -1)
TRACE=$(shell ls ChampSim/dpc3_traces/ | grep $(TRACE_ID))

.phony: all clean

all:
	echo "Please refer to the README.md in ChampSim/ for detailed instructions"

sandbox perceptron static bimodal gshare bplog tage:
	cd ChampSim && \
	bash build_champsim.sh \
	$@ $(L1I_PRE) $(L1D_PRE) $(L2_PRE) $(LLC_PRE) $(LLC_REP) 1

run_sandbox run_perceptron run_static run_bimodal run_gshare run_bplog run_tage:
	cd ChampSim && \
	bash run_champsim.sh $(subst run_,,$@)-$(L1I_PRE)-$(L1D_PRE)-$(L2_PRE)-\
	$(LLC_PRE)-$(LLC_REP)-1core $(N_WARM) $(N_SIM) $(TRACE)
	grep "Accuracy\|IPC\|branch predictor" ChampSim/results_$(N_SIM)M/$(TRACE)-\
	$(subst run_,,$@)-$(L1I_PRE)-$(L1D_PRE)-$(L2_PRE)-$(LLC_PRE)-$(LLC_REP)-\
	1core.txt

analyze:
	cd ChampSim && python3 analyze.py branch.csv

clean:
	cd ChampSim && make clean
