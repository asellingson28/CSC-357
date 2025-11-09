#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <cmath>
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
    FILE *inputfile1 = fopen("PLTL7.bmp", "rb");
    FILE *inputfile2 = fopen("PLTL7_2.bmp", "rb");
    FILE *outfile = fopen("output.bmp", "wb");
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    fread(&bfh.bfType, 2, 1, inputfile1);
    fread(&bfh.bfSize, 4, 1, inputfile1);
    fread(&bfh.bfReserved1, 2, 1, inputfile1);
    fread(&bfh.bfReserved2, 2, 1, inputfile1);
    fread(&bfh.bfOffBits, 4, 1, inputfile1);
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, inputfile1);
    int w1 = bih.biWidth;
    int h1 = bih.biHeight;
    int size1 = bih.biSizeImage; // w * h including padding

    BITMAPFILEHEADER bfh2;
    BITMAPINFOHEADER bih2;
    
    fread(&bfh2.bfType, 2, 1, inputfile2);
    fread(&bfh2.bfSize, 4, 1, inputfile2);
    fread(&bfh2.bfReserved1, 2, 1, inputfile2);
    fread(&bfh2.bfReserved2, 2, 1, inputfile2);
    fread(&bfh2.bfOffBits, 4, 1, inputfile2);
    fread(&bih2, sizeof(BITMAPINFOHEADER), 1, inputfile2);

    int w2 = bih2.biWidth;
    int h2 = bih2.biHeight;
    int size2 = bih2.biSizeImage; // w * h including padding

     BYTE *data1 = (BYTE *)mmap(NULL, size1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    BYTE *data2 = (BYTE *)mmap(NULL, size2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    BYTE *out = (BYTE *)mmap(NULL, size1, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    fread(data1, size1, 1, inputfile1);
    fread(data2, size2, 1, inputfile2);


    for (int y = 0; y < h1; y++)
    {
        for (int x = 0; x < w1; x++)
        {
            int rowStride = w1 * 3 + (4 - (w1 * 3) % 4) % 4;
            int i = y * rowStride + x * 3;

            PIXEL p;
            p.b=data1[i];
            p.g=data1[i+1];
            p.r=data1[i+2];
            // PLTL7 has a width of 1423 pixel and PLTL7_2 of 1200 pixel
            float ratio = (float) 1200/1423;

            int x_s, y_s;
            x_s = (int) (ratio * x);
            y_s = (int) (ratio * y);
            
            int rowStride2 = w2 * 3 + (4 - (w2 * 3) % 4) % 4;
            int j = y_s * rowStride2 + x_s * 3;
            PIXEL q; 
            q.b = data2[j];
            q.g = data2[j + 1];
            q.r = data2[j + 2];

            PIXEL o;
            o.b = (q.b*0.5) + (p.b*0.5);
            o.g = (q.g*0.5) + (p.g*0.5);
            o.r = (q.r*0.5) + (p.r*0.5);
            out[i] = o.b;
            out[i+1] = o.g;
            out[i+2] = o.r;
        }
    }

     // header shit
    fwrite(&bfh.bfType, 2, 1, outfile);
    fwrite(&bfh.bfSize, 4, 1, outfile);
    fwrite(&bfh.bfReserved1, 2, 1, outfile);
    fwrite(&bfh.bfReserved2, 2, 1, outfile);
    fwrite(&bfh.bfOffBits, 4, 1, outfile);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, outfile);
    fwrite(out, size1, 1, outfile); // all pixel data
    return 0;
}