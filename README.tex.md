# Game of Life in C

This project was part of the course Parallel and Distributed Programming taken at Uppsala University during 2019. Mind that the code was produced at an earlier stage in my education and that everything might not be best practice.

Game of life is a "cellular automata" in which special rules and initial conditions completely determine the outcome. The playing field is a grid of any size and every cell can be either "dead" or "alive". Each timestep, the cells change depending on the number of living cells they have adjacent. The rules are: 
\begin{enumerate}
    \item A new cell is born if it has exactly three neighbours
    \item A cell dies if it has more than three neighbours
    \item A cell dies if it has less than two neighbours
    \item For all others cases, the cells remain unchanged
\end{enumerate}

## Parallelisation

The program was parallelised with MPI and one can choose to run the code with or without graphics, with two different parallelisation methods.

When graphics is turned off, the starting matrix is first scattered into the different processes, evenly. The processes then calculate the next timestep for the local array. Thereafter the penultimate row at the top and bottom is sent to the process below and above in rank respectively, where it replaces the bottom and top row respectively, so the boundary is correct for the next timestep. This process is then repeated, but without any scatter, as the local arrays are all already updated. 

When graphics is turned on, the starting matrix is first scattered, but after the next timestep has been computed, the data in each process is gathered in the master process, in which the drawGraphics function is called to update the graphic interface. The loop is then repeated with the gathered matrix once again being scattered to the processes. Note that this type of execution is not as effective as running without graphics, and that running with graphics also involves additional function calls, this "with graphics-mode" was simply included for fun.

## Installation

Note: This has so far only been tried out on Windows with Ubuntu Terminal.

Download and compile the code:
```
cd <WORKING-DIRECTORY>
 
git clone https://github.com/LoweLundin/Game-of-Life-in-C

cd <WORKING-DIRECTORY>/Game-of-Life-in-C/game_of_life

make
```

## Usage

If you want graphics, make sure you have an X server running, Xming has been used in development.
```bash
To run, enter: 
mpirun -np (-n on some systems) [desired number of threads] ./gol [side length of grid] [number of timesteps] [waitTime (typically 0)] [boolean graphics on/off]

Example: 
mpirun -np 4 ./gol 512 500 0 1
```

Note that:
\begin{itemize}
\item The program is runnable for any number of threads (except 0 and 1 if graphics is turned off), but the program can only be expected to run faster up until the number of threads is twice that of the number of cores.  
\item Side length should be evenly divisible by number of threads and the graphics will start looking strange or stop displaying any pixels when the side length exceeds windowWidth or windowHeight.
\item Large enough grid sizes might crash the execution, as the memory needed to store them exceeds the working memory.
\item waitTime is mainly used for smaller grids to slow down execution, when trying to push numbers, always keep it at 0.
\item Running with graphics on means longer running times and should not be used when trying to push numbers.
\end{itemize}



