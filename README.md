# Game of Life

This project was produced for the course Parallel and Distributed Programming taken at Uppsala University during 2019. Mind that the code was produced at an earlier stage in my education and that everything might not be best practice.

Game of life is a "cellular automata" in which special rules and initial conditions completely determine the outcome. The playing field is a grid of any size and every cell can be either "dead" or "alive". Each timestep, the cells change depending on the number of living cells they have adjacent. The rules are: 
<p align="center"><img src="/tex/2652a4cbf53e98daa4bc6264cec685a2.svg?invert_in_darkmode&sanitize=true" align=middle width=394.47589335pt height=113.24201624999999pt/></p>

<p align="center">
  <img width="500" height="500" src=game_of_life.gif>

</p>
<p align="center">Run on 512 x 512 grid with a square on the boundary as initial condition and 0.05 waitTime. </p>


## Parallelisation

The program was parallelised with MPI and one can choose to run the code with or without graphics, with two different parallelisation methods.

When graphics is turned off, the starting matrix is first scattered into the different processes, evenly. The processes then calculate the next timestep for the local array. Thereafter the penultimate row at the top and bottom is sent to the process below and above in rank respectively, where it replaces the bottom and top row respectively, so the boundary is correct for the next timestep. This process is then repeated, but without any scatter, as the local arrays are all already updated. 

When graphics is turned on, the starting matrix is first scattered, but after the next timestep has been computed, the data in each process is gathered in the master process, in which the drawGraphics function is called to update the graphic interface. The loop is then repeated with the gathered matrix once again being scattered to the processes. Note that this type of execution is not as effective as running without graphics, and that running with graphics also involves additional function calls, this "with graphics-mode" was simply included for fun.

## Installation

Note: This has so far only been tried out on Windows with Ubuntu Terminal.

Download and compile the code:
```
$ cd <WORKING-DIRECTORY>
 
$ git clone https://github.com/LoweLundin/Game-of-Life

$ cd <WORKING-DIRECTORY>/Game-of-Life/game_of_life

$ make
```

## Usage

If you want graphics, make sure you have an X server running, Xming has been used in development.
```bash
To run, enter: 
$ mpirun -np (-n on some systems) [desired number of threads] ./gol [side length of grid] [initial condition to run (s/f/g/r)] [number of timesteps] [waitTime (typically 0)] [boolean graphics on/off]

Example: 
$ mpirun -np 4 ./gol 512 s 500 0 1
```

Note that:
<p align="center"><img src="/tex/fbc140709d82202f7635d83c57b2c642.svg?invert_in_darkmode&sanitize=true" align=middle width=675.8451941999999pt height=285.114159pt/></p>

## Related information

Project report hosted on [Dropbox](https://www.dropbox.com/s/yjhkwn9mc8skqxw/Game_of_Life.pdf?dl=0)

