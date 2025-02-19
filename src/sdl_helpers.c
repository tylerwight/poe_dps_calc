#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>


SDL_Window* sdl_setup(char *name, int size_x, int size_y, Uint32 flags){
    SDL_Window *window = NULL;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("failed to init: %s", SDL_GetError());
        return -1;
    }
 

    
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);


    // and prepare OpenGL stuff
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    window = SDL_CreateWindow(name, 0, 0, size_x, size_y, flags);
    if (window == NULL) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return -1;
    }

    return window;
}



bool sdl_main_loop(SDL_Window *window){
        SDL_Event e;
        bool quit = false;
        while (SDL_PollEvent(&e) != 0){
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT)
            quit = true;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(window))
            quit = true;
        }
    return quit;
}

