#include "simulation.h"
#include "simulation_state.h"
#include "trains.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <ctime>

// ============================================================================
// SIMULATION.CPP - Implementation of main simulation logic
// ============================================================================

// ----------------------------------------------------------------------------
// SIMULATE ONE TICK
// ----------------------------------------------------------------------------

void simulateOneTick() {
        //spawn
    spawnTrainsForTick();

    //route determination
    determineAllRoutes();

    //update switch counter
    updateSwitchCounters();

    //flip acc to k values
    queueSwitchFlips();

    //movement
    moveAllTrains();

    //change state and reset counter
    applyDeferredFlips();
    
}

// ----------------------------------------------------------------------------
// CHECK IF SIMULATION IS COMPLETE
// ----------------------------------------------------------------------------

bool isSimulationComplete() {
}

void updateTick(){
    tick++;

    simulateOneTick();

}