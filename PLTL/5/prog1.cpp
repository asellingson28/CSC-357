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

// khhale@calpoly.edu pzhou02@calpoly.edu
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

    FILE *inputfile = fopen("img.bmp", "rb");
    FILE *outfile = fopen("result.bmp", "wb");

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    fread(&bfh.bfType, 2, 1, inputfile);
    fread(&bfh.bfSize, 4, 1, inputfile);
    fread(&bfh.bfReserved1, 2, 1, inputfile);
    fread(&bfh.bfReserved2, 2, 1, inputfile);
    fread(&bfh.bfOffBits, 4, 1, inputfile);
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, inputfile);

    int w = bih.biWidth;
    int h = bih.biHeight;
    int size = bih.biSizeImage; // w * h including padding

    // int finfo = shm_open("PLTLINFO", O_CREAT | O_RDWR, 0777);
    // ftruncate(finfo, sizeof(BITMAPINFOHEADER));
    // BYTE *info = (BYTE *)mmap(NULL, sizeof(BITMAPINFOHEADER), PROT_READ | PROT_WRITE, MAP_SHARED, finfo, 0);
    // fread(info, sizeof(BITMAPINFOHEADER), 1, inputfile);


    int fdata = shm_open("PLTLDATA", O_CREAT | O_RDWR, 0777);
    ftruncate(fdata, size);
    BYTE *data = (BYTE *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fdata, 0);
    
    // cout << w << " " << h;
    fread(data, size, 1, inputfile);
    
    int fd = shm_open("PLTL5", O_CREAT | O_RDWR, 0777);
    ftruncate(fd, size);
    BYTE *out = (BYTE *)mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0); // fd makes it shared

    int flag = shm_open("PLTL5flag", O_CREAT | O_RDWR, 0777);
    ftruncate(flag, 4);
    int *outflag = (int *)mmap(NULL, 4, PROT_WRITE | PROT_READ, MAP_SHARED, flag, 0); 
    *outflag = 0;



    for (int q = 0; q < w/4; q++)
    {
        for (int j = 0; j < h; j++) {
            int rowStride = w * 3 + (4 - (w * 3) % 4) % 4;
            int idx = j * rowStride + q * 3;
            PIXEL p;
            BYTE B = data[idx];
            BYTE G = data[idx+1];
            BYTE R = 0;
            p ={B, G, R};
            out[idx] = p.b;
            out[idx + 1] = p.g;
            out[idx + 2] = p.r;
        }
    }




    while (*outflag==0) {
        
    }
    fwrite(&bfh.bfType, 2, 1, outfile);
    fwrite(&bfh.bfSize, 4, 1, outfile);
    fwrite(&bfh.bfReserved1, 2, 1, outfile);
    fwrite(&bfh.bfReserved2, 2, 1, outfile);
    fwrite(&bfh.bfOffBits, 4, 1, outfile);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, outfile);
    fwrite(out, size, 1, outfile);
    close(fd);
    close(flag);
    shm_unlink("PLTL5");
    shm_unlink("PLTL5flag");
    shm_unlink("PLTL5DATA");
    munmap(out, size);
    munmap(outflag, 4);
    munmap(data, size);
    return 0;
}