#ifndef SIMULATION_H
#define SIMULATION_H

// ============================================================================
// SIMULATION.H - Simulation tick logic
// ============================================================================

void printGridToTerminal();
// ----------------------------------------------------------------------------
// MAIN SIMULATION FUNCTION
// ----------------------------------------------------------------------------
// Run one simulation tick.
void simulateOneTick();


// ----------------------------------------------------------------------------
// UTILITY
// ----------------------------------------------------------------------------
// True if all trains are delivered or crashed.
bool isSimulationComplete();

void updateTick();

#endif


