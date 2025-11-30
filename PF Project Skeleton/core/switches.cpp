#include "switches.h"
#include "simulation_state.h"
#include "grid.h"
#include "io.h"

// ============================================================================
// SWITCHES.CPP - Switch management
// ============================================================================

// ----------------------------------------------------------------------------
// UPDATE SWITCH COUNTERS
// ----------------------------------------------------------------------------
// Increment counters for trains entering switches.
// ----------------------------------------------------------------------------
void updateSwitchCounters()
{
    for (int i = 0; i < numTrains; i++)
    {
        if (trainState[i] != TRAIN_STATE_ACTIVE)
            continue;

        int x = train_x[i];
        int y = train_y[i];
        char tile = grid[y][x];

        if (tile >= 'A' && tile <= 'Z')
        {
            int swIdx = tile - 'A';

            // calculating from wher the train is coming from , like if moving right means it comes from left
            //(dir+2)%4 gives opposite direction
            int entryDir = (currDir[i] + 2) % 4;

            // incrment based on mode
            if (sw_mode[swIdx] == SWITCH_MODE_GLOBAL)
            {
                // global
                counters[swIdx][0]++;
            }
            else
            {
                // according to direction
                counters[swIdx][entryDir]++;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// QUEUE SWITCH FLIPS
// ----------------------------------------------------------------------------
// Queue flips when counters hit K.
// ----------------------------------------------------------------------------
void queueSwitchFlips()
{
     for (int i = 0; i < MAX_SWITCHES; i++)
    {
        if (!switchActive[i])
            continue;

        //checking for all four directions
        for (int dir = 0; dir < 4; dir++)
        {

            //determining the k value
            int currentCount = counters[i][dir];
            int kValue = kValues[i][dir];

            if (sw_mode[i] == SWITCH_MODE_GLOBAL && dir > 0)
                continue;

            //if limit reached flipt he queue
            if (currentCount >= kValue && kValue > 0)
            {
                flipQueued[i] = true;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// APPLY DEFERRED FLIPS
// ----------------------------------------------------------------------------
// Apply queued flips after movement.
// ----------------------------------------------------------------------------
void applyDeferredFlips()
{
}

// ----------------------------------------------------------------------------
// UPDATE SIGNAL LIGHTS
// ----------------------------------------------------------------------------
// Update signal colors for switches.
// ----------------------------------------------------------------------------
void updateSignalLights()
{
}

// ----------------------------------------------------------------------------
// TOGGLE SWITCH STATE (Manual)
// ----------------------------------------------------------------------------
// Manually toggle a switch state.
// ----------------------------------------------------------------------------
void toggleSwitchState()
{
}

// ----------------------------------------------------------------------------
// GET SWITCH STATE FOR DIRECTION
// ----------------------------------------------------------------------------
// Return the state for a given direction.
// ----------------------------------------------------------------------------
int getSwitchStateForDirection()
{
}
