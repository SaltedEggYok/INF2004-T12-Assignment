#include "map.h"
#include "pico/stdlib.h"
#include <string.h>

//create map
int height = INITIAL_HEIGHT;
int width = INITIAL_WIDTH;

//tile pointers
struct Tile* startTile;
struct Tile* currTile;
struct Tile* endTile; 

//create map
struct Tile map[INITIAL_HEIGHT][INITIAL_WIDTH];

//Function to set tile status
void setTile(int x_pos, int y_pos, enum Direction dir, bool traversable) {
    switch(dir){
        case UP:
            map[y_pos][x_pos].upClear = traversable;
            break;
        case DOWN:
            map[y_pos][x_pos].downClear = traversable;
            break;
        case LEFT:
            map[y_pos][x_pos].leftClear = traversable;
            break;
        case RIGHT:
            map[y_pos][x_pos].rightClear = traversable;
            break;
        default:
            //should never enter
            break;
    }
    
    return;
}

//set all 4 directions at once
void setTileAll(int x_pos, int y_pos, bool up, bool down, bool left, bool right) {
    map[y_pos][x_pos].upClear = up;
    map[y_pos][x_pos].downClear = down;
    map[y_pos][x_pos].leftClear = left;
    map[y_pos][x_pos].rightClear = right;

    map[y_pos][x_pos].x_pos = x_pos;
    map[y_pos][x_pos].y_pos = y_pos;
    printf("x: %d, y: %d\n", x_pos, y_pos);
    return;
}

//initializing all values to default values
void initMapData(){
    for(int i = 0 ; i < INITIAL_HEIGHT; i++){
        for(int j = 0; j < INITIAL_WIDTH; j++){
            map[i][j].upClear = false;
            map[i][j].downClear = false;
            map[i][j].leftClear = false;
            map[i][j].rightClear = false;
            map[i][j].weight = 1;
            map[i][j].x_pos = j;
            map[i][j].y_pos = i;
            map[i][j].parent = NULL;
        }
    }   
}

void TEST_MAP_READING() {
    // char cwd[1024];
    // if (getcwd(cwd, sizeof(cwd)) != NULL) {
    //     printf("Current working directory: %s\n", cwd);
    // } else {
    //     perror("getcwd() error");
    // }

    // FILE* file = fopen("map.txt", "r");
    // if (file == NULL) {
    //     perror("Failed to open file");
    //     return;
    // }

    // int fileHeight = 0;  // Number of lines in the file
    // size_t fileWidth = 0;  // Maximum line length

    // //first pass, get width and height************

    // char line[1024]; // Adjust the buffer size as needed
    // while (fgets(line, sizeof(line), file) != NULL) {
    //     fileHeight++;  // Increment the line count

    //     // Get the length of the current line
    //     size_t currentLineLength = strlen(line);

    //     // Remove the newline character, if present
    //     char *newline = strchr(line, '\n');
    //     if (newline) {
    //         *newline = ' ';
    //     }


    //     // Update maxLineLength if necessary
    //     if (currentLineLength > fileWidth) {
    //         fileWidth = currentLineLength;
    //     }
    // }

    // //reset file pointer
    // fseek(file, 0, SEEK_SET);

    // //second pass, read map ************
    // int fileData[fileHeight][fileWidth];

    // for (int i = 0; i < fileHeight; i++) {
    //     for (int j = 0; j < fileWidth; j++) {
    //         char input = fgetc(file);
    //         if (input == ' '){
    //             continue;
    //         }
    //         fileData[i][j] = input;
    //     }
    // }

    //HARDCODED DATA FOR TESTING
    int fileHeight = 21;
    int fileWidth = 21;
    int fileData[21][21] = {
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0},
        {0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0},
        {0,1,0,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0},
        {0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0},
        {0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0},
        {0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0},
        {0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0},
        {0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0},
        {0,1,1,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0},
        {0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0},
        {0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0},
        {0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1,0},
        {0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0},
        {0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0},
        {0,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0},
        {0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0},
        {0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}
    };
    //read map and convert to tile map
    for (int i = 1; i < fileHeight; i+=2) {
        for (int j = 1; j < fileWidth; j+=2) {
            bool up = false;
            bool down = false;
            bool left = false;
            bool right = false;

            if(fileData[i][j-1] == 1){
                left = true;
            }
            if(fileData[i][j+1] == 1){
                right = true;
            }
            if(fileData[i-1][j] == 1){
                up = true;
            }
            if(fileData[i+1][j] == 1){
                down = true;
            }

            setTileAll(j/2, i/2, up, down, left, right);

        }
    }
    

    //print map debug
    // int width = fileWidth/2;
    // int height = fileHeight/2;
    for(int i = 0 ; i < fileHeight/2; i++){
        for(int j = 0; j < fileWidth/2; j++){
            // Define four boolean values
            int bool1 = map[i][j].upClear;
            int bool2 = map[i][j].leftClear;
            int bool3 = map[i][j].rightClear;
            int bool4 = map[i][j].downClear;

            // Convert the booleans to an integer
            int result = (bool1 << 3) | (bool2 << 2) | (bool3 << 1) | bool4;

            if(result < 10){
                printf(" ");
            }
            printf("%d", result);
        }
        printf("\n");
    }

}
// Function to calculate the Manhattan distance between two tiles
int manhattanDistance(struct Tile* a, struct Tile* b) {
    return abs(a->x_pos - b->x_pos) + abs(a->y_pos - b->y_pos);
}

