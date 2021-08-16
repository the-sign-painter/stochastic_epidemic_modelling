Collection of Stochastic Epidemic Modelling programs written in C. This project is for fun by a bachelor of physics (so far out of study range) and so commits welcome.

So far:
- Reed Frost SIR
- Markovian SIR
- Markovian SIS

Targets:
- Create a proper Makefile
- Create deterministic model to compare against
- Create an SEIR (Markovian)
- Introduce birth and death rates to Markovian
- Introduce vaccines

Dependancies:
- gcc, for compiling c
- gmp, used for precise floats and large numbers
- gnuplot, used for plotting

Directions of use:
- go into Markovian or Reed-Frost directory 
- adjust the parameters in main function of main.c
- execute the ./run.sh to compile and run the program
- review the data in the 'data' file and the graphs produced

Disclaimer:
- Only developed on Arch Linux
