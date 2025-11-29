#include "io.h"
#include "simulation_state.h"
#include "grid.h"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;


void cleanString(string &str)
{
    str.erase(remove(str.begin(), str.end(), '\r'), str.end());
}

bool loadLevelFile(const char *filename)
{
    //reset state
    initializeSimulationState();

    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Could not open file " << filename << endl;
        return false;
    }

    string line;
    while (file >> line)
    {

        if (line == "ROWS:")
        {
            file >> rows;
        }
        else if (line == "COLS:")
        {
            file >> cols;
        }
        else if (line == "MAP:")
        {
            string dummy;  //to skip spaces
            getline(file, dummy);

            for (int r = 0; r < rows; r++)
            {
                //save pos if we exceed 
                streampos oldPos = file.tellg();

                string rowStr;
                getline(file, rowStr);
                cleanString(rowStr);

                if (rowStr.find("SWITCHES:") != string::npos || rowStr.find("TRAINS:") != string::npos)
                {
                    //return one position backwsrd
                    file.seekg(oldPos); 
                    break;
                }

                for (int c = 0; c < cols; c++)
                {
                    if (c < (int)rowStr.length())
                    {
                        grid[r][c] = rowStr[c];
                        if (grid[r][c] >= 'A' && grid[r][c] <= 'Z')
                        {
                            int idx = grid[r][c] - 'A';
                            switchActive[idx] = true;
                        }
                    }
                    else
                    {
                        grid[r][c] = ' ';
                    }
                }
            }
        }
        else if (line == "SWITCHES:")
        {
            while (true)
            {
                while (isspace(file.peek()))
                    file.get();
                if (file.peek() == EOF)
                    break;

                //save pos for token
                streampos pos = file.tellg();

                string token;
                file >> token;

                if (token == "TRAINS:" || token == "TRAINS")
                {
                    //go back if hit train
                    file.seekg(pos);
                    break;
                }

                //passing switch
                int idx = token[0] - 'A';
                if (idx >= 0 && idx < MAX_SWITCHES)
                {
                    string mode;
                    file >> mode;
                    sw_mode[idx] = (mode == "PER_DIR") ? 0 : 1;

                    file >> initial_state[idx];
                    switchState[idx] = initial_state[idx];

                    for (int k = 0; k < 4; k++)
                        file >> kValues[idx][k];
                    string l1, l2;
                    file >> l1 >> l2;
                }
            }
        }
        else if (line == "TRAINS:" || line == "TRAINS")
        {
            while (file.good() && numTrains < MAX_TRAINS)
            {
                while (isspace(file.peek()))
                    file.get();
                if (!isdigit(file.peek()))//stop if not a number
                    break; 

                file >> spawnTick[numTrains];
                file >> spawnX[numTrains];
                file >> spawnY[numTrains];
                file >> spawn_dir[numTrains];
                file >> trainColor[numTrains];
                numTrains++;
            }
        }
    }

    cout << "Level Loaded: " << rows << "x" << cols << endl;
    cout << "Total Trains: " << numTrains << endl;
    return true;
}

// ----------------------------------------------------------------------------
// INITIALIZE LOG FILES
// ----------------------------------------------------------------------------
// Create/clear CSV logs with headers.
// ----------------------------------------------------------------------------
void initializeLogFiles() {
}

// ----------------------------------------------------------------------------
// LOG TRAIN TRACE
// ----------------------------------------------------------------------------
// Append tick, train id, position, direction, state to trace.csv.
// ----------------------------------------------------------------------------
void logTrainTrace() {
}

// ----------------------------------------------------------------------------
// LOG SWITCH STATE
// ----------------------------------------------------------------------------
// Append tick, switch id/mode/state to switches.csv.
// ----------------------------------------------------------------------------
void logSwitchState() {
}

// ----------------------------------------------------------------------------
// LOG SIGNAL STATE
// ----------------------------------------------------------------------------
// Append tick, switch id, signal color to signals.csv.
// ----------------------------------------------------------------------------
void logSignalState() {
}

// ----------------------------------------------------------------------------
// WRITE FINAL METRICS
// ----------------------------------------------------------------------------
// Write summary metrics to metrics.txt.
// ----------------------------------------------------------------------------
void writeMetrics() {
}
