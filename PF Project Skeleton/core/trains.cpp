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

int getDistanceToDestination(int trainId)
{
    int cx = train_x[trainId];
    int cy = train_y[trainId];
    int minDist = INT_MAX;

    // manhatten distance to nearest destination
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            if (grid[r][c] == 'D')
            {
                int dist = abs(c - cx) + abs(r - cy);
                if (dist < minDist)
                {
                    minDist = dist;
                }
            }
        }
    }
    return (minDist == INT_MAX) ? 0 : minDist;
}


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

        cout<< "Train "<< trainId<< " at ("<<cx<<","<<cy<<") "<<endl;

    return true;
}


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

        // must be gird
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


void determineAllRoutes()
{
    for (int i = 0; i < numTrains; i++)
    {
        if (trainState[i] == TRAIN_STATE_ACTIVE)
        {
            //determine routes for train and where the train will go 
            store_moveValid[i] = determineNextPosition(i);
        }
        else
        {
            store_moveValid[i] = false;
        }
    }
}


void moveAllTrains()
{
        detectCollisions();

    for (int i = 0; i < numTrains; i++)
    {
        if (trainState[i] == TRAIN_STATE_ACTIVE && store_moveValid[i])
        {
            train_x[i] = store_nextX[i];
            train_y[i] = store_nextY[i];
            currDir[i] = store_nextDir[i];
        }
    }
    checkArrivals();
}


void detectCollisions()
{
     int dist[MAX_TRAINS];
    for (int i = 0; i < numTrains; i++)
    {
        if (trainState[i] == TRAIN_STATE_ACTIVE)
        {
            dist[i] = getDistanceToDestination(i);
        }
        else
        {
            dist[i] = 0;
        }
    }

    for (int i = 0; i < numTrains; i++)
    {
        if (trainState[i] != TRAIN_STATE_ACTIVE)
            continue;
        if (!store_moveValid[i])
            continue;

        for (int j = i + 1; j < numTrains; j++)
        {
            if (trainState[j] != TRAIN_STATE_ACTIVE)
                continue;
            if (!store_moveValid[j])
                continue;

            bool conflict = false;

            //same target collision
            if (store_nextX[i] == store_nextX[j] && store_nextY[i] == store_nextY[j])
            {
                conflict = true;
            }
            //head on swap collision
            else if (store_nextX[i] == train_x[j] && store_nextY[i] == train_y[j] &&
                     store_nextX[j] == train_x[i] && store_nextY[j] == train_y[i])
            {
                conflict = true;
            }

            if (conflict)
            {
                cout << "Conflict: Train " << i << " (Dist " << dist[i] << ") vs Train " << j << " (Dist " << dist[j] << ")" << endl;

               //equal distance crash
                if (dist[i] == dist[j])
                {
                    trainState[i] = TRAIN_STATE_CRASHED;
                    trainState[j] = TRAIN_STATE_CRASHED;
                    store_moveValid[i] = false;
                    store_moveValid[j] = false;
                    crashedCount += 2;
                    cout << "  -> TIE! Both Crashed." << endl;
                }
                //piorty to higher distance
                else if (dist[i] > dist[j])
                {
                    //i is further so i moves and j will wait
                    store_moveValid[j] = false;
                    store_nextX[j] = train_x[j];
                    store_nextY[j] = train_y[j];
                    cout << "  -> Train " << i << " has priority. Train " << j << " waits." << endl;
                }
                else
                {
                    // j is further, so j moves. i waits.
                    store_moveValid[i] = false;
                    store_nextX[i] = train_x[i];
                    store_nextY[i] = train_y[i];
                    cout << "  -> Train " << j << " has priority. Train " << i << " waits." << endl;
                }
            }
        }
    }
}


void checkArrivals()
{
        for (int i = 0; i < numTrains; i++)
    {
        if (trainState[i] == TRAIN_STATE_ACTIVE)
        {
            // chcek for the destination tile
            if (grid[train_y[i]][train_x[i]] == 'D')
            {
                trainState[i] = TRAIN_STATE_ARRIVED;
                arrivedCount++;
                cout << "Train " << i << " Arrived!" << endl;
            }
        }
    }
}



