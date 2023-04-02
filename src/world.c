#include "world.h"
#include <stdio.h>

struct World {
    char* map;
    int map_width;
    int map_height;
    float scale;
};


World worldCreate(char* map, int width, int height, float scale) {

    World world = malloc(sizeof(struct World));

    world->map = map;
    world->map_width = width;
    world->map_height = height;
    world->scale = scale;

    return world;
}

Ray worldCastRay(World world, Position position, float ray_angle, float player_angle) {

    Ray ray;

    int map_x;
    int map_y;

    float march_x = 0;
    float march_y = 0;

    float delta_x = -sin(ray_angle);
    float delta_y = cos(ray_angle);

    char curr;

    // printf("%f %f\n", delta_x, delta_y);

    do {
        march_x += delta_x;
        march_y += delta_y;

        map_x = (int) ((position.x+march_x)/world->scale);
        map_y = (int) ((position.y+march_y)/world->scale);

        if (map_x < 0 || map_x >= world->map_width || map_y < 0 || map_y >= world->map_height) {

            ray.depth = 1000000;
            ray.color = 0x000000;
            
            return ray;
        }

        curr = world->map[map_y*world->map_width + map_x];

    } while (curr == ' ' || curr == 'P');

    switch(curr) {
        case 'r':
            ray.color = 0xFF2222;
            break;
        case 'g':
            ray.color = 0x22FF22;
            break;
        case 'b':
            ray.color = 0x2222FF;
            break;

        default:
            ray.color = 0x222222;
    }

    float hypotenuse = sqrt(march_x*march_x + march_y*march_y)/world->scale;
    ray.depth = sin(M_PI_2 - fabs(player_angle - ray_angle))*hypotenuse;
    
    return ray;
}

Position worldGetPlayerPosition(World world) {

    Position position;

    for (int y = 0; y < world->map_height; y++) {
        for (int x = 0; x < world->map_width; x++) {

            if (world->map[y*world->map_width + x] == 'P') {

                position.x = x*world->scale;
                position.y = y*world->scale;

                return position;
            }
        }
    }

    position.x = -1;
    position.y = -1;

    return position;
}