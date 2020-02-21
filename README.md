# Game of Life in C

This project was part of the course Parallel and Distributed Programming taken at Uppsala University during 2019. Mind that the code was produced at an earlier stage in my education and that everything might not be best practice.

Game of life is a "cellular automata" in which special rules and initial conditions completely determine the outcome. The playing field is a grid of any size and every cell can be either "dead" or "alive". Each timestep, the cells change depending on the number of living cells they have adjacent. The rules are: 
<p align="center"><img src="/tex/2652a4cbf53e98daa4bc6264cec685a2.svg?invert_in_darkmode&sanitize=true" align=middle width=394.47589335pt height=113.24201624999999pt/></p>

The rules were implemented and parallelised with MPI. One can also choose to run the code with or without graphics.

## Parallelisation

The code has been parallel

## Installation

Note: This has so far only been tried out on Windows with Ubuntu Terminal.
```bash
Clone into the repository: git clone https://github.com/LoweLundin/Game-of-Life-in-C

Open the download diretory in terminal

Enter command: make
```

## Usage

If you want graphics, make sure you have an X server running, Xming has been used in development.
```bash

To run enter: 

mpirun -np (-n on some systems) [desired number of threads] ./gol [side length of grid] [number of timesteps] [waittime (typically 0)] [boolean graphics on/off]

example: mpirun -np 4 ./gol 512 500 0 1
```

Note that:
<p align="center"><img src="/tex/579c74e8fd04f35083a0eed7500e531c.svg?invert_in_darkmode&sanitize=true" align=middle width=675.8451941999999pt height=232.51141979999997pt/></p>



