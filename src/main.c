#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

// Glew must come before opengl
#include <GL\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "window.h"
#include "shader.h"

#define WIDTH 300
#define HEIGHT 300

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

Window window = NULL;
Shader shader = NULL;
char texture_data[WIDTH*HEIGHT*4];
GLuint VAO = -1;
GLuint screen_texture = -1;

bool quit = false;
SDL_Event event;

bool keydown_w = false;
bool keydown_a = false;
bool keydown_s = false;
bool keydown_d = false;

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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
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

void render() {

    // shader = createShader(vertex_shader_source, fragment_shader_source);
    // compileShader(shader);
    glClear(GL_COLOR_BUFFER_BIT);
    useShader(shader);

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // glBindVertexArray(0);
    // glUseProgram(0);
    swapWindow(window);

    // glBindTexture(GL_TEXTURE_2D, 0); 
}

void genImage(int t) {

    for (int xx = 0; xx < WIDTH; xx++) {

        for (int yy = 0; yy < HEIGHT; yy++) {

            int cell_start = (yy*WIDTH + xx)*3;

            texture_data[cell_start] = 0x00;
            texture_data[cell_start + 1] = 0x00;
            texture_data[cell_start + 2] = 0x00;

            if ((xx+t)/8 % 3 == 0) {
                texture_data[cell_start] = 0xFF;
            }

            if ((xx+t)/8 % 3 == 1) {
                texture_data[cell_start+1] = 0xFF;
            }

            if ((xx+t)/8 % 3 == 2) {
                texture_data[cell_start+2] = 0xFF;
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

    // glBindTexture(GL_TEXTURE_2D, 0); 
}

void pollEvents() {

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                // User clicked the close button, exit loop
                printf("Quit event received\n");
                quit = true;
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

    window = createWindow(WIDTH, HEIGHT);
    int t = 0;

    if (!initWindow(window)) {

        printf("Failed to initialize SDL\n");

    } else {

        printf("SDL initialized\n");

        initShader(&shader, vertex_shader_source, fragment_shader_source);
        createScreenTexture();

        glClearColor(0.5, 0.2, 0.5, 1.0);
        createQuadVAO();

        while (!quit) {

            // Poll for events
            pollEvents();

            if (keydown_a) {
                t += 8;
            }

            if (keydown_d) {
                t -= 8;
            }

            genImage(t);
            render();
        }
    }  

    glDeleteBuffers(1, &VAO);
    glDeleteTextures(1, &screen_texture);

    destroyWindow(window);
    destroyShader(shader);

    // free(shader);
    // free(window);

    return 0;
}