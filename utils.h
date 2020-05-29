#pragma once

unsigned char* fif_slice;
char* fif_slicestring;

//Read bytes of data
unsigned char* fif_readbytes(FIF* fiffile,unsigned long length) {
    delete[] fif_slice;
    fif_slice = new unsigned char[length];
    for(unsigned int i=0;i<length;i++) {
        fif_slice[i]=fiffile->data[fiffile->datapos++];
    }
    return fif_slice;
}

//Get slice as a null terminated string
char* fif_readbytesAsString(FIF* fiffile,unsigned long length) {
    unsigned char slicestr[length+1];
    unsigned char* bytes=fif_readbytes(fiffile,length);
    for(unsigned int i=0;i<length;i++) {
        slicestr[i]=bytes[i];
        if(slicestr[i]=='\0') {slicestr[i]=' ';}
    }
    slicestr[length]='\0';
    delete[] fif_slice;
    fif_slicestring = new char[length+1];
    for(unsigned int i=0;i<length+1;i++) {
        fif_slicestring[i]=slicestr[i];
    }
    return fif_slicestring;
}

//Get slice and convert it to a number
unsigned char fif_readbyte(FIF* fiffile) {
    return fiffile->data[fiffile->datapos++];
}

bool fif_bits[8] = {0};
const bool* fif_getbits(unsigned char b) {
    unsigned char mask=1;
    for(unsigned char i=0;i<8;i++) {
        fif_bits[i]=(b & (mask << i)) != 0;
    }
    return fif_bits;
}
