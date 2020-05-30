# cpp-decoder
C++ decoder for FIF

## Building
Run the build.sh script to compile both programs.

Reqired libraries: png++ (for fif2png) and SDL2 (for fifsdl).

## fif2png
Converts a FIF file to a PNG file.

./fif2png input.fif output.png

## fifsdl
SDL based viewer for FIF images and videos.

./fifsdl input.fif

## fif_decoder.h usage
FIF struct:
```cpp
struct FIFrgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
}
struct FIF {
    const unsigned char* data;
    unsigned int width;
    unsigned int height;
    unsigned long datapos;
    bool eof;
    FIFrgb currentBG;
    FIFrgb currentFG;
    FIFrgb* decoded_data = nullptr;
}
```
Create your FIF struct object:
`cpp
FIF* fif = new FIF;
`
Write file data to FIF->data

Read FIF:
`cpp
FIF_read(fif)
`
Return codes (signed int):

Less than 0: Error reading file

More than 0: Sleep operation, returned number is milliseconds to sleep

0: Done reading file


Read the decoded data from FIF->decoded_data as standard RGB pixel data. (FIF->width * FIF->height)


Closing:
```cpp
FIF_free(fif);
delete fif;
```
