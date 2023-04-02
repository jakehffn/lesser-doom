#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>

// Glew must come before opengl
#include <GL\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "window.h"
#include "shader.h"
#include "world.h"

#define WIDTH 800
#define HEIGHT 600

const char* vertex_shader_source = "#version 300 es\n"
    "precision highp float;\n"
    "layout (location = 0) in vec4 vertex;\n"  // <vec2 position, vec2 texCoords>
    "out vec2 TexCoords;\n"
    "void main() {\n"
        "TexCoords = vertex.zw;\n"
        "gl_Position = vec4(2.0f*vertex.x - 1.0f, 1.0f - 2.0f*vertex.y, 0.0, 1.0);\n"
    "}\0";

const char* fragment_shader_source = "#version 300 es\n"
    "precision highp float;\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D screenTexture;\n"
    "void main() {\n"
        "color = vec4(texture(screenTexture, vec2(TexCoords.x, TexCoords.y)).rgb, 1.0f);\n"
        // "color = vec4(1.0f*TexCoords.x, 0.6f*TexCoords.y, 1.0f, 1.0f);\n"
    "}\0";

const float quadVertexData[] = { 
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

World world;
char map[] = 
    "rrrrrrrrrrrrrrrrrrrrrrrrrrrrrr"
    "b                 g          g"
    "b                 g          g"
    "b   P             g          g"
    "b                 g          g"
    "b                 g          g"
    "b                 g          g"
    "b     ggggggggggggg          g"
    "b                 g          g"
    "b  r              g          g"
    "b                 g          g"
    "b                            g"
    "b                            g"
    "b    gb                     bg"
    "b                            g"
    "b                            g"
    "b                            g"
    "b                            g"
    "b#############################";

const int world_width = 30;
const int world_height = 19;
const float world_scale = 10;

Position player_position;
float player_angle = 0;
float player_speed = 50;
float mouse_sensitivity = 20;
float fov = 120;

Window window = NULL;
Shader shader = NULL;
char texture_data[WIDTH*HEIGHT*3];
GLuint vao = -1;
GLuint screen_texture = -1;

int window_width = WIDTH;
int window_height = HEIGHT;

bool quit = false;
SDL_Event event;

bool keydown_w = false;
bool keydown_a = false;
bool keydown_s = false;
bool keydown_d = false;
int mouse_move_x = 0;

void createScreenTexture() {

    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

    glBindTexture(GL_TEXTURE_2D, 0); 
}

void createQuadVAO() {

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // The verticies will never change so the buffer ID is not saved
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // Free bound buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void renderScene() {

    float angle_delta = (fov/ 180 * M_PI)/WIDTH;
    float half_fov = (fov/ 180 * M_PI)/2;

    for (int x = 0; x < WIDTH; x++) {

        Ray ray = worldCastRay(world, player_position, player_angle + angle_delta*x - half_fov, player_angle);

        if (x == WIDTH/2) {
            printf("depth %f\n", ray.depth);
        }

        int wall_scaling = 3;
        int wall_height = (int) (((float) HEIGHT / (ray.depth))*wall_scaling);

        for (int y = 0; y < HEIGHT; y++) {

            if (y > (HEIGHT-wall_height)/2 && y < wall_height+(HEIGHT-wall_height)/2) {

                texture_data[(y*WIDTH + x)*3 + 0] = ray.color >> 16;
                texture_data[(y*WIDTH + x)*3 + 1] = ray.color >> 8;
                texture_data[(y*WIDTH + x)*3 + 2] = ray.color;
            } else {
                texture_data[(y*WIDTH + x)*3 + 0] = 0x20;
                texture_data[(y*WIDTH + x)*3 + 1] = 0x20;
                texture_data[(y*WIDTH + x)*3 + 2] = 0x20;
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, screen_texture); 
    
    glTexSubImage2D(
        GL_TEXTURE_2D, 
        0, 
        0, 
        0, 
        WIDTH, 
        HEIGHT,
        GL_RGB, 
        GL_UNSIGNED_BYTE,
        texture_data
    );
}

void render() {

    renderScene();

    // shader = createShader(vertex_shader_source, fragment_shader_source);
    // compileShader(shader);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderUse(shader);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // glBindVertexArray(0);
    // glUseProgram(0);
    windowSwap(window);

    // glBindTexture(GL_TEXTURE_2D, 0); 
}

void updatePlayer(uint64_t delta) {

    player_angle += mouse_move_x/1000.0 * mouse_sensitivity;

    float x_fraction;
    float y_fraction;
    float mult = (delta/1000.0)*player_speed;

    if ((keydown_w != keydown_s)) {
        
        mult = keydown_s ? mult*-1 : mult;

        x_fraction = -sin(player_angle);
        y_fraction = cos(player_angle);


        player_position.x += x_fraction*mult;
        player_position.y += y_fraction*mult;
    }
}

void pollEvents() {

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                // User clicked the close button, exit loop
                printf("Quit event received\n");
                quit = true;
                break;

            case SDL_WINDOWEVENT:

                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    glViewport(0, 0, event.window.data1, event.window.data2);
                }
                break;
            case SDL_MOUSEMOTION:

                mouse_move_x = event.motion.xrel;
                break;

            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {

                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_w:
                        keydown_w = true;
                        break;

                    case SDLK_a:
                        keydown_a = true;
                        break;

                    case SDLK_s:
                        keydown_s = true;
                        break;

                    case SDLK_d:
                        keydown_d = true;
                        break;
                }
                break;

            case SDL_KEYUP:
                switch(event.key.keysym.sym) {

                    case SDLK_w:
                        keydown_w = false;
                        break;

                    case SDLK_a:
                        keydown_a = false;
                        break;

                    case SDLK_s:
                        keydown_s = false;
                        break;

                    case SDLK_d:
                        keydown_d = false;
                        break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                // User clicked the mouse button
                printf("Mouse button %d clicked at (%d,%d)\n", event.button.button, event.button.x, event.button.y);
                break;

            default:
                // Ignore other events
                break;
        }
    }
}

int main(int argv, char** args) {

    window = windowCreate(WIDTH, HEIGHT);
    int t = 0;

    world = worldCreate(map, world_width, world_height, world_scale);
    player_position = worldGetPlayerPosition(world);

    if (!windowInit(window)) {

        printf("Failed to initialize SDL\n");

    } else {

        printf("SDL initialized\n");

        shaderInit(&shader, vertex_shader_source, fragment_shader_source);
        createScreenTexture();

        glClearColor(0.5, 0.2, 0.5, 1.0);
        createQuadVAO();

        uint64_t last_frame = SDL_GetTicks64();
        uint64_t current_frame = SDL_GetTicks64();
        uint64_t delta_time = 0;

        while (!quit) {

            current_frame = SDL_GetTicks64();
            delta_time = current_frame - last_frame;
            last_frame = current_frame;

            // Poll for events
            pollEvents();
            updatePlayer(delta_time);

            render();
            printf("%f %f %f\n", player_position.x, player_position.y, player_angle);

            mouse_move_x = 0;
        }
    }  

    glDeleteBuffers(1, &vao);
    glDeleteTextures(1, &screen_texture);

    windowDestroy(window);
    shaderDestroy(shader);

    return 0;
}