// Function to find the tile with the lowest total cost in the open set
struct Tile* findLowestCostTile(struct Tile* openSet[], int size) {
    struct Tile* lowestCostTile = NULL;
    int lowestCost = 1000000;//arbritary large number

    for (int i = 0; i < size; i++) {
        if (openSet[i]->weight < lowestCost) {
            lowestCost = openSet[i]->weight;
            lowestCostTile = openSet[i];
        }
    }

    return lowestCostTile;
}

// Function to implement A* pathfinding
void astar(struct Tile map[INITIAL_HEIGHT][INITIAL_WIDTH], struct Tile* start, struct Tile* end) {
    int openSetSize = 0;
    struct Tile* openSet[INITIAL_HEIGHT * INITIAL_WIDTH];
    struct Tile* closedSet[INITIAL_HEIGHT][INITIAL_WIDTH];

    for(int i = 0 ; i < INITIAL_HEIGHT; i++){
        for(int j = 0; j < INITIAL_WIDTH; j++){
            closedSet[i][j] = NULL;
        }
    }

    for(int i = 0 ; i < INITIAL_HEIGHT * INITIAL_WIDTH; i++){
        openSet[i] = NULL;
    }

    // Initialize the start tile
    start->weight = manhattanDistance(start, end);
    openSet[openSetSize++] = start;

    struct Tile* current = start;
    while (openSetSize > 0) {

        printf("current: %d, %d\n", current->x_pos, current->y_pos);


        bool bool1 = (current->y_pos - 1 >= 0 && current->upClear == true);
        bool bool2 = (current->y_pos + 1 < INITIAL_HEIGHT && current->downClear == true);
        bool bool3 = (current->x_pos - 1 >= 0 && current->leftClear == true);
        bool bool4 = (current->x_pos + 1 < INITIAL_WIDTH && current->rightClear == true);
        printf("bools: %d, %d, %d, %d\n", bool1, bool2, bool3, bool4);
        // Explore neighbor tiles
        // TODO: Implement logic to check neighboring tiles, calculate weights, and update the path
        struct Tile* neighbors[4] = {
            // Define the four possible neighbor tiles (up, down, left, right)
            bool1 ?  &map[current->y_pos - 1][current->x_pos] : NULL, // Up
            bool2 ?  &map[current->y_pos + 1][current->x_pos]: NULL, // Down
            bool3 ?  &map[current->y_pos][current->x_pos - 1]: NULL, // Left
            bool4 ? &map[current->y_pos][current->x_pos + 1]: NULL // Right
        };
        
        for (int i = 0; i < 4; i++) {
            struct Tile* neighbor = neighbors[i];

            if (neighbor == NULL || closedSet[neighbor->y_pos][neighbor->x_pos] != NULL) {
                // Skip this neighbor if it's already evaluated, not walkable, or outside the map boundaries
                if(neighbor != NULL){
                    printf("closed set has: %d, %d ", neighbor->x_pos, neighbor->y_pos);
                }
                printf("skipped \n");
                continue;
            }

            printf("neighbor: %d, %d\n", neighbor->x_pos, neighbor->y_pos);
            int tentativeGScore = current->weight + 1; // 1 is the cost to move to a neighboring tile (adjust as needed)

            if (tentativeGScore < neighbor->weight || closedSet[neighbor->y_pos][neighbor->x_pos] == NULL) {
                neighbor->parent = current;
                neighbor->weight = tentativeGScore + manhattanDistance(neighbor, end);
                if (closedSet[neighbor->y_pos][neighbor->x_pos] == NULL) {
                    printf("added to open set : %d, %d\n", neighbor->x_pos, neighbor->y_pos);
                    openSet[openSetSize++] = neighbor;
                }
            }
        }

        // Move the current tile from open set to closed set
        for (int i = 0; i < openSetSize; i++) {
            if (openSet[i] == current) {
                openSetSize--;
                openSet[i] = openSet[openSetSize];
                openSet[openSetSize] = NULL;
                printf("removed from open set: %d, %d\n", current->x_pos, current->y_pos);
                printf("replace with: %d, %d\n", openSet[i]->x_pos, openSet[i]->y_pos);
                break;
            }
        }


        closedSet[current->y_pos][current->x_pos] = current;
        printf("added to closed set: %d, %d\n", current->x_pos, current->y_pos);


        // Find the tile with the lowest total cost in the open set
        //struct Tile* prev = current; 
        current = findLowestCostTile(openSet, openSetSize);
        printf("lowest cost tile: %d, %d\n", current->x_pos, current->y_pos);

        if (current->x_pos == end->x_pos && current->y_pos == end->y_pos) {
            // Path found
            // TODO: Trace and print the path
            printf("Path found.\n");
            //current->parent = prev;
            while (current != NULL) {
                printf("(%d, %d) -> ", current->x_pos, current->y_pos);
                current = current->parent;
            }
            return;
        }

    }

    // Path not found
    printf("No path found.\n");
}

void initMap(){
    initMapData();
}

// int main(){

//     stdio_init_all();

//     sleep_ms(10000); //time buffer to start serial monitoring
//     //create map
//     //struct Map* map = createMap(MAZE_WIDTH, MAZE_HEIGHT);
//     //add tiles
//     //addTileToLeft(m
//     initMap();
//     TEST_MAP_READING();

//     astar(map, &map[0][0], &map[9][9]);

//     return 0;
// }