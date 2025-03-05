#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"
#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "sdl_helpers.c"

#define DEBUG 1
#define WEAPON_TEXT_MAX 5000

typedef struct{
    int  phys_low, phys_high;
    int  fire_low, fire_high;
    int  cold_low, cold_high;
    int  ligh_low, ligh_high;
    int  chaos_low, chaos_high;
    float aps;
} weapon;

typedef struct{
    float dps;
    float edps;
    float combined;
} dps_vals;

void dps_cleanup( SDL_Window *window, SDL_GLContext *context);
void dps_render(SDL_Window *window, SDL_GLContext *context, ImGuiIO *ioptr, ImVec4 *clear_color);
void dps_config_window(ImGuiIO *ioptr, char *input_buffer, dps_vals *calculated_dps);
void dps_start_frame();
dps_vals dps_calculate_dps(weapon *input_weapon);
void dps_populate_weapon(const char *weapon_text, weapon *output_weapon);
void dps_debug_window(ImGuiIO *ioptr, char *input_buffer, dps_vals *calculated_dps, weapon *weapon);







int main(int argc, char* argv[]){
    //setup SDL and OpenGL
    SDL_Window *window = sdl_setup("PoE DPS Calculator", 750, 400, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
    char input_buffer[WEAPON_TEXT_MAX] = {0};
    dps_vals calculated_dps;
    weapon main_weapon = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0};


    //main loop
    bool quit = false;
    while (!quit){
        quit = sdl_main_loop(window);

        // start imgui frame
        dps_start_frame();

        //application actions
        dps_config_window(ioptr, input_buffer, &calculated_dps);

        if (strlen(input_buffer) > 5){
            dps_populate_weapon(input_buffer, &main_weapon);
            calculated_dps = dps_calculate_dps(&main_weapon);
        }
        if (DEBUG == 1){
            dps_debug_window(ioptr, input_buffer, &calculated_dps, &main_weapon);
        }



        // render
        dps_render( window, &gl_context, ioptr, &clear_color);
    }

    
    // clean up
    dps_cleanup(window, &gl_context);
}




void dps_populate_weapon(const char *weapon_text, weapon *output_weapon){
    *output_weapon = (weapon) {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0};
    char loop1[WEAPON_TEXT_MAX];
    strncpy(loop1, weapon_text, WEAPON_TEXT_MAX);
    char loop2[WEAPON_TEXT_MAX];
    strncpy(loop2, weapon_text, WEAPON_TEXT_MAX);

    bool has_fire_dmg = false;
    bool has_cold_dmg = false;
    bool has_ligh_dmg = false;

    //First loop: check for fire, cold, ligh dmg
    char *line = strtok(loop1, "\r\n");
    while (line != NULL){
        int low, high;
        char tmp[256] = {0};

        if (sscanf(line, "Adds %d to %d %s Damage", &low, &high, tmp) == 3){
            if (strstr(tmp, "Fire")){
                has_fire_dmg = true;
            }
            if (strstr(tmp, "Cold")){
                has_cold_dmg = true;
            }
            if (strstr(tmp, "Lightning")){
                has_ligh_dmg = true;
            }
            

        }

        line = strtok(NULL, "\r\n");
    }



    // actually populate
    line = strtok(loop2, "\r\n");
    while (line != NULL){
        sscanf(line, "Physical Damage: %d-%d", &output_weapon->phys_low, &output_weapon->phys_high);
        sscanf(line, "Chaos Damage: %d-%d", &output_weapon->chaos_low, &output_weapon->chaos_high);
        sscanf(line, "Attacks per Second: %f", &output_weapon->aps);


        if (has_fire_dmg == true && has_cold_dmg == false && has_ligh_dmg == false){
            sscanf(line, "Elemental Damage: %d-%d", &output_weapon->fire_low, &output_weapon->fire_high);
        }
        if (has_fire_dmg == false && has_cold_dmg == true && has_ligh_dmg == false){
            sscanf(line, "Elemental Damage: %d-%d", &output_weapon->cold_low, &output_weapon->cold_high);
        }
        if (has_fire_dmg == false && has_cold_dmg == false && has_ligh_dmg == true){
            sscanf(line, "Elemental Damage: %d-%d", &output_weapon->ligh_low, &output_weapon->ligh_high);
        }
        if (has_fire_dmg == true && has_cold_dmg == true && has_ligh_dmg == false){
            sscanf(line, "Elemental Damage: %d-%d (augmented), %d-%d ", &output_weapon->fire_low, &output_weapon->fire_high, &output_weapon->cold_low, &output_weapon->cold_high);
        }
        if (has_fire_dmg == true && has_cold_dmg == false && has_ligh_dmg == true){
            sscanf(line, "Elemental Damage: %d-%d (augmented), %d-%d ", &output_weapon->fire_low, &output_weapon->fire_high, &output_weapon->ligh_low, &output_weapon->ligh_high);
        }
        if (has_fire_dmg == false && has_cold_dmg == true && has_ligh_dmg == true){
            sscanf(line, "Elemental Damage: %d-%d (augmented), %d-%d ", &output_weapon->cold_low, &output_weapon->cold_high, &output_weapon->ligh_low, &output_weapon->ligh_high);
        }
        if (has_fire_dmg == true && has_cold_dmg == true && has_ligh_dmg == true){
            sscanf(line, "Elemental Damage: %d-%d (augmented), %d-%d (augmented), %d-%d (augmented)",
            &output_weapon->fire_low, &output_weapon->fire_high, &output_weapon->cold_low, &output_weapon->cold_high, &output_weapon->ligh_low, &output_weapon->ligh_high);
        }


        line = strtok(NULL, "\r\n");
    }   

}

