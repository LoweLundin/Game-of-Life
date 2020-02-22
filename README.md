# Game of Life in C

This project was part of the course Parallel and Distributed Programming taken at Uppsala University during 2019. Mind that the code was produced at an earlier stage in my education and that everything might not be best practice.

Game of life is a "cellular automata" in which special rules and initial conditions completely determine the outcome. The playing field is a grid of any size and every cell can be either "dead" or "alive". Each timestep, the cells change depending on the number of living cells they have adjacent. The rules are: 
<p align="center"><img src="/tex/2652a4cbf53e98daa4bc6264cec685a2.svg?invert_in_darkmode&sanitize=true" align=middle width=394.47589335pt height=113.24201624999999pt/></p>



## Parallelisation

The program was parallelised with MPI and one can choose to run the code with or without graphics, with two different parallelisation methods,

When graphics is turned off, the starting matrix is first scattered into the different processes, evenly. The processes then calculate the next timestep for the local array. Thereafter the penultimate row at the top and bottom is sent to the process below and above in rank respectively, where it replaces the bottom and top row respectively, so the boundary is correct for the next timestep. This process is then repeated, but without any scatter, as the local arrays are all already updated. 

When graphics is turned on, the starting matrix is first scattered, but after the next timestep has been computed, the data in each process is gathered in the master process, in which the drawGraphics function is called to update the graphic interface. The loop is then repeated with the gathered matrix once again being scattered to the processes. Note that this type of execution is not as effective as running without graphics, and that running with graphics also involves additional function calls, this "with graphics-mode" was simply included for fun.

## Installation

Note: This has so far only been tried out on Windows with Ubuntu Terminal.

Download and compile the code
```
<img src="/tex/bd4629dfa85a459e2c053a1984f31ec8.svg?invert_in_darkmode&sanitize=true" align=middle width=281.90691869999995pt height=22.831056599999986pt/> git clone https://github.com/LoweLundin/Game-of-Life-in-C

<img src="/tex/80ad2bf4299100c69f840e1d7f4a7558.svg?invert_in_darkmode&sanitize=true" align=middle width=499.32025769999996pt height=24.65753399999998pt/> make
```

## Usage

If you want graphics, make sure you have an X server running, Xming has been used in development.
```
To run enter: 
mpirun -np (-n on some systems) [desired number of threads] ./gol [side length of grid] [number of timesteps] [waittime (typically 0)] [boolean graphics on/off]

Example: 
$ mpirun -np 4 ./gol 512 500 0 1
```

Note that:
<p align="center"><img src="/tex/579c74e8fd04f35083a0eed7500e531c.svg?invert_in_darkmode&sanitize=true" align=middle width=675.8451941999999pt height=232.51141979999997pt/></p>



