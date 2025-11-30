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

void initializeLogFiles()
{
    // initializing trace.csv
    ofstream trace("out/trace.csv");
    if (!trace.is_open())
    {
        cout << "Warning: Could not create out/trace.csv (make sure 'out' directory exists)" << endl;
    }
    else
    {
        trace << "Tick,TrainID,X,Y,Direction,State\n";
        trace.close();
    }

    // intitializing switchse.csv
    ofstream switches("out/switches.csv");
    if (!switches.is_open())
    {
        cout << "Warning: Could not create out/switches.csv" << endl;
    }
    else
    {
        switches << "Tick,Switch,Mode,State\n";
        switches.close();
    }

    cout << "Log files initialized in out/ directory" << endl;
}

bool loadLevelFile(const char *filename)
{
    // reset state
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
            string dummy; // to skip spaces
            getline(file, dummy);

            for (int r = 0; r < rows; r++)
            {
                // save pos if we exceed
                streampos oldPos = file.tellg();

                string rowStr;
                getline(file, rowStr);
                cleanString(rowStr);

                if (rowStr.find("SWITCHES:") != string::npos || rowStr.find("TRAINS:") != string::npos)
                {
                    // return one position backwsrd
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

                // save pos for token
                streampos pos = file.tellg();

                string token;
                file >> token;

                if (token == "TRAINS:" || token == "TRAINS")
                {
                    // go back if hit train
                    file.seekg(pos);
                    break;
                }

                // passing switch
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
                if (!isdigit(file.peek())) // stop if not a number
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

    initializeLogFiles();

    return true;
}

// ----------------------------------------------------------------------------
// LOG TRAIN TRACE
// ----------------------------------------------------------------------------
// Append tick, train id, position, direction, state to trace.csv.
// ----------------------------------------------------------------------------
void logTrainTrace()
{
    ofstream file("out/trace.csv", ios::app);

    if (!file.is_open())
    {
        return; //return in case file is not open
    }

    for (int i = 0; i < numTrains; i++)
    {
        // write all the trains that are active , arived or crashed
        if (trainState[i] == TRAIN_STATE_ACTIVE || trainState[i] == TRAIN_STATE_ARRIVED || trainState[i] == TRAIN_STATE_CRASHED)
        {
            file << tick << ","
                 << i << ","
                 << train_x[i] << ","
                 << train_y[i] << ","
                 << currDir[i] << ","
                 << trainState[i] << "\n";
        }
    }

    file.close();
}

// ----------------------------------------------------------------------------
// LOG SWITCH STATE
// ----------------------------------------------------------------------------
// Append tick, switch id/mode/state to switches.csv.
// ----------------------------------------------------------------------------
void logSwitchState()
{
     ofstream file("out/switches.csv", ios::app);
    
    if (!file.is_open())
    {
        return; 
    }

    //write all the active switches
    for (int i = 0; i < MAX_SWITCHES; i++)
    {
        if (switchActive[i])
        {
            string modeStr = (sw_mode[i] == SWITCH_MODE_PER_DIR) ? "PER_DIR" : "GLOBAL";
            
            file << tick << ","
                 << (char)('A' + i) << ","
                 << modeStr << ","
                 << switchState[i] << "\n";
        }
    }
    
    file.close();
}

// ----------------------------------------------------------------------------
// WRITE FINAL METRICS
// ----------------------------------------------------------------------------
// Write summary metrics to metrics.txt.
// ----------------------------------------------------------------------------
void writeMetrics()
{
      ofstream file("out/metrics.txt");

    if (!file.is_open())
    {
        cerr << "Error: Could not open metrics.txt for writing" << endl;
        return;
    }

    // basic metrics
    file << "=== SWITCHBACK RAILS SIMULATION METRICS ===" << "\n\n";
    file << "Total Ticks Simulated: " << tick << "\n";
    file << "Total Trains: " << numTrains << "\n";
    file << "Trains Arrived: " << arrivedCount << "\n";
    file << "Trains Crashed: " << crashedCount << "\n";
    file << "Trains Still Active: " << (numTrains - arrivedCount - crashedCount) << "\n";
    file << "Total Switch Flips: " << switchFlips << "\n\n";

    // calculating success rate
    if (numTrains > 0)
    {
        double successRate = (arrivedCount * 100.0) / numTrains;
        double crashRate = (crashedCount * 100.0) / numTrains;
        file << "Success Rate: " << successRate << "%\n";
        file << "Crash Rate: " << crashRate << "%\n\n";
    }

    // calculating efficency
    file << "=== EFFICIENCY METRICS ===" << "\n";
    if (arrivedCount > 0)
    {
        file << "Average Switch Flips per Delivered Train: " << (switchFlips * 1.0 / arrivedCount) << "\n";
        file << "Average Ticks per Delivered Train: " << (tick * 1.0 / arrivedCount) << "\n";
    }
    else
    {
        file << "No trains delivered - efficiency metrics unavailable\n";
    }

    file << "\n=== SWITCH STATISTICS ===" << "\n";
    int activeSwitches = 0;
    for (int i = 0; i < MAX_SWITCHES; i++)
    {
        if (switchActive[i])
        {
            activeSwitches++;
            file << "Switch " << (char)('A' + i) << ": ";
            file << "Mode=" << (sw_mode[i] == SWITCH_MODE_PER_DIR ? "PER_DIR" : "GLOBAL");
            file << ", Final State=" << switchState[i];
            file << ", K-values=[" << kValues[i][0] << "," << kValues[i][1]
                 << "," << kValues[i][2] << "," << kValues[i][3] << "]";
            file << "\n";
        }
    }
    file << "Total Active Switches: " << activeSwitches << "\n";

    // per train statics
    file << "\n=== PER-TRAIN DETAILS ===" << "\n";
    for (int i = 0; i < numTrains; i++)
    {
        file << "Train " << i << ": ";

        if (trainState[i] == TRAIN_STATE_ARRIVED)
            file << "ARRIVED";
        else if (trainState[i] == TRAIN_STATE_CRASHED)
            file << "CRASHED";
        else if (trainState[i] == TRAIN_STATE_ACTIVE)
            file << "STILL ACTIVE";
        else if (trainState[i] == TRAIN_STATE_SCHEDULED)
            file << "NEVER SPAWNED";
        else
            file << "UNKNOWN STATE";

        file << " (Spawn Tick: " << spawnTick[i]<< ")\n";
    }

    file.close();
}
