//C++ FastIF decoder
//Copyright (C) 2020 Extrasklep

#pragma once
#include <iostream>
#include <cstring>

struct FIFrgb {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
};

struct FIF {
    const unsigned char* data = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned long datapos = 0;
    bool eof = 0;
    FIFrgb currentBG;
    FIFrgb currentFG;
    FIFrgb* decoded_data = nullptr;
};

#include "utils.h"

const unsigned char fif_bitindex[8] = {0,2,4,1,3,5,6,7};

void FIF_renderchar(FIF* fiffile, unsigned int x, unsigned int y, unsigned char c) {
    const bool* bits = fif_getbits(c);
    unsigned int pos = (x*2)+(y*4)*fiffile->width;
    for(unsigned char i=0;i<8;i++) {
        unsigned int wpos = pos + ((fif_bitindex[i] / 2) * fiffile->width) + (fif_bitindex[i] % 2);
        if(wpos >= fiffile->width*fiffile->height) {std::cout << "Writer went out of bounds (X: " << x << " Y: " << y << ").\n"; return;};
        fiffile->decoded_data[wpos] = bits[i] ? fiffile->currentFG : fiffile->currentBG;
    }
}

/*
 * Main read function
 * 
 * Returns:
 *  0 - success, done decoding file
 * >0 - sleep operation, ms to sleepyes
 * <0 - error
 */
signed int FIF_read(FIF* fiffile) {
    if(fiffile->eof) return 0;
    if(fiffile->datapos == 0) {
        //Magic
        char* str = fif_readbytesAsString(fiffile,6);
        if(strcmp(str,"FastIF") != 0) {
            std::cout << "Invalid FIF file.\n";
            return -1;
        }
        //Width and height
        fiffile->width  = fif_readbyte(fiffile) * 2;
        fiffile->height = fif_readbyte(fiffile) * 4;
        //Initialize memory
        fiffile->decoded_data = new FIFrgb[fiffile->width*fiffile->height];
        fiffile->currentBG.r = 0;
        fiffile->currentBG.g = 0;
        fiffile->currentBG.b = 0;
        fiffile->currentFG.r = 0;
        fiffile->currentFG.g = 0;
        fiffile->currentFG.b = 0;
    }
    
    //Main reading loop
    while(!fiffile->eof) {
        unsigned char opbyte = 0;
        switch(opbyte = fif_readbyte(fiffile)) {
            //BG color
            case 0x01: {
                fiffile->currentBG.r = fif_readbyte(fiffile);
                fiffile->currentBG.g = fif_readbyte(fiffile);
                fiffile->currentBG.b = fif_readbyte(fiffile);
                break;
            }
            //FG color
            case 0x02: {
                fiffile->currentFG.r = fif_readbyte(fiffile);
                fiffile->currentFG.g = fif_readbyte(fiffile);
                fiffile->currentFG.b = fif_readbyte(fiffile);
                break;
            }
            //Horizontal line
            case 0x10: {
                unsigned int x = fif_readbyte(fiffile);
                unsigned int y = fif_readbyte(fiffile);
                unsigned int s = fif_readbyte(fiffile);
                for(unsigned int i=0;i<s;i++) {
                    FIF_renderchar(fiffile,x+i,y,fif_readbyte(fiffile));
                }
                break;
            }
            //Fill
            case 0x11: {
                unsigned int x = fif_readbyte(fiffile);
                unsigned int y = fif_readbyte(fiffile);
                unsigned int fw = fif_readbyte(fiffile);
                unsigned int fh = fif_readbyte(fiffile);
                unsigned char c = fif_readbyte(fiffile);
                for(unsigned int ix=0;ix<fw;ix++) {
                    for(unsigned int iy=0;iy<fh;iy++) {
                        FIF_renderchar(fiffile,x+ix,y+iy,c);
                    }
                }
                break;
            }
            //Sleep
            case 0x12: {
                return fif_readbyte(fiffile) * 10;
                break;
            }
            //Vertical line
            case 0x13: {
                unsigned int x = fif_readbyte(fiffile);
                unsigned int y = fif_readbyte(fiffile);
                unsigned int s = fif_readbyte(fiffile);
                for(unsigned int i=0;i<s;i++) {
                    FIF_renderchar(fiffile,x,y+i,fif_readbyte(fiffile));
                }
                break;
            }
            //EOF
            case 0x20: {
                fiffile->eof = 1;
                return 0;
            }
            //Unsupported
            default: {
                std::cout << "Unsupported opcode " << std::hex << (unsigned int)opbyte << " at offset " << fiffile->datapos-1 << ".\n";
                return -1;
            }
        }
    }
    return 0;
}

//Free FIF struct memory
void FIF_free(FIF* fiffile) {
    delete[] fiffile->data;
    delete[] fiffile->decoded_data;
}
