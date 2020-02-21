# Game of Life in C

This project was part of the course Parallel and Distributed Programming taken at Uppsala University during 2019. Mind that the code was produced at an earlier stage in my education and that everything might not be best practice.

Game of life is a "cellular automata" in which special rules and initial conditions completely determine the outcome. The playing field is a grid of any size and every cell can be either "dead" or "alive". Each timestep, the cells change depending on the number of living cells they have adjacent. The rules are: 
\begin{enumerate}
    \item A new cell is born if it has exactly three neighbours
    \item A cell dies if it has more than three neighbours
    \item A cell dies if it has less than two neighbours
    \item For all others cases, the cells remain unchanged
\end{enumerate}

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
\begin{itemize}
\item The program is runnable for any number of threads (except 0 and 1 if graphics is turned off), but the program can only be expected to run faster up until the number of threads is twice that of the number of cores.  
\item Side length should be evenly divisible by number of threads and the graphics will start looking strange or stop displaying any pixels when the side length exceeds windowWidth or windowHeight
\item waittime is mainly used for smaller grids to slow down execution, when trying to push numbers, always keep it at 0
\item Running with graphics on means longer running times and should not be used when trying to push numbers.
\end{itemize}



