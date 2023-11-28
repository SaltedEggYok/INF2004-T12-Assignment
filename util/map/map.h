#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include "common.h"

#define INITIAL_WIDTH 13
#define INITIAL_HEIGHT 9

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

//struct for a tile, consists of a type and pointers to adjacent tiles, as well as a weight
struct Tile {
    //true if direction is traversable
    bool upClear; 
    bool downClear;
    bool leftClear;
    bool rightClear;
    int weight; //for pathfinding, almost always just 1
    int x_pos;
    int y_pos;
    bool traversable;
    struct Tile* parent; //for use in pathfinding
};



//void resetMapData();
void setTile(int x_pos, int y_pos, enum Direction dir, bool traversable);
void setTileAll(int x_pos, int y_pos, bool up, bool down, bool left, bool right, bool traversable);
void astar(struct Tile *map[INITIAL_HEIGHT][INITIAL_WIDTH], struct Tile* start, struct Tile* end);


void mappingBehaviour(int startingX, int startingY);
void initMap();