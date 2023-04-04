#pragma once

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

typedef struct World* World;

struct Position {
    double x;
    double y;
};

struct Ray {
    double depth;
    unsigned int color;
    double angle_of_incidence;
};

typedef struct Position Position;
typedef struct Ray Ray;

World worldCreate(char* map, int width, int height, double scale);
Ray worldCastRay(World world, Position position, double ray_angle, double player_angle);
Position worldGetPlayerPosition(World world);