//C++ FastIF decoder
//Copyright (C) 2020 Extrasklep

#include <iostream>
#include <fstream>
#include <png++/png.hpp>
#include "fif_decoder.h"

int main(int argc, char** args) {
    if(argc<3) {
        std::cout << "Usage: " << args[0] << " [FIF input file] [PNG output file]\n";
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
    signed int res = 0;
    while((res = FIF_read(fif))) {
        if(res == 0) break;
        if(res < 0) {
            std::cout << "FIF error " << (int)res << ".\n";
            exit(2);
        }
    }
    
    //Write output
    png::image<png::rgb_pixel> image(fif->width,fif->height);
    for(unsigned int y=0;y<fif->height;y++) {
        for(unsigned int x=0;x<fif->width;x++) {
            image[y][x] = png::rgb_pixel(fif->decoded_data[y*fif->width+x].r,fif->decoded_data[y*fif->width+x].g,fif->decoded_data[y*fif->width+x].b);
        }
    }
    image.write(args[2]);
    
    FIF_free(fif);
    delete fif;
    return 0;
}
