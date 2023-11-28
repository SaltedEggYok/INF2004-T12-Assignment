#include "map.h"
#include "pico/stdlib.h"
#include <string.h>

// variables
// create map
struct Tile map[INITIAL_HEIGHT][INITIAL_WIDTH];
// holds the tiles that have been visited for pathfinding
struct Tile *visitedMap[INITIAL_HEIGHT][INITIAL_WIDTH];
// holds the final path after pathfinding
struct Tile *finalPath[INITIAL_HEIGHT * INITIAL_WIDTH];

// set all relevant information at once
/*
    @brief: Sets the status of a tile in the map
    @param: x_pos: x position of the tile
            y_pos: y position of the tile
            up: true if tile above is traversable, false if not
            down: true if tile below is traversable, false if not
            left: true if tile to the left is traversable, false if not
            right: true if tile to the right is traversable, false if not
            traversable: true if tile itself is traversable, false if not
    @return: None
*/
void setTileAll(int x_pos, int y_pos, bool up, bool down, bool left, bool right, bool traversable)
{
    map[y_pos][x_pos].upClear = up;
    map[y_pos][x_pos].downClear = down;
    map[y_pos][x_pos].leftClear = left;
    map[y_pos][x_pos].rightClear = right;
    map[y_pos][x_pos].traversable = traversable;

    map[y_pos][x_pos].x_pos = x_pos;
    map[y_pos][x_pos].y_pos = y_pos;
    printf("x: %d, y: %d \n", x_pos, y_pos);
    return;
}

// initializing all values to default values
/*
    @brief: Resets the map data to default values
    @param: None
    @return: None
*/
void resetMapData()
{
    // initialize map, set all values to default
    //
    for (int i = 0; i < INITIAL_HEIGHT; i++)
    {
        for (int j = 0; j < INITIAL_WIDTH; j++)
        {
            map[i][j].upClear = false;
            map[i][j].downClear = false;
            map[i][j].leftClear = false;
            map[i][j].rightClear = false;
            map[i][j].weight = 1;
            map[i][j].x_pos = j;
            map[i][j].y_pos = i;
            map[i][j].parent = NULL;
            map[i][j].traversable = false;
        }
    }

    // initialize visited map, set all pointers to NULL
    //
    for (int i = 0; i < INITIAL_HEIGHT; i++)
    {
        for (int j = 0; j < INITIAL_WIDTH; j++)
        {
            visitedMap[i][j] = NULL;
        }
    }
}

