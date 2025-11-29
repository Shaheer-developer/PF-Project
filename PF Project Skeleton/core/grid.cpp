#include "grid.h"
#include "simulation_state.h"

// checking if position is inside grid
bool isInBounds(int x, int y)
{
    return (x >= 0 && x < cols && y >= 0 && y < rows);
}

// checking for a track tile
bool isTrackTile(char t)
{
    if (t == '=' || t == '+' || t == '|')
        return true;
}

// checking for switch tile
bool isSwitchTile(char t)
{
    return (t >= 'A' && t <= 'Z');
}

// checking whic swtich is it
int getSwitchIndex(char t)
{
    if (t >= 'A' && t <= 'Z')
        return t - 'A';

    return -1;
}

// checking for spawn point
bool isSpawnPoint(int x, int y)
{
    if (!isInBounds(x, y))
        return false;
    return (grid[y][x] == 'S');
}

// checking for destiination point
bool isDestinationPoint(int x, int y)
{
    if (!isInBounds(x, y))
        return false;
    return (grid[y][x] == 'D');
}

// toggling safety tile
bool toggleSafetyTile(int x, int y)
{
    if (!isInBounds(x, y))
        return false;

    safetyTiles[y][x] = !safetyTiles[y][x];
    return true;
}
