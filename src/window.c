#include "window.h"

struct Window {

    int width;
    int height;

    SDL_Window* window;
    SDL_GLContext context;

};

Window createWindow(int width, int height) {

    Window window = malloc(sizeof(Window));

    window->height = height;
    window->width = width;

    return window;
}

void destroyWindow(Window window) {
    
	SDL_DestroyWindow(window->window);
	window->window = NULL;

	// Quit SDL subsystems
	SDL_Quit();
	// free(window);
}

bool initWindow(Window window) {

    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;

	} else {

        #ifndef __EMSCRIPTEN__

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

        #endif

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		window->window = SDL_CreateWindow(
            "AGE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			window->width, window->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);

		if(window->window == NULL) {

			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;

		} else {

            window->context = SDL_GL_CreateContext(window->window);

			if(window->context == NULL) {
				printf("SDL: OpenGL context could not be created!\nSDL Error: %s\n", SDL_GetError());
				success = false;
			} else {

                #ifndef __EMSCRIPTEN__

                    glewExperimental = GL_TRUE; 
                    GLenum glewError = glewInit();
                    if( glewError != GLEW_OK ) {
                        printf("GLEW: Error initializing! %s\n", glewGetErrorString(glewError));
                    }

                #endif

				// Use Vsync
				if( SDL_GL_SetSwapInterval( 2 ) < 0 ) {
					printf("SDL: Warning: Unable to set VSync!\nSDL Error: %s\n", SDL_GetError());
				}
			}
		}
	}

	return success;
}

void swapWindow(Window window) {
    SDL_GL_SwapWindow(window->window);
}