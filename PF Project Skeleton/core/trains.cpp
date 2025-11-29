#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include <cstdlib>
#include <iostream>
#include <climits>

using namespace std;

//for storing info
static int store_nextX[MAX_TRAINS];
static int store_nextY[MAX_TRAINS];
static int store_nextDir[MAX_TRAINS];
static bool store_moveValid[MAX_TRAINS];

void spawnTrainsForTick()
{
    for (int i = 0; i < numTrains; i++)
    {
        if (trainState[i] == TRAIN_STATE_SCHEDULED && spawnTick[i] <= tick)
        {

            int tx = spawnX[i];
            int ty = spawnY[i];

            if (grid[ty][tx] != 'S')
            {
                int bestDist = INT_MAX;
                int bestX = -1;
                int bestY = -1;

                //search in gird
                for (int r = 0; r < rows; r++)
                {
                    for (int c = 0; c < cols; c++)
                    {
                        if (grid[r][c] == 'S')
                        {
                            //manhatten distance for destination 
                            int dist = abs(c - tx) + abs(r - ty);
                            if (dist < bestDist)
                            {
                                bestDist = dist;
                                bestX = c;
                                bestY = r;
                            }
                        }
                    }
                }

                //relocate the nearest spawn point, because some spawn points are incorrect given in level files
                if (bestX != -1)
                {
                    cout << "Warning: Train " << i << " config (" << tx << "," << ty
                         << ") invalid. Relocating to nearest Spawn at (" << bestX << "," << bestY << ")." << endl;
                    tx = bestX;
                    ty = bestY;
                }
            }

            //occupancy check
            bool occupied = false;
            for (int j = 0; j < numTrains; j++)
            {
                if (trainState[j] == TRAIN_STATE_ACTIVE && train_x[j] == tx && train_y[j] == ty)
                {
                    occupied = true;
                    break;
                }
            }

            //spawn
            if (!occupied)
            {
                trainState[i] = TRAIN_STATE_ACTIVE;
                train_x[i] = tx;
                train_y[i] = ty;
                currDir[i] = spawn_dir[i];
                cout << "Spawned Train " << i << " at (" << tx << "," << ty << ")" << endl;
            }
            else
            {
                cout << "Train " << i << " delayed (Spawn blocked)" << endl;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// DETERMINE NEXT POSITION for a train
// ----------------------------------------------------------------------------
// Compute next position/direction from current tile and rules.
// ----------------------------------------------------------------------------
bool determineNextPosition(int trainId)
{
     int cx = train_x[trainId];
    int cy = train_y[trainId];
    int cdir = currDir[trainId];

 
    int nextDir = getNextDirection(trainId, cx, cy, cdir);

    int nx = cx;
    int ny = cy;

    if (nextDir == DIR_UP)
        ny--;
    else if (nextDir == DIR_DOWN)
        ny++;
    else if (nextDir == DIR_LEFT)
        nx--;
    else if (nextDir == DIR_RIGHT)
        nx++;

    //store result 
    store_nextX[trainId] = nx;
    store_nextY[trainId] = ny;
    store_nextDir[trainId] = nextDir;

    // Cvhecking for Bounds
   if(!isInBounds(nx , ny)){
    return false;
   }

    //checking for track existnece
    if (grid[ny][nx] == '.' || grid[ny][nx] == ' ')
        return false;

    return true;
}

// ----------------------------------------------------------------------------
// GET NEXT DIRECTION based on current tile and direction
// ----------------------------------------------------------------------------
// Return new direction after entering the tile.
// ----------------------------------------------------------------------------
int getNextDirection(int trainId, int x, int y, int inDir)
{
    char tile = grid[y][x];

    // checking for next direction accroding to tile
    if (tile == '/')
    {
        if (inDir == DIR_UP)
            return DIR_RIGHT;
        if (inDir == DIR_LEFT)
            return DIR_DOWN;
        if (inDir == DIR_DOWN)
            return DIR_LEFT;
        if (inDir == DIR_RIGHT)
            return DIR_UP;
    }
    else if (tile == '\\')
    {
        if (inDir == DIR_UP)
            return DIR_LEFT;
        if (inDir == DIR_RIGHT)
            return DIR_DOWN;
        if (inDir == DIR_DOWN)
            return DIR_RIGHT;
        if (inDir == DIR_LEFT)
            return DIR_UP;
    }
    else if (tile >= 'A' && tile <= 'Z')
    {
        int idx = tile - 'A';
        //checking for different states
        if (switchState[idx] == 1)
        {
            // Turn logic similar to curves
            if (inDir == DIR_UP)
                return DIR_RIGHT;
            if (inDir == DIR_DOWN)
                return DIR_LEFT;
            if (inDir == DIR_LEFT)
                return DIR_UP;
            if (inDir == DIR_RIGHT)
                return DIR_DOWN;
        }
    }

    else if (tile == '+')
    {
        return getSmartDirectionAtCrossing(x, y);
    }

    return inDir;
}

// ----------------------------------------------------------------------------
// SMART ROUTING AT CROSSING - Route train to its matched destination
// ----------------------------------------------------------------------------
// Choose best direction at '+' toward destination.
// ----------------------------------------------------------------------------
int getSmartDirectionAtCrossing(int x, int y)
{
     //Find the nearest destination tile for the train
    int minDist = INT_MAX;
    int targetX = -1, targetY = -1;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (grid[r][c] == 'D')
            {
                int dist = abs(c - x) + abs(r - y);
                if (dist < minDist)
                {
                    minDist = dist;
                    targetX = c;
                    targetY = r;
                }
            }
        }
    }

    if (targetX == -1)
        return DIR_RIGHT; // no destination found

    //picking the distancce with least distance
    int bestDir = DIR_RIGHT;
    int bestDist = INT_MAX;

    // checking neightbours
    int dx[] = {0, 1, 0, -1}; 
    int dy[] = {-1, 0, 1, 0};

    for (int d = 0; d < 4; d++)
    {
        int nx = x + dx[d];
        int ny = y + dy[d];

        // must be grid
        if (isInBounds(nx, ny) && grid[ny][nx] != ' ')
        {
            int distToDest = abs(targetX - nx) + abs(targetY - ny);
            if (distToDest < bestDist)
            {
                bestDist = distToDest;
                bestDir = d;
            }
        }
    }

    return bestDir;
}

// ----------------------------------------------------------------------------
// DETERMINE ALL ROUTES (PHASE 2)
// ----------------------------------------------------------------------------
// Fill next positions/directions for all trains.
// ----------------------------------------------------------------------------
void determineAllRoutes()
{
}

// ----------------------------------------------------------------------------
// MOVE ALL TRAINS (PHASE 5)
// ----------------------------------------------------------------------------
// Move trains; resolve collisions and apply effects.
// ----------------------------------------------------------------------------
void moveAllTrains()
{
}

// ----------------------------------------------------------------------------
// DETECT COLLISIONS WITH PRIORITY SYSTEM
// ----------------------------------------------------------------------------
// Resolve same-tile, swap, and crossing conflicts.
// ----------------------------------------------------------------------------
void detectCollisions()
{
}

// ----------------------------------------------------------------------------
// CHECK ARRIVALS
// ----------------------------------------------------------------------------
// Mark trains that reached destinations.
// ----------------------------------------------------------------------------
void checkArrivals()
{
}

// ----------------------------------------------------------------------------
// APPLY EMERGENCY HALT
// ----------------------------------------------------------------------------
// Apply halt to trains in the active zone.
// ----------------------------------------------------------------------------
void applyEmergencyHalt()
{
}

// ----------------------------------------------------------------------------
// UPDATE EMERGENCY HALT
// ----------------------------------------------------------------------------
// Decrement timer and disable when done.
// ----------------------------------------------------------------------------
void updateEmergencyHalt()
{
}
