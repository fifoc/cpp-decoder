//FastIF SDL viewer
//Copyright (C) 2020 Extrasklep

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include "fif_decoder.h"

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

unsigned int XRES, YRES;

void setpixelsdl(SDL_Surface *surface, int x, int y, uint32_t pixel) {
    uint8_t *target_pixel = (uint8_t *)surface->pixels + y * surface->pitch + x * 4;
    *(uint32_t *)target_pixel = pixel;
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t mapRGB(int r, int g, int b) {return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);}

bool init(const char* title) {
    //Initialize SDL
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
        std::cout << "SDL init error: " << SDL_GetError();
    }
    else {
        //Create window
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, XRES, YRES, SDL_WINDOW_SHOWN);
        if(window == NULL) {
            std::cout << "SDL window error: " << SDL_GetError();
        }
        else {
            //Get window surface
            screenSurface = SDL_GetWindowSurface(window);
            //Update the surface
            SDL_UpdateWindowSurface(window);
        }
    }
    return 0;
}

void quit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** args) {
    if(argc<2) {
        std::cout << "Usage: " << args[0] << " [FIF input file]\n";
        return 0;
    }
    
    FIF* fif = new FIF;
    std::ifstream ifile(args[1],std::ios::in|std::ios::binary|std::ios::ate);
    if(!ifile.is_open()) {
        perror(args[1]);
        exit(1);
    } else {
        std::streampos size = ifile.tellg();
        ifile.seekg(0);
        fif->data = new unsigned char[size];
        ifile.read((char*)fif->data,size);
        ifile.close();
    }
    
    //Read FIF
    signed int res = FIF_read(fif);
    if(res < 0) {
        std::cout << "FIF error " << (int)res << ".\n";
        exit(2);
    }
    XRES = fif->width;
    YRES = fif->height;
    unsigned long vbufsize = fif->width*fif->height;
    
    bool quitRequest=false;
    SDL_Event sdlEvent;
    
    init("FIF viewer");
    
    while(quitRequest==0) {
        //Handle events on queue 
        while(SDL_PollEvent( &sdlEvent ) != 0) { 
            //Quit request
            if( sdlEvent.type == SDL_QUIT ) {quitRequest = true;}
        }
        
        //Play FIF
        if(!fif->eof) {
            signed int res = FIF_read(fif);
            if(res == 0) {
                std::cout << "Playback done\n";
            } else if(res < 0) {
                std::cout << "FIF error " << (int)res << ".\n";
                exit(2);
            } else if(res > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(res));
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        
        //Write image data to SDL
        for(unsigned long i=0;i<vbufsize;i++) {
            ((uint32_t*)screenSurface->pixels)[i] = mapRGB(fif->decoded_data[i].r,fif->decoded_data[i].g,fif->decoded_data[i].b);
        }
        
        SDL_UpdateWindowSurface( window );
    }
    
    quit();
    
    FIF_free(fif);
    delete fif;
    
    return 0;
}
