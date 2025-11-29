#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include <cstdlib>
#include <iostream>
#include <climits>

using namespace std;

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
bool determineNextPosition()
{
}

// ----------------------------------------------------------------------------
// GET NEXT DIRECTION based on current tile and direction
// ----------------------------------------------------------------------------
// Return new direction after entering the tile.
// ----------------------------------------------------------------------------
int getNextDirection()
{
}

// ----------------------------------------------------------------------------
// SMART ROUTING AT CROSSING - Route train to its matched destination
// ----------------------------------------------------------------------------
// Choose best direction at '+' toward destination.
// ----------------------------------------------------------------------------
int getSmartDirectionAtCrossing()
{
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
