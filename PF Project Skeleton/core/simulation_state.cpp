#include "simulation_state.h"
#include <cstring>
using namespace std;


//varibles initialized declared in simulation_state.h
int rows = 0;
int cols = 0;
char grid[MAX_ROWS][MAX_COLS];
bool safetyTiles[MAX_ROWS][MAX_COLS];


int numTrains = 0;
int spawnTick[MAX_TRAINS];
int spawnX[MAX_TRAINS];
int spawnY[MAX_TRAINS];
int spawn_dir[MAX_TRAINS];
int trainColor[MAX_TRAINS];

int train_id[MAX_TRAINS];
int trainState[MAX_TRAINS];
int train_x[MAX_TRAINS];
int train_y[MAX_TRAINS];
int currDir[MAX_TRAINS];
int delay[MAX_TRAINS];


bool switchActive[MAX_SWITCHES];
int sw_mode[MAX_SWITCHES];
int switchState[MAX_SWITCHES];
int initial_state[MAX_SWITCHES];
int kValues[MAX_SWITCHES][4];
int counters[MAX_SWITCHES][4];
bool flipQueued[MAX_SWITCHES];

int tick = 0;
int seed = 0;
int weather = WEATHER_NORMAL;


int arrivedCount = 0;
int crashedCount = 0;
int switchFlips = 0;
long totalWaitTime = 0;

//intialize simmulation state
void initializeSimulationState()
{
    // Reset integers
    rows = 0;
    cols = 0;
    numTrains = 0;
    tick = 0;
    seed = 0;
    weather = WEATHER_NORMAL;

    // Reset Metrics
    arrivedCount = 0;
    crashedCount = 0;
    switchFlips = 0;
    totalWaitTime = 0;

   
    // set everything to 0
    memset(grid, '.', sizeof(grid));
    memset(safetyTiles, 0, sizeof(safetyTiles));

    memset(switchActive, 0, sizeof(switchActive));
    memset(counters, 0, sizeof(counters));
    memset(flipQueued, 0, sizeof(flipQueued));

    // Initialize trains 
    for (int i = 0; i < MAX_TRAINS; i++)
    {
        trainState[i] = TRAIN_STATE_SCHEDULED;
    }
}
