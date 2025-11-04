#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <sys/time.h>
#include <cmath>
#define PI 3.14159265


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

int main(int argc, char *argv[])
{
    timeval time_initial;
    gettimeofday(&time_initial, 0);

    if (argc != 5) return 1;
    // argv is the script name
    string inputname = argv[1];
    float rads = atof(argv[2]);
    int n = atoi(argv[3]);
    string outputname = argv[4];
    // FIXME replace with open and mmap
    FILE *inputfile = fopen(inputname.c_str(), "rb");
    FILE *outfile = fopen(outputname.c_str(), "wb");

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

    fread(data, size, 1, inputfile);
    
    int currentmax = size/n; // split into n number of chunks
    int currentstart = 0;
    int rows_per = h / n;

    for (int q = 0; q < n; q++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // child
            for (int y = currentstart; y < currentmax; y++)
            {
                for (int x = 0; x < w; x++)
                {
                    // pixel calc
                    int rowStride = w * 3 + (4 - (w * 3) % 4) % 4;
                    int i = y * rowStride + x * 3;
                    PIXEL p;
                    BYTE B = data[i];
                    BYTE G = data[i + 1];
                    BYTE R = data[i + 2];
                    p = {B, G, R};

                    // matrix and origin calculation 
                    float alpha = - rads;
                    int xr = x - w/2;
                    int yr = y -h/2;

                    // $x_{ot} = x_r \cos \alpha + y_r \sin \alpha$
                    int xot = xr * cos(alpha) + yr * sin(alpha);
                    // $y_{ot} = -x_r \sin \alpha + y_r \cos \alpha$
                    int yot = -xr * sin(alpha) + yr * cos(alpha);

                    int xo = xot + w/2;
                    int yo = yot + h/2;

                    // Now you have the original coordinates x_o and y_o. Check if they are within the
                    // boundaries, if not, put black into the pixel at x and y. But if, then sample the color at x_o
                    // and y_o into x and y.
                    if (xo >= 0 && xo < w && yo >= 0 && yo < h) {
                        int j = yo * rowStride + xo * 3; 

                        out[i] = data[j];
                        out[i + 1] = data[j + 1];
                        out[i + 2] = data[j+2];
                    } else {
                        out[i] = 0;
                        out[i + 1] = 0;
                        out[i + 2] = 0;
                    }
                }
            }
            _exit(0);
        }
        currentstart += rows_per;
        currentmax += rows_per;
        if (q == n - 2) currentmax = h; // ensure last chunk covers any remainder

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
    cout << "Using " << n << " subprocesses with a rotation of " << rads << " radians" << endl;
    cout << ms << " microseconds" << endl;
    cout << "Output: " << outputname << endl;

    return 0;
}