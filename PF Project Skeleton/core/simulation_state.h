#ifndef SIMULATION_STATE_H
#define SIMULATION_STATE_H


#define MAX_ROWS 64
#define MAX_COLS 64
#define MAX_TRAINS 100
#define MAX_SWITCHES 26 // A through Z


// Directions
#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3

// Train Lifecycle States
#define TRAIN_STATE_SCHEDULED 0 // Not on map yet
#define TRAIN_STATE_ACTIVE 1    // Currently moving
#define TRAIN_STATE_ARRIVED 2   // Reached D
#define TRAIN_STATE_CRASHED 3   // Collided
#define TRAIN_STATE_WAITING 4   // Waiting for spawn slot

//switch constents
#define SWITCH_MODE_PER_DIR 0
#define SWITCH_MODE_GLOBAL 1


#define WEATHER_NORMAL 0
#define WEATHER_RAIN 1
#define WEATHER_FOG 2


// singal constants
#define SIGNAL_GREEN 0
#define SIGNAL_YELLOW 1
#define SIGNAL_RED 2

//grid state
extern int rows;
extern int cols;
extern char grid[MAX_ROWS][MAX_COLS];       // The base track layout 
extern bool safetyTiles[MAX_ROWS][MAX_COLS]; // True if a safety tile is placed here


extern int numTrains; // Total number of trains 

// Properties read from file
extern int spawnTick[MAX_TRAINS];
extern int spawnX[MAX_TRAINS];
extern int spawnY[MAX_TRAINS];
extern int spawn_dir[MAX_TRAINS];
extern int trainColor[MAX_TRAINS]; // 0 or 1 usually

// Dynamic properties 
extern int train_id[MAX_TRAINS];
extern int trainState[MAX_TRAINS];
extern int train_x[MAX_TRAINS];
extern int train_y[MAX_TRAINS];
extern int currDir[MAX_TRAINS];   // 0=Up, 1=Right, 2=Down, 3=Left
extern int delay[MAX_TRAINS]; 

//switches states
extern bool switchActive[MAX_SWITCHES]; // True if this letter exists in map
extern int sw_mode[MAX_SWITCHES];    // PER_DIR or GLOBAL
extern int switchState[MAX_SWITCHES];   // 0 or 1
extern int initial_state[MAX_SWITCHES];

 //threshold values
extern int kValues[MAX_SWITCHES][4];


extern int counters[MAX_SWITCHES][4];

// Deferred Flip Queue (Set to true if switch should flip at end of tick)
extern bool flipQueued[MAX_SWITCHES];

//global varibales
extern int tick;
extern int seed;
extern int weather;


extern int arrivedCount;
extern int crashedCount;
extern int switchFlips;
extern long totalWaitTime;

//initialization function
void initializeSimulationState();

#endif