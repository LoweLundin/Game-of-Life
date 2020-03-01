#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include <math.h>
#include <sys/time.h>
#include <mpi.h>

/*
In Game Of Life, every cell on the grid is either alive or dead, the rules are:
1. A new cell is born if it has exactly three neighbours
2. A cell dies if it has more than three neighbours
3. A cell dies if it has less than two neighbours
4. For all others cases, the cells remain unchanged

Game Of Life is deterministic as all future states depend on the initial state.
*/

/*
When testing out parameters to run with graphics, mind that smaller grids will be much quicker to compute
and it is therefore advisable to run with a small waitTime,  e.g. 0.5. 
*/

char *Step(char *boardState, int N, int rows);
void square(int N);
void floater(int N);
void gosper_glider_gun(int N);
void drawGraphics(int N, char * boardState, float waitTime);
void print_matrix(char* A, int n, int rows);

const float Color = 0;
const int windowWidth = 600, windowHeight = 600, W = 1, H = 1;
char *boardState, *newBoardState;

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("Wrong number of input arguments\n");
        printf("To run, enter 'mpirun -np (-n on some systems) [number of threads] ./gol [side length of square] [initial state (s/f/g)] [timesteps] [waittime (typically 0)] [boolean graphics on/off]'\n");
        return -1;
    }
    int N = atoi(argv[1]);        // The size of one side of the generated matrix.
    char type_of_matrix = argv[2][0]; // The type of initial states.
    int nsteps = atoi(argv[3]);   // The number of iterations.
    double waitTime = atof(argv[4]);    // The waitTime, if one would like to see the graphics slower.
    int graphics = atoi(argv[5]); // If this is 1, there will be graphics for each timestep, otherwise only the end-state will be shown.
    
    int  rows;
    int size, rank;
    int i, j;
    char *local;
    double starttime, time;

    boardState = (char*) calloc((N+2) * (N+2), sizeof(char));

    waitTime = waitTime * 1000000; //As usleep is used

    /*
    Generating the initial condition. Feel free to play around with and create own intitial conditions. https://www.conwaylife.com/wiki/Main_Page can be used for inspiration.
    */

    switch (type_of_matrix)
    {
    case 's': square(N);
        break;

    case 'f': floater(N);
        break;

    case 'g':
        if(N<38)
        {
            printf("Too small grid, running square instead\n");
            square(N);
        }
        else
        {
            gosper_glider_gun(N);
        }
        break;

    default:
        printf("Unrecognised command, defaulting to square\n");
        square(N);
    }

    MPI_Request request;
    MPI_Status  status;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);       

    /* Global IDs */
    MPI_Comm_size(MPI_COMM_WORLD, &size);     
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    
    
    if(rank == 0 )
    {
        // Only runs for even splits
        if(N % size!=0)
        {
            printf("The number of rows must be evenly divisible with the number of threads\n");
            return -1;
        }
        else
        {
            rows = N/size;
        }
        // Initializes graphics and runs one step to show the starting state
        if(graphics == 1)
        {
            InitializeGraphics("gameOfLife", windowWidth, windowHeight);
            SetCAxes(0, 1);
            drawGraphics(N, boardState, waitTime);
        }  
    }

    // Broadcasting the number of rows each process is going to take
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    local  = (char*) calloc((N+2) * (rows+2), sizeof(char));

    // Generating the arrays for number of elements to receive and displacement
    int counts[size];
    int sendback[size];
    int displs[size];
    int displsBack[size];

    for (i=0; i<size; i++)
    {
        counts[i] = (N+2)*(rows+2);
        sendback[i] = (N+2)*(rows);
        displs[i] = i*(N+2)*(rows);
        displsBack[i] = i*(N+2)*(rows) + (N+2);
    }

    starttime = MPI_Wtime();

    /*
    If graphics is turned on, the starting board is first scattered to the local matrices,
    the next step is computed and all the data is gathered back into the master process,
    in order for it to be able to run drawGraphics. The process is then repeated.
    */
    if(graphics == 1)
    {
    for (i = 0; i < nsteps; i++)
    { 
        MPI_Scatterv(boardState, counts, displs, MPI_CHAR, local, (N+2)*(rows+2), MPI_CHAR, 0, MPI_COMM_WORLD);        
        local = Step(local, N, rows);
        MPI_Gatherv(local+(N+2), (N+2)*(rows), MPI_CHAR, boardState, sendback, displsBack, MPI_CHAR, 0, MPI_COMM_WORLD);
        if(rank==0)
        {
            drawGraphics(N, boardState, waitTime);
        }
    }
    
    time = MPI_Wtime() - starttime;
    if(rank == 0 )
    {
        printf("Execution time: %f\n", time);
        printf("Freezing graphics 10 seconds in end state\n");
        drawGraphics(N, boardState, 10000000);
    }

    /*
    Without graphics, the starting board is, just as before scattered into the local matrices,
    thereafter the boardstate for the next timestep is computed and the processes each send out their 
    rows second from the top and bottom, which are placed as the bottom and top rows of the processes
    below and above in rank respectively. 
    */
    } 
    else
    {
        MPI_Scatterv(boardState, counts, displs, MPI_CHAR, local, (N+2)*(rows+2), MPI_CHAR, 0, MPI_COMM_WORLD);   
        for (i = 0; i < nsteps; i++)
        {
            local = Step(local, N, rows);
            if (rank == 0)
            {
                MPI_Isend(local+(N+2)*(rows), (N+2), MPI_CHAR, rank+1, (rank)*112, MPI_COMM_WORLD, &request);

                MPI_Recv(local+(N+2)*(rows+1), (N+2), MPI_CHAR, rank+1, (rank+1)*111, MPI_COMM_WORLD, &status);
            } 
            else if (rank == size-1) 
            {  
                MPI_Isend(local+(N+2), (N+2), MPI_CHAR, rank-1, (rank)*111, MPI_COMM_WORLD, &request);

                MPI_Recv(local, (N+2), MPI_CHAR, rank-1, (rank-1)*112, MPI_COMM_WORLD, &status);  
            } 
            else 
            {
                MPI_Isend(local+(N+2)*(rows), (N+2), MPI_CHAR, rank+1, (rank)*112, MPI_COMM_WORLD, &request);
                MPI_Isend(local+(N+2), (N+2), MPI_CHAR, rank-1, (rank)*111, MPI_COMM_WORLD, &request);
                
                MPI_Recv(local, (N+2), MPI_CHAR, rank-1, (rank-1)*112, MPI_COMM_WORLD, &status);
                MPI_Recv(local+(N+2)*(rows+1), (N+2), MPI_CHAR, rank+1, (rank+1)*111, MPI_COMM_WORLD, &status);
            }
        }
        MPI_Gatherv(local+(N+2), (N+2)*(rows), MPI_CHAR, boardState, sendback, displsBack, MPI_CHAR, 0, MPI_COMM_WORLD);
        time = MPI_Wtime() - starttime;

        if(rank == 0 ){
            printf("Exeution time: %f\n", time);
            printf("Endstate after %d timesteps displayed for 10 seconds\n", nsteps);
            if(graphics == 0 ){
                InitializeGraphics("gameOfLife", windowWidth, windowHeight); //If the program can't find any X server, this will crash the program, this is however after the execution 
                SetCAxes(0, 1);
                drawGraphics(N, boardState, 10000000);
            }
        } 
    }
    MPI_Finalize();

    return 0;
}

