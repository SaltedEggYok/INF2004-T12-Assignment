#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"

#define INITIAL_WIDTH 20
#define INITIAL_HEIGHT 20

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
    int weight; //unsure of this yet
    int x_pos;
    int y_pos;
    struct Tile* parent; //for use in pathfinding
};



//void initMapData();
void setTile(int x_pos, int y_pos, enum Direction dir, bool traversable);
void setTileAll(int x_pos, int y_pos, bool up, bool down, bool left, bool right);
void astar(struct Tile map[INITIAL_HEIGHT][INITIAL_WIDTH], struct Tile* start, struct Tile* end);

void initMap();