dps_vals dps_calculate_dps(weapon *input_weapon){
    weapon w = *input_weapon;
    float summed = (w.phys_low + w.phys_high + 
                    w.fire_low + w.fire_high + 
                    w.cold_low + w.cold_high + 
                    w.ligh_low + w.ligh_high + 
                    w.chaos_low + w.chaos_high);

    dps_vals dps;
    dps.combined = (summed/2) * w.aps;

    return dps;
}



void dps_config_window(ImGuiIO *ioptr, char *input_buffer, dps_vals *calculated_dps){
    //resize window to viewport
    ImVec2 screen_size = {ioptr->DisplaySize.x, ioptr->DisplaySize.y};
    igSetNextWindowSize(screen_size, 0);
    igSetNextWindowPos((ImVec2){0,0}, 0, (ImVec2){0,0});

    //main window config
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
    {
        
        igBegin("DPS Calc", NULL, flags);
        igText("Paste Weapon info here");

        igInputTextMultiline(" ", input_buffer, WEAPON_TEXT_MAX, (ImVec2){400, 300}, 0, NULL, NULL);
        igSameLine(0,75);
        igSetWindowFontScale(2.0);
        igText("total DPS: %f \n Ele DPS: %f", calculated_dps->combined, calculated_dps->combined);
        igSetWindowFontScale(1.0);

        

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
    SDL_GL_MakeCurrent(window, *context);
    glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
    glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);

}


void dps_start_frame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
}



void dps_debug_window(ImGuiIO *ioptr, char *input_buffer, dps_vals *calculated_dps, weapon *weapon){


    //main window config
    ImGuiWindowFlags flags = 0;
    {
        
        igBegin("Debug", NULL, flags );
        igText("Input char length: %d", strlen(input_buffer));
        igText("Physical Damage: %d-%d", weapon->phys_low, weapon->phys_high);
        igText("Fire Damage: %d-%d", weapon->fire_low, weapon->fire_high);
        igText("Cold Damage: %d-%d", weapon->cold_low, weapon->cold_high);
        igText("Lightning Damage: %d-%d", weapon->ligh_low, weapon->ligh_high);
        igText("Chaos Damage: %d-%d", weapon->chaos_low, weapon->chaos_high);
        igText("Attacks Per Second: %f", weapon->aps);
        igText("%.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
        igEnd();
    }
    
    return;
}