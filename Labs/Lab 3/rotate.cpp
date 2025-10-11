//     Exercise:
// Lets speed up working on a file:
// Read any bitmap file and rotate the image around its middle point. However, do this in N (1-4)
// processes to speed up the CPU worktime.
// Measure the time print it.
// Howto:
// Similar to program 1, read a bitmap (BMP) file into memory. Allocate your memory with
// mmap() and don’t forget to set the shared memory flag.
// Fork the process with fork() so that N processes work on the file at the same time.
// Program call:
// ./yourprogram [IMAGEFILE] [ROTATING ANGLE] [N PROCESSES] [OUTPUTFILE]
// [IMAGEFILE] that’s your bitmap
// [ROTATING ANGLE] a floating point number in radiants
// [N PROCESSES] number of processes involved. 1-4.
// [OUTPUTFILE] the output file
// Example:
// ./rotate lion.bmp 2.1 4 result.bmp

#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>  
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>


using namespace std;


typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

struct BITMAPFILEHEADER
{
    WORD bfType; //specifies the file type
    DWORD bfSize; //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};
struct BITMAPINFOHEADER
{
    DWORD biSize; //specifies the number of bytes required by the struct
    LONG biWidth; //specifies width in pixels
    LONG biHeight; //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage; //size of image in bytes
    LONG biXPelsPerMeter; //number of pixels per meter in x axis
    LONG biYPelsPerMeter; //number of pixels per meter in y axis
    DWORD biClrUsed; //number of colors used by th ebitmap
    DWORD biClrImportant; //number of colors that are important
};

int main(int argc, char *argv[]) { 
    // start timer
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

    fwrite(&bfh.bfType,     2, 1, outfile);
    fwrite(&bfh.bfSize,     4, 1, outfile);
    fwrite(&bfh.bfReserved1,2, 1, outfile);
    fwrite(&bfh.bfReserved2,2, 1, outfile);
    fwrite(&bfh.bfOffBits,  4, 1, outfile);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, outfile);

    int w = bih.biWidth;
    int h = bih.biHeight;
    int size = bih.biSizeImage; // w * h including padding

    // // mmap in c

    // using malloc
            // BYTE* data = (BYTE *)malloc(size);
            // fread(data, size, 1, inputfile);
            // fclose(inputfile);
            // BYTE* out = (BYTE *) malloc(size);
    // using mmap

    BYTE *in_map = (BYTE*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, -1, 0);

    BYTE *out_map = (BYTE*)mmap(NULL, size, PROT_WRITE, MAP_SHARED, -1, 0);

    memcpy(out_map, in_map, size);
    close(infile);
    close(outfile);

    // load all bytes of the file into memory


    // create n_processes forks 

    // end timer
    // wait(0);
    timeval time_final;
    gettimeofday(&time_final, 0);
    long ms = time_final.tv_usec - time_initial.tv_usec;
    ms += (time_final.tv_usec - time_initial.tv_usec) * 1000000; // convert to microseconds
    return 0;
}