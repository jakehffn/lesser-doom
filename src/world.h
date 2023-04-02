#pragma once

#include <stdlib.h>
#include <math.h>

typedef struct World* World;

struct Position {
    float x;
    float y;
};

struct Ray {

    float depth;
    unsigned int color;
};

typedef struct Position Position;
typedef struct Ray Ray;

World worldCreate(char* map, int width, int height, float scale);
Ray worldCastRay(World world, Position position, float angle);
Position worldGetPlayerPosition(World world);