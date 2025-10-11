// emails: bquezon@calpoly.edu, jrange19@calpoly.edu, hchen82@calpoly.edu
#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <sys/time.h>


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

int main()
{
    // start timer
    // clock_t a = clock();
    // cout << (float) a/ CLOCKS_PER_SEC << endl;
    timeval time_initial;
    gettimeofday(&time_initial, 0);

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

    BYTE *data = (BYTE *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    BYTE *out = (BYTE *)mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // cout << w << " " << h;
    fread(data, size, 1, inputfile);
    
    int currentmax = size/4;
    int currentstart = 0;
    for (int q = 0; q < 4; q++)
    {
        // q is a fork
        pid_t pid = fork();
        if(pid == 0) {
            // child
            for (int i = currentstart; i < currentmax; i+=3)
            {
                if(q == 0) {
                    PIXEL p;
                    BYTE B = data[i];
                    BYTE G = 0;
                    BYTE R = data[i + 2];
                    p ={B, G, R};
        
                    out[i] = p.b;
                    out[i + 1] = p.g;
                    out[i + 2] = p.r;
                } else if (q == 1)
                {
                    PIXEL p;
                    BYTE B = 0;
                    BYTE G = data[i+1];
                    BYTE R = data[i + 2];
                    p ={B, G, R};
        
                    out[i] = p.b;
                    out[i + 1] = p.g;
                    out[i + 2] = p.r;
                } else if (q == 2)
                {
                    PIXEL p;
                    BYTE B = data[i];
                    BYTE G = data[i+1];
                    BYTE R = 255;
                    p ={B, G, R};
        
                    out[i] = p.b;
                    out[i + 1] = p.g;
                    out[i + 2] = p.r;
                } else if (q == 3)
                {
                    PIXEL p;
                    BYTE B = 255;
                    BYTE G = data[i+1];
                    BYTE R = data[i+2];
                    p ={B, G, R};
        
                    out[i] = p.b;
                    out[i + 1] = p.g;
                    out[i + 2] = p.r;
                }
                
            }
            _exit(0);
        }
        currentstart += size/4;
        currentmax += size/4;
    }




    wait(0);

    fwrite(&bfh.bfType, 2, 1, outfile);
    fwrite(&bfh.bfSize, 4, 1, outfile);
    fwrite(&bfh.bfReserved1, 2, 1, outfile);
    fwrite(&bfh.bfReserved2, 2, 1, outfile);
    fwrite(&bfh.bfOffBits, 4, 1, outfile);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, outfile);
    fwrite(out, size, 1, outfile);
    
    timeval time_final;
    gettimeofday(&time_final, 0);
    long ms = time_final.tv_usec - time_initial.tv_usec;
    ms += (time_final.tv_sec - time_initial.tv_sec) * 1000000; // convert to microseconds
    cout << ms << endl;
    // a = clock();
    // cout << (float) a/ CLOCKS_PER_SEC << endl;
    // cout << ms << endl;
    return 0;
}