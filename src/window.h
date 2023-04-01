#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// GLEW must come before OpenGL
#include <GL\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

typedef struct Window* Window;

Window createWindow(int width, int height);
void destroyWindow(Window window);
bool initWindow(Window window);
void swapWindow(Window window);