// The stepping function
char *Step(char *boardState, int N, int rows)
{
    newBoardState = calloc((N+2) * (rows+2),sizeof(char));
    int i, j, k, l; 
    int neighbours = 0;
    
    // i and j are used to cycle through the pixels of a matrix, while k and l are used to look at each pixel's neighbours
    for (i = 1; i < rows+1; i++)
    {
        for (j = 1; j < N+1; j++)
        {
            for (k = i - 1; k < i + 2; k++)
            {
                for (l = j - 1; l < j + 2; l++)
                {
                        if (!(i == k && j == l))
                        {
                            neighbours += boardState[k*(N+2)+l];
                        }
                }
            }

        // Stating the rules of Game of Life
        if (neighbours == 2)
        {
            newBoardState[i*(N+2)+j] = boardState[i*(N+2)+j];
        }
        else if (neighbours == 3)
        {
            newBoardState[i*(N+2)+j] = 1;
        }
        else
        {
            newBoardState[i*(N+2)+j] = 0;
        }
        neighbours = 0;
    }
}
    return newBoardState;
}


/* The matrix has the size of N*N given as input, with 1's along the boundary and 0's elsewhere.
   The matrix is also zero-padded, so neighbourchecks in the step function don't go out of bounds.*/

void square(int N)
{
    for (int i = 1; i < N+1; i++)
    {
        for(int j = 1; j < N+1; j++)
        {
            if (i > 1 && i < N+1 && (j == 1 || j == N))
            {
                boardState[i*(N+2)+j] = 1;
            }
            else if (i==1 || i == N)
            {
                boardState[i*(N+2)+j] = 1;
            }
        }
    }
}


