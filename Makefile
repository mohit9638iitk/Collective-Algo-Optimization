SHELL := bash
all: compile execute 

compile:
	@echo "Compiling the source code src.c"
	mpicc src.c -o src 
	
execute:
	@echo "---------------------------------------------"
	@echo "Running the program for all the configurations"
	./execute_config.sh
	
plot: 
	@echo "Plotting the graphs"
	python3 plot.py


