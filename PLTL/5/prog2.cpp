#include <fcntl.h>
#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "../../../../../../../Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk/usr/include/unistd.h"


using namespace std;

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;      // specifies the file type
    DWORD bfSize;     // specifies the size in bytes of the bitmap file
    WORD bfReserved1; // reserved; must be 0
    WORD bfReserved2; // reserved; must be 0
    DWORD bfOffBits;  // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;         // specifies the number of bytes required by the struct
    LONG biWidth;         // specifies width in pixels
    LONG biHeight;        // specifies height in pixels
    WORD biPlanes;        // specifies the number of color planes, must be 1
    WORD biBitCount;      // specifies the number of bits per pixel
    DWORD biCompression;  // specifies the type of compression
    DWORD biSizeImage;    // size of image in bytes
    LONG biXPelsPerMeter; // number of pixels per meter in x axis
    LONG biYPelsPerMeter; // number of pixels per meter in y axis
    DWORD biClrUsed;      // number of colors used by the bitmap
    DWORD biClrImportant; // number of colors that are important
} BITMAPINFOHEADER;

struct PIXEL
{
    // in range 0 to 255
    BYTE b; // 1 byte
    BYTE g; // 1 byte
    BYTE r; // 1 byte
};

int main () {


    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    

    int w = 1423;
    int h = 798;
    int size = 3409058; // w * h including padding

    
    // cout << w << " " << h;
    

    // fread(&bfh.bfSize, 4, 1, );
    //int fd = -1;
    //while (fd == -1) {
        //}
        
        
        //int flag = -1;
        //while (flag == -1) {
            //}
    int fd = shm_open("PLTL5", O_RDWR, 0777);
        
    int flag = shm_open("PLTL5flag", O_RDWR, 0777);

    int fdata = shm_open("PLTLDATA", O_RDWR, 0777);
    
    int *outflag = (int *)mmap(NULL, 4, PROT_WRITE | PROT_READ, MAP_SHARED, flag, 0); 
    BYTE *out = (BYTE *)mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0); // fd makes it shared
    BYTE *data = (BYTE *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fdata, 0);

    for (int q = w/4; q < w/2; q++)
    {
        for (int j = 0; j < h; j++) {
            int rowStride = w * 3 + (4 - (w * 3) % 4) % 4;
            int idx = j * rowStride + q * 3;
            PIXEL p;
            BYTE B = data[idx];
            BYTE G = 0;
            BYTE R = data[idx+2];
            p ={B, G, R};
            out[idx] = p.b;
            out[idx + 1] = p.g;
            out[idx + 2] = p.r;
        }
    }
    // close(fd);
    // close(flag);
    // shm_unlink("PLTL5");
    // shm_unlink("PLTL5flag");
    // shm_unlink("PLTL5DATA");
    // munmap(out, size);
    // munmap(outflag, 4);
    // munmap(data, size);
    return 0;
}