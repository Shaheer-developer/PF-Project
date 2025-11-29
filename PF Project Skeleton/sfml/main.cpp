#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/io.h"
#include <iostream>

using namespace std;

// ============================================================================
// MAIN.CPP - Entry point of the application (NO CLASSES)
// ============================================================================

// ----------------------------------------------------------------------------
// MAIN ENTRY POINT
// ----------------------------------------------------------------------------
// This function is the main entry point of the application. It handles command
// line arguments to specify the level file to load, loads the level file using
// loadLevelFile, initializes the simulation system, initializes the SFML
// application window, prints control instructions to the console, runs the
// main application loop, cleans up resources, and prints final simulation
// statistics. Returns 0 on success, 1 on error (e.g., failed to load level
// file or initialize application).
// ----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    cout << "[DEBUG] Starting program..." << endl;

    if (argc < 2) {
        cerr << "Usage: ./switchback_rails <level_file>" << endl;
        return 1;
    }

    cout << "[DEBUG] Loading level file: " << argv[1] << endl;

    if (!loadLevelFile(argv[1])) {
        cerr << "[ERROR] Failed to load level file." << endl;
        return 1;
    }

    cout << "[DEBUG] Level loaded successfully." << endl;
    cout << "[DEBUG] Initializing Graphics..." << endl;

    if (!initializeApp()) {
        cerr << "[ERROR] Failed to initialize App (Window creation failed)." << endl;
        return 1;
    }

    cout << "[DEBUG] Graphics Initialized. Starting Loop..." << endl;

    runApp();

    cout << "[DEBUG] Loop finished. Cleaning up..." << endl;

    cleanupApp();

    cout << "[DEBUG] Exiting." << endl;
    return 0;
}
