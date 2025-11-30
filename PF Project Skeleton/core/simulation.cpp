#include "simulation.h"
#include "simulation_state.h"
#include "trains.h"
#include "switches.h"
#include "io.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// ============================================================================
// SIMULATION.CPP - Implementation of main simulation logic
// ============================================================================

// ----------------------------------------------------------------------------
// SIMULATE ONE TICK
// ----------------------------------------------------------------------------
void printGridToTerminal()
{
    cout << "\n========== Tick: " << tick << " ==========\n";

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            // checking if train is present, then we wil show train and if train not present then show the tile
            bool trainHere = false;
            for (int i = 0; i < numTrains; i++)
            {
                if (trainState[i] == TRAIN_STATE_ACTIVE &&
                    train_x[i] == c && train_y[i] == r)
                {
                    cout << i; // show train
                    trainHere = true;
                    break;
                }
            }
            if (!trainHere)
            {
                cout << grid[r][c]; // show tile
            }
        }
        cout << endl;
    }
    cout << "=====================================\n";
}

void simulateOneTick()
{
    // spawn
    spawnTrainsForTick();

    // route determination
    determineAllRoutes();

    // update switch counter
    updateSwitchCounters();

    // flip acc to k values
    queueSwitchFlips();

    // movement
    moveAllTrains();

    // change state and reset counter
    applyDeferredFlips();

    printGridToTerminal();
}

// ----------------------------------------------------------------------------
// CHECK IF SIMULATION IS COMPLETE
// ----------------------------------------------------------------------------

bool isSimulationComplete()
{
}

void updateTick()
{
    tick++;

    simulateOneTick();
}