// A single floater (or glider) in starting in the lower left corner
void floater(int N)
{
for (int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++){
            if (i == 1 && j==2)
            {
                boardState[i*(N+2)+j] = 1;
            }
            else if (i == 2 && j == 3)
            {
                boardState[i*(N+2)+j] = 1;
            }
            else if (i == 3 && (j==1 || j==2 || j==3))
            {
                boardState[i*(N+2)+j] = 1;
                boardState[i*(N+2)+j] = 1;
                boardState[i*(N+2)+j] = 1;
            }
            }              
    }
}

// A glider gun in the lower left corner, shooting gliders diagonally up/right
void gosper_glider_gun(int N)
{
    boardState[N+2+25] = 1;

    boardState[2*(N+2)+23] = 1;
    boardState[2*(N+2)+25] = 1;
    
    boardState[3*(N+2)+13] = 1;
    boardState[3*(N+2)+14] = 1;
    boardState[3*(N+2)+21] = 1;
    boardState[3*(N+2)+22] = 1;
    boardState[3*(N+2)+35] = 1;
    boardState[3*(N+2)+36] = 1;

    boardState[4*(N+2)+12] = 1;
    boardState[4*(N+2)+16] = 1;
    boardState[4*(N+2)+21] = 1;
    boardState[4*(N+2)+22] = 1;
    boardState[4*(N+2)+35] = 1;
    boardState[4*(N+2)+36] = 1;

    boardState[5*(N+2)+1] = 1;
    boardState[5*(N+2)+2] = 1;
    boardState[5*(N+2)+11] = 1;
    boardState[5*(N+2)+17] = 1;
    boardState[5*(N+2)+21] = 1;
    boardState[5*(N+2)+22] = 1;

    boardState[6*(N+2)+1] = 1;
    boardState[6*(N+2)+2] = 1;
    boardState[6*(N+2)+11] = 1;
    boardState[6*(N+2)+15] = 1;
    boardState[6*(N+2)+17] = 1;
    boardState[6*(N+2)+18] = 1;
    boardState[6*(N+2)+23] = 1;
    boardState[6*(N+2)+25] = 1;

    boardState[7*(N+2)+11] = 1;
    boardState[7*(N+2)+17] = 1;
    boardState[7*(N+2)+25] = 1;
    
    boardState[8*(N+2)+12] = 1;
    boardState[8*(N+2)+16] = 1;

    boardState[9*(N+2)+13] = 1;
    boardState[9*(N+2)+14] = 1;    
}

// The function to draw the graphics 
void drawGraphics(int N,char * boardState, float waitTime)
{
    float oneDivN = 1 / (float)(N);
    ClearScreen();
    for (int i = 1; i < N + 2; i++)
    {
        for (int j = 1; j < N + 2; j++)
        {
            if (boardState[(N+2)*j+i] == 1) DrawRectangle((float)(i - 1) * oneDivN, (float)(j - 1) * oneDivN, W, H, oneDivN, oneDivN, Color);
        }
    }
    Refresh();
    usleep(waitTime);
}

// For printing the matrices in the terminal
void print_matrix(char* A, int n, int rows){
    int i,j;
    for (i=0; i<(rows+2)*(n+2); i++) {
        printf("%c ", A[i]);
        if ((i+1)%(n+2) == 0) printf("\n");
    }
    printf("\n");
}
