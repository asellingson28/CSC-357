/* 

Write a program to change the contrast of a bitmap image.
The image files should be 24Bit per pixel (standard) BMP files. You will find several ones in the
adjunkt zip to this assigment. You can use your own ones – save them as 24Bit BMP in e.g.
photoshop.
For looping through the pixels and change:
- Color saturation
- Lightness
- Contrast

he program should read several parameters from the comand line:
[programname] [imagefile1] [outputfile] [operation]
[contrastfactor]
e.g.
./imgchange face.bmp resultface.bmp contrast 3.2
Catching wrong or missing parameters not necessary.
Operations are meant with normalized values:
Contrast: Pixel = pow ( Pixel , factor); //factor [0,100]
Saturation: Pixel (color) = Pixel (color) + (Pixel (color) – Pixel (average)) * factor; //factor [0,1]
Lightness: Pixel (color) = Pixel (color) + factor; //factor [-1,1]
Don’t forget, that the result pixel color values must be (capped) in the range of [0,1];
Howto:
First read the bitmap file header infos into corresponding structures and then the image data
into an array of BYTES (unsigned char). You need to allocate this array first. The size is
dependend on the resolution, which you get with the file headers:
http://www.dragonwins.com/domains/GetTechEd/bmp/bmpfileformat.htm Wikipedia isn’t
bad here, but a bit chaotic:
https://en.wikipedia.org/wiki/BMP_file_format
Important!
Color tables and bit masks are not necessary, that’s too complex. So all optional BMP data will
be skipped!

*/

#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>  

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

// each pixel has 3 bytes
struct PIXEL
{
    // in range 0 to 255 
    BYTE b; // 1 byte
    BYTE g; // 1 byte
    BYTE r; // 1 byte
};

struct FPixel {
    double b, g, r; // 0..1
};

enum class Op { Contrast, Saturation, Lightness };

FPixel normalizePixel(const PIXEL& p) {
    return FPixel{
        p.b / 255.0,
        p.g / 255.0,
        p.r / 255.0
    };
}

PIXEL unnormalizePixel(const FPixel& p){
    PIXEL q;
    q.b = (BYTE)(p.b * 255.0);
    q.g = (BYTE)(p.g * 255.0);
    q.r = (BYTE)(p.r * 255.0);
    return q;
}


FPixel contrastPixel(const FPixel& p, double factor) {
    return FPixel{pow(p.b, factor), pow(p.g, factor), pow(p.r, factor)};
}
FPixel saturatePixel(const FPixel& p, double factor) {
    double avg = (p.r + p.g + p.b) / 3.0;

    return FPixel{
        p.b + (p.b - avg) * factor,
        p.g + (p.g - avg) * factor,
        p.r + (p.r - avg) * factor
    };
}
FPixel lightenPixel(const FPixel& p, double factor) {

    return FPixel{
        p.b + factor,
        p.g + factor,
        p.r + factor
    };
}
int main(int argc, char *argv[]) { // argc is argument count, argv is the cmdline input where argv[0] is the filename
    // we want filename inputfile outputfile operation constantfactor such that argc = 5
    if (argc != 5) return 1;
    string inputname = argv[1];
    string outputname = argv[2];
    string operation = argv[3];
    double constfac = atof(argv[4]); 

    FILE *inputfile = fopen(inputname.c_str(), "rb");
    FILE *outfile = fopen(outputname.c_str(), "wb");

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    // reads the amount of bytes of a bfh and bih from input file into local program memory
    //fread where, how much, how often, input file
    // fread(&bfh, sizeof(BITMAPFILEHEADER), 1, inputfile);
    fread(&bfh.bfType, 2, 1, inputfile);
    fread(&bfh.bfSize, 4, 1, inputfile);
    fread(&bfh.bfReserved1, 2, 1, inputfile);
    fread(&bfh.bfReserved2, 2, 1, inputfile);
    fread(&bfh.bfOffBits, 4, 1, inputfile);
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, inputfile);

    int w = bih.biWidth;
    int h = bih.biHeight;
    int size = bih.biSizeImage; // w * h including padding

    BYTE* data = (BYTE *)malloc(size);
    fread(data, size, 1, inputfile);
    fclose(inputfile);
    BYTE* out = (BYTE *) malloc(size);
    // memcpy(out, data, size);
    /* Each pixel is represented by three bytes. 
    The first byte gives the intensity of the red component, 
    the second byte gives the intensity of the green component, 
    and the third byte gives the intensity of the blue component. from */

    // data starts on the bottom left 
    Op op;

    if (operation == "contrast") {
        // spec: factor in [0, 100]
        if (!(constfac >= 0.0 && constfac <= 100.0)) return 3;
        op = Op::Contrast;
    } else if (operation == "saturation") {
        // spec: factor in [0, 1]
        if (!(constfac >= 0.0 && constfac <= 1.0)) return 3;
        op = Op::Saturation;
    } else if (operation == "lightness") {
        // spec: factor in [-1, 1]
        if (!(constfac >= -1.0 && constfac <= 1.0)) return 3;
        op = Op::Lightness;
    } else {
        return 3; // unknown operation
    }

    fwrite(&bfh.bfType,     2, 1, outfile);
    fwrite(&bfh.bfSize,     4, 1, outfile);
    fwrite(&bfh.bfReserved1,2, 1, outfile);
    fwrite(&bfh.bfReserved2,2, 1, outfile);
    fwrite(&bfh.bfOffBits,  4, 1, outfile);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, outfile);
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
            p = { B, G, R };
            FPixel q = normalizePixel(p);

            // switch case with enum class
            switch (op) {
                case Op::Contrast:   q = contrastPixel(q,   constfac); break;
                case Op::Saturation: q = saturatePixel(q,   constfac); break;
                case Op::Lightness:  q = lightenPixel(q,    constfac); break;
            }
            PIXEL o = unnormalizePixel(q);
            out[idx]     = o.b; out[idx + 1] = o.g; out[idx + 2] = o.r;
        }
    }
    
    fwrite(out, size, 1, outfile);
    return 0;
}