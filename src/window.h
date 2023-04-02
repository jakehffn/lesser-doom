#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// GLEW must come before OpenGL
#include <GL\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

typedef struct Window* Window;

Window windowCreate(int width, int height);
void windowDestroy(Window window);
bool windowInit(Window window);
void windowSwap(Window window);