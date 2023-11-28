#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include "common.h"

#define INITIAL_WIDTH 13
#define INITIAL_HEIGHT 9

// struct for a tile, consists of a type and pointers to adjacent tiles, as well as a weight
//
struct Tile
{
    // true if direction is traversable
    //
    bool upClear;
    bool downClear;
    bool leftClear;
    bool rightClear;
    // true if tile is traversable
    //
    bool traversable;
    // for pathfinding, almost always just 1
    //
    int weight;
    // stores the x and y position of the tile
    //
    int x_pos;
    int y_pos;
    // for use in pathfinding
    //
    struct Tile *parent;
};

// void resetMapData();
void initMap();
void setTileAll(int x_pos, int y_pos, bool up, bool down, bool left, bool right, bool traversable);
void astar(struct Tile *map[INITIAL_HEIGHT][INITIAL_WIDTH], struct Tile *start, struct Tile *end);
void mappingBehaviour(int startingX, int startingY);