/*
    @brief: Loads the map data, and converts it to a tile map
            Hardcoded for this project
    @param: None
    @return: None
*/
void loadMap()
{
    // final map data
    //
    int fileHeight = 9;
    int fileWidth = 13;
    int fileData[9][13] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
        {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
        {0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    // read map and convert to tile map
    //
    for (int i = 0; i < fileHeight; i++)
    {
        for (int j = 0; j < fileWidth; j++)
        {
            // printf("%d", fileData[i][j]); //DEBUG

            // store data into memory, 1 = traversable, 0 = not traversable
            //
            bool up = false;
            bool down = false;
            bool left = false;
            bool right = false;
            bool traversable = false;

            // keep within edges
            //
            if (j - 1 >= 0)
            {
                if (fileData[i][j - 1] == 1)
                {
                    left = true;
                }
            }
            if (j + 1 < fileWidth)
            {
                if (fileData[i][j + 1] == 1)
                {
                    right = true;
                }
            }
            if (i - 1 >= 0)
            {
                if (fileData[i - 1][j] == 1)
                {
                    up = true;
                }
            }
            if (i + 1 < fileHeight)
            {
                if (fileData[i + 1][j] == 1)
                {
                    down = true;
                }
            }

            // set if tile is traversable
            traversable = fileData[i][j] == 1 ? true : false;

            setTileAll(j, i, up, down, left, right, traversable);
        }
    }

    // print map debug
    printf("Raw Map Data \n");
    for (int i = 0; i < fileHeight; i++)
    {
        for (int j = 0; j < fileWidth; j++)
        {
            printf("%d ", map[i][j].traversable ? 1 : 0);
        }
        printf("\n\n");
    }

    // print traversal map debug
    printf("Traversal Map Data \n");

    for (int i = 0; i < fileHeight; i++)
    {
        for (int j = 0; j < fileWidth; j++)
        {
            // Define four boolean values
            //
            int bool1 = map[i][j].upClear;
            int bool2 = map[i][j].leftClear;
            int bool3 = map[i][j].rightClear;
            int bool4 = map[i][j].downClear;

            // Convert the booleans to an integer
            //
            int result = (bool1 << 3) | (bool2 << 2) | (bool3 << 1) | bool4;

            if (result < 10)
            {
                printf(" ");
            }
            printf("%d ", result);
        }
        printf("\n\n");
    }
}

/*
    @brief: Calculates the Manhattan distance between two tiles
            For use with A* pathfinding
    @param: a: pointer to first tile
            b: pointer to second tile
    @return: Manhattan distance between the two tiles
*/
int manhattanDistance(struct Tile *a, struct Tile *b)
{
    return abs(a->x_pos - b->x_pos) + abs(a->y_pos - b->y_pos);
}

/*
    @brief: Finds the tile with the lowest total cost in the open set
            For use with A* pathfinding
    @param: openSet: array of pointers to tiles in the open set
            size: size of the open set
    @return: pointer to the tile with the lowest total cost in the open set
*/
struct Tile *findLowestCostTile(struct Tile *openSet[], int size)
{
    struct Tile *lowestCostTile = NULL;
    int lowestCost = 1000000; // arbritary large number

    for (int i = 0; i < size; i++)
    {
        if (openSet[i]->weight < lowestCost)
        {
            lowestCost = openSet[i]->weight;
            lowestCostTile = openSet[i];
        }
    }

    return lowestCostTile;
}

// Function to implement A* pathfinding
/*
    @brief: Implements the A* pathfinding algorithm, using BFS
    @param: map: 2d array of pointers to tiles
            start: pointer to the starting tile
            end: pointer to the ending tile
    @return: None
            Prints the path from start to end
            Pushes the path into the finalPath array, treated as stack
*/
void astar(struct Tile *map[INITIAL_HEIGHT][INITIAL_WIDTH], struct Tile *start, struct Tile *end)
{
    // check if tiles are valid
    //
    if (start == NULL || end == NULL)
    {
        printf("Invalid start or end tile.\n");
        return;
    }
    if (start->traversable == false)
    {
        printf("Start tile is not traversable.\n");
        return;
    }
    if (end->traversable == false)
    {
        printf("End tile is not traversable.\n");
        return;
    }

    int openSetSize = 0;
    // open set is a list of tiles to be evaluated, for BFS
    //
    struct Tile *openSet[INITIAL_HEIGHT * INITIAL_WIDTH];
    // closed set is a list of tiles that have been evaluated, for BFS
    //
    struct Tile *closedSet[INITIAL_HEIGHT][INITIAL_WIDTH];

    // initialize open and closed sets
    //
    for (int i = 0; i < INITIAL_HEIGHT; i++)
    {
        for (int j = 0; j < INITIAL_WIDTH; j++)
        {
            closedSet[i][j] = NULL;
        }
    }
    for (int i = 0; i < INITIAL_HEIGHT * INITIAL_WIDTH; i++)
    {
        openSet[i] = NULL;
    }

    // Initialize the start tile
    //
    start->weight = manhattanDistance(start, end);
    // set 1st tile to start, and increment size after setting
    //
    openSet[openSetSize++] = start;

    struct Tile *current = start;
    while (openSetSize > 0)
    {

        // printf("current: %d, %d\n", current->x_pos, current->y_pos); //DEBUG

        // booleans to check if neighbouring tiles are valid
        //
        bool bool1 = (current->y_pos - 1 >= 0 && current->upClear == true);
        bool bool2 = (current->y_pos + 1 < INITIAL_HEIGHT && current->downClear == true);
        bool bool3 = (current->x_pos - 1 >= 0 && current->leftClear == true);
        bool bool4 = (current->x_pos + 1 < INITIAL_WIDTH && current->rightClear == true);
        // printf("bools: %d, %d, %d, %d\n", bool1, bool2, bool3, bool4); //DEBUG

        // Explore neighbor tiles
        // get the four possible neighbouring tiles (up, down, left, right)
        //
        struct Tile *neighbors[4] = {
            bool1 ? map[current->y_pos - 1][current->x_pos] : NULL, // Up
            bool2 ? map[current->y_pos + 1][current->x_pos] : NULL, // Down
            bool3 ? map[current->y_pos][current->x_pos - 1] : NULL, // Left
            bool4 ? map[current->y_pos][current->x_pos + 1] : NULL  // Right
        };

        // process neighbours
        //
        for (int i = 0; i < 4; i++)
        {
            struct Tile *neighbor = neighbors[i];

            // Skip this neighbor if it's already evaluated, not walkable, or outside the map boundaries
            //
            if (neighbor == NULL || closedSet[neighbor->y_pos][neighbor->x_pos] != NULL)
            {
                // if (neighbor != NULL) //DEBUG
                // {
                //     printf("closed set has: %d, %d ", neighbor->x_pos, neighbor->y_pos);
                // }
                // printf("skipped \n"); //DEBUG
                continue;
            }

            // printf("neighbor: %d, %d\n", neighbor->x_pos, neighbor->y_pos); //DEBUG
            // 1 is the cost to move to a neighboring tile (adjust as needed)
            // weight cumutatively will just be number of tiles from start to "this" tile
            //
            int tentativeGScore = current->weight + 1;

            // if neighbour is not in closed set, process it
            //
            if (tentativeGScore < neighbor->weight || closedSet[neighbor->y_pos][neighbor->x_pos] == NULL)
            {
                // set parent for path retracing
                //
                neighbor->parent = current;
                // set weight
                //
                neighbor->weight = tentativeGScore + manhattanDistance(neighbor, end);
                if (closedSet[neighbor->y_pos][neighbor->x_pos] == NULL) // safety
                {
                    // printf("added to open set : %d, %d\n", neighbor->x_pos, neighbor->y_pos); //DEBUG
                    // add to open set if not in closed set / not processed yet
                    openSet[openSetSize++] = neighbor;
                }
            }
        }

        // Move the current tile from open set to closed set
        //
        for (int i = 0; i < openSetSize; i++)
        {
            if (openSet[i] == current)
            {
                openSetSize--;
                openSet[i] = openSet[openSetSize];
                openSet[openSetSize] = NULL;
                // printf("removed from open set: %d, %d\n", current->x_pos, current->y_pos); //DEBUG
                // printf("replace with: %d, %d\n", openSet[i]->x_pos, openSet[i]->y_pos); //DEBUG
                break;
            }
        }

        // Add the current tile to the closed set
        //
        closedSet[current->y_pos][current->x_pos] = current;
        // printf("added to closed set: %d, %d\n", current->x_pos, current->y_pos); //DEBUG

        // Find the tile with the lowest total cost in the open set and set to traverse next
        //
        current = findLowestCostTile(openSet, openSetSize);
        // printf("lowest cost tile: %d, %d\n", current->x_pos, current->y_pos); //DEBUG

        // Check if the current tile is the end tile
        //
        if (current->x_pos == end->x_pos && current->y_pos == end->y_pos)
        {
            // Path found
            //
            printf("Path found.\n");

            // print path, starting from end
            //
            while (current != NULL)
            {
                // Add current tile to path
                // push into stack for traversal
                //
                for (int i = INITIAL_HEIGHT * INITIAL_WIDTH; i > 0; --i)
                {
                    finalPath[i] = finalPath[i - 1];
                }
                finalPath[0] = current;

                printf("(%d, %d) -> ", current->x_pos, current->y_pos);
                current = current->parent;
            }
            // exit function when path found
            //
            return;
        }
    }

    // Path not found
    //
    printf("No path found.\n");
}

/*
    @brief: Initializes the map
            For main.c to call, for consistency
    @param: None
    @return: None
*/
void initMap()
{
    resetMapData();
}

/*
    @brief: Prints the visited map
            For debugging purposes
    @param: None
    @return: None
*/
void printVisitedMap()
{
    // print visited map
    //
    printf("\n");
    for (int i = 0; i < INITIAL_HEIGHT; i++)
    {
        for (int j = 0; j < INITIAL_WIDTH; j++)
        {
            if (visitedMap[i][j] == NULL)
            {
                printf("  ");
            }
            else
            {
                printf("%d ", visitedMap[i][j]->traversable ? 1 : 0);
            }
        }
        printf("\n\n");
    }
}

/*
    @brief: Implements the mapping behaviour
    @param: startingX: x position of the starting tile
            startingY: y position of the starting tile
    @return: None
            Prints the visited map after each step
*/
void mappingBehaviour(int startingX, int startingY)
{
    // original map is just a 2d array of tiles on the edges
    // starting position is on 1 tile on the edge
    // ending position is on 1 tile on the edge
    // car will start at starting position
    // perform DFS to find all tiles
    // after mapping all tiles, perform A* back to starting position
    // car will then follow the path back to starting position
    // perform A* again to find path to ending position
    // car will then follow the path to ending position

    // tiles to visit, for DFS algorithm
    //
    struct Tile *tilesToVisit[INITIAL_HEIGHT * INITIAL_WIDTH];

    // initialize visited map
    //
    for (int i = 0; i < INITIAL_HEIGHT; i++)
    {
        for (int j = 0; j < INITIAL_WIDTH; j++)
        {
            visitedMap[i][j] = NULL;
        }
    }

    // starting position
    //
    int currXPos = startingX;
    int currYPos = startingY;

    // add starting tile to tiles to visit
    //
    tilesToVisit[0] = &map[currYPos][currXPos];

    // DFS algorithm
    // check if there are tiles to visit
    //
    while (tilesToVisit[0] != NULL)
    {
        // general algorithm
        // check if tile is visited
        // if not visited, visit tile
        // if visited, move to next tile
        // if no more tiles, stop

        // process tile
        // move to tile
        //
        currXPos = tilesToVisit[0]->x_pos;
        currYPos = tilesToVisit[0]->y_pos;

        // remove tile from list, pop stack
        //
        for (int i = 0; i < INITIAL_HEIGHT * INITIAL_WIDTH; ++i)
        {
            if (tilesToVisit[i] == NULL)
            {
                break;
            }
            tilesToVisit[i] = tilesToVisit[i + 1];
        }

        // check 4 directions, update visitedmap
        // check left
        // if there is a tile to the left, and has not been visited
        //
        if (currXPos - 1 >= 0 && visitedMap[currYPos][currXPos - 1] == NULL)
        {
            // if tile is traversable
            //
            if (visitedMap[currYPos][currXPos]->leftClear == true)
            {
                // add to tiles to visit
                // push tile into stack
                //
                for (int i = INITIAL_HEIGHT * INITIAL_WIDTH; i > 0; --i)
                {
                    if (tilesToVisit[i - 1] == NULL)
                    {
                        continue;
                    }
                    tilesToVisit[i] = tilesToVisit[i - 1];
                }
                tilesToVisit[0] = &map[currYPos][currXPos - 1];
            }
            // if tile is not traversable/wall, do nothing

            // update visited map
            //
            visitedMap[currYPos][currXPos - 1] = &map[currYPos][currXPos - 1];
        }

        // check up
        // if there is a tile above, and has not been visited
        //
        if (currYPos - 1 >= 0 && visitedMap[currYPos - 1][currXPos] == NULL)
        {
            // if tile is traversable
            //
            if (visitedMap[currYPos][currXPos]->upClear == true)
            {
                // add to tiles to visit
                // push tile into stack
                //
                for (int i = INITIAL_HEIGHT * INITIAL_WIDTH; i > 0; --i)
                {
                    if (tilesToVisit[i - 1] == NULL)
                    {
                        continue;
                    }
                    tilesToVisit[i] = tilesToVisit[i - 1];
                }
                tilesToVisit[0] = &map[currYPos - 1][currXPos];
            }
            // if tile is not traversable/wall, do nothing

            // update visited map
            //
            visitedMap[currYPos - 1][currXPos] = &map[currYPos - 1][currXPos];
        }

        // check right
        // if there is a tile to the right, and has not been visited
        //
        if (currXPos + 1 < INITIAL_WIDTH && visitedMap[currYPos][currXPos + 1] == NULL)
        {
            // if tile is traversable
            //
            if (visitedMap[currYPos][currXPos]->rightClear == true)
            {
                // add to tiles to visit
                // push tile into stack
                //
                for (int i = INITIAL_HEIGHT * INITIAL_WIDTH; i > 0; --i)
                {
                    if (tilesToVisit[i - 1] == NULL)
                    {
                        continue;
                    }
                    tilesToVisit[i] = tilesToVisit[i - 1];
                }
                tilesToVisit[0] = &map[currYPos][currXPos + 1];
            }
            // if tile is not traversable/wall, do nothing

            // update visited map
            //
            visitedMap[currYPos][currXPos + 1] = &map[currYPos][currXPos + 1];
        }

        // check down
        // if there is a tile below, and has not been visited
        //
        if (currYPos + 1 < INITIAL_HEIGHT && visitedMap[currYPos + 1][currXPos] == NULL)
        {
            // if tile is traversable
            //
            if (visitedMap[currYPos][currXPos]->downClear == true)
            {
                // add to tiles to visit
                // push tile into stack
                //
                for (int i = INITIAL_HEIGHT * INITIAL_WIDTH; i > 0; --i)
                {
                    if (tilesToVisit[i - 1] == NULL)
                    {
                        continue;
                    }
                    tilesToVisit[i] = tilesToVisit[i - 1];
                }
                tilesToVisit[0] = &map[currYPos + 1][currXPos];
            }
            // if tile is not traversable/wall, do nothing

            // update visited map
            //
            visitedMap[currYPos + 1][currXPos] = &map[currYPos + 1][currXPos];
        }

        // print visited map after each step
        //
        printVisitedMap();
        // short sleep
        //
        sleep_ms(250);
    }
}

/*
    @brief: Prints the final path
            For debugging purposes
    @param: None
    @return: None
*/
void printFinalPath()
{
    // Loops through the final path and prints the map at each step

    for (int i = 0;; ++i)
    {
        struct Tile *tile = finalPath[i];
        if (tile == NULL)
        {
            printf("End of path\n");
            break;
        }
        printf("\n");
        for (int i = 0; i < INITIAL_HEIGHT; i++)
        {
            for (int j = 0; j < INITIAL_WIDTH; j++)
            {
                if (visitedMap[i][j] == NULL)
                {
                    printf("  ");
                }
                else
                {
                    if (i == tile->y_pos && j == tile->x_pos)
                    {
                        // X denotes the "traveler"
                        //
                        printf("X ");
                    }
                    else
                    {
                        // else print the rest of the maze
                        //
                        printf("%d ", visitedMap[i][j]->traversable ? 1 : 0);
                    }
                }
            }
            printf("\n\n");
        }
        printf("\n");
        // short sleep
        //
        sleep_ms(250);
    }
}

int main()
{
    stdio_init_all();

    sleep_ms(10000); // time buffer to start serial monitoring

    resetMapData();

    loadMap();

    mappingBehaviour(0, 5);

    astar(visitedMap, visitedMap[5][0], visitedMap[3][12]);

    printFinalPath();

    return 0;
}