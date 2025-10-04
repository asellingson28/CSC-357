
#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>
#include <algorithm>

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
    FILE *inputfile = fopen("PLTL2.bmp", "rb");
    FILE *outfile = fopen("mirrored.bmp", "wb");

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

    BYTE *out = (BYTE *)malloc(size);
    BYTE *data = (BYTE *)malloc(size);

    // cout << w << " " << h;
    fread(data, size, 1, inputfile);

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int rowStride = w * 3 + (4 - (w * 3) % 4) % 4;
            int idx = y * rowStride + x * 3;

            PIXEL p;
            BYTE B = data[idx];
            BYTE G = data[idx + 1];
            BYTE R = data[idx + 2];
            p = {B, G, R};

            int newIdx = (h - y - 1) * rowStride + (w - x - 1) * 3;
            out[newIdx] = p.b;
            out[newIdx + 1] = p.g;
            out[newIdx + 2] = p.r;
        }
    }

    // const float ratiof = 4.731475; // 5 
    float ratio = 5;
    float ratiox = 0.4216444132;
    float ratioy = 0.5012531328;

    
    for (int x = 0; x < 600; x++)
    {
        for (int y = 399; y > 0; y--)
        {
            int rowStride = w * 3 + (4 - (w * 3) % 4) % 4;
            int idx = (y - 1) * rowStride + (x) * 3;
            PIXEL p;
            BYTE B = data[(int) (idx*(ratio-3))];
            BYTE G = data[(int) (idx*(ratio-3) + 1)];
            BYTE R = data[(int) (idx*(ratio-3) + 2)];
            p = {B, G, R};

            out[idx]  = p.b;
            out[idx + 1]  = p.g;
            out[idx + 2]  = p.r;

        }
    }

    fwrite(&bfh.bfType, 2, 1, outfile);
    fwrite(&bfh.bfSize, 4, 1, outfile);
    fwrite(&bfh.bfReserved1, 2, 1, outfile);
    fwrite(&bfh.bfReserved2, 2, 1, outfile);
    fwrite(&bfh.bfOffBits, 4, 1, outfile);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, outfile);
    fwrite(out, size, 1, outfile);
    return 0;
}