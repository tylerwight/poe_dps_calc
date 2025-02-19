#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"
#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "sdl_helpers.c"


void dps_cleanup( SDL_Window *window, SDL_GLContext *context);
void dps_render(SDL_Window *window, SDL_GLContext *context, ImGuiIO *ioptr, ImVec4 *clear_color);
void dps_config_window(ImGuiIO *ioptr, char *input_buffer, float *calculated_dps);
void dsp_start_frame();

int main(int argc, char* argv[]){
    //setup SDL and OpenGL
    SDL_Window *window = sdl_setup("PoE DPS Calculator", 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    const char* glsl_version = "#version 130";
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);  // enable vsync
    SDL_Log("opengl version: %s", (char*)glGetString(GL_VERSION));


    // setup imgui
    igCreateContext(NULL);
    ImGuiIO* ioptr = igGetIO();
    ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    igStyleColorsDark(NULL);


    //Application vars
    ImVec4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};
    char input_buffer[5000] = {0};
    float calculated_dps = 0.0f;


    //main loop
    bool quit = false;
    while (!quit){
        quit = sdl_main_loop(window);

        // start imgui frame
        dsp_start_frame();

        //application actions
        dps_config_window(ioptr, input_buffer, &calculated_dps);
        calculated_dps += 0.001;

        // render
        dps_render( window, &gl_context, ioptr, &clear_color);
    }

    
    // clean up
    dps_cleanup(window, &gl_context);
}



void dps_config_window(ImGuiIO *ioptr, char *input_buffer, float *calculated_dps){
    //resize window to viewport
    ImVec2 screen_size = {ioptr->DisplaySize.x, ioptr->DisplaySize.y};
    igSetNextWindowSize(screen_size, 0);
    igSetNextWindowPos((ImVec2){0,0}, 0, (ImVec2){0,0});

    //main window config
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    {
        
        igBegin("DPS Calc", NULL, flags);
        igText("Paste Weapon info here");

        igInputTextMultiline(" ", input_buffer, 5000, (ImVec2){400, 300}, 0, NULL, NULL);
        igSameLine(0,100);
        igSetWindowFontScale(2.0);
        igText(" DPS %f", *calculated_dps);
        igSetWindowFontScale(1.0);

        igText("%.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);

        igEnd();
    }
    
    return;
}


void dps_cleanup(SDL_Window *window, SDL_GLContext *context){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    igDestroyContext(NULL);

    SDL_GL_DeleteContext(*context);
    if (window != NULL){
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();

    return;
}


void dps_render(SDL_Window *window, SDL_GLContext *context, ImGuiIO *ioptr, ImVec4 *clear_color){
    igRender();
    SDL_GL_MakeCurrent(window, context);
    glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
    glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);

}


void dsp_start_frame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
}