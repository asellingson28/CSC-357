#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <cmath>
#include <sys/wait.h>
#include <unistd.h>
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


PIXEL get_color(BYTE* data, int x, int y, int w, int h) {
    PIXEL p = {0, 0, 0}; // default (black) for out-of-bounds

    if (x < 0 || y < 0 || x >= w || y >= h) return p;

    int rowStride = w * 3 + (4 - (w * 3) % 4) % 4; 
    int j = y * rowStride + x * 3;                 

    BYTE B = data[j];
    BYTE G = data[j + 1];
    BYTE R = data[j + 2];
    p = {B, G, R};

    return p;
}
/**
 * @brief 
 * 
 * @param smallerimagedata 
 * @param largerimagedata 
 * @param ratio 
 * @param w1 
 * @param h1 
 * @param currentstart 
 * @param currentmax 
 * @param w2 
 * @param h2 
 * @param out 
 */
void call_merge(BYTE* smallerimagedata, BYTE* largerimagedata, float ratio, int w1, int h1, int currentstart, int currentmax, int w2, int h2, BYTE* out) {
    // cout << q << " " << currentstart << " " << currentmax << endl;
    for (int y = currentstart; y < currentmax; y++)
    {
        for (int x = 0; x < w1; x++)
        {
            int rowStride = w1 * 3 + (4 - (w1 * 3) % 4) % 4;
            int i = y * rowStride + x * 3;

            float sx = (float)w2 / (float)w1;
            float sy = (float)h2 / (float)h1;
            float xfloat = (x + 0.5f)*sx - 0.5f;
            float yfloat = (y + 0.5f)*sy - 0.5f;

            
            float x1, x2, y1, y2;
            x1 = floor(xfloat);
            x2 = ceil(xfloat);
            y1 = floor(yfloat);
            y2 = ceil(yfloat);

            float dx = (xfloat - x1) / (x2 - x1);
            float dy = (yfloat - y1) / (y2 - y1);

            PIXEL p_left_upper = get_color(smallerimagedata, (int) x1, (int) y2,w2, h2);
            PIXEL p_right_upper = get_color(smallerimagedata, (int) x2, (int) y2,w2, h2);
            PIXEL p_left_lower = get_color(smallerimagedata, (int) x1, (int) y1,w2, h2);
            PIXEL p_right_lower = get_color(smallerimagedata, (int) x2, (int) y1,w2, h2);

            BYTE red_left_upper = p_left_upper.r;
            BYTE red_right_upper = p_right_upper.r;
            BYTE red_left_lower = p_left_lower.r;
            BYTE red_right_lower = p_right_lower.r;
            BYTE blue_left_upper = p_left_upper.b;
            BYTE blue_right_upper = p_right_upper.b;
            BYTE blue_left_lower = p_left_lower.b;
            BYTE blue_right_lower = p_right_lower.b;
            BYTE green_left_upper = p_left_upper.g;
            BYTE green_right_upper = p_right_upper.g;
            BYTE green_left_lower = p_left_lower.g;
            BYTE green_right_lower = p_right_lower.g;

            BYTE red_left = red_left_upper * (dy) + red_left_lower * (1-dy);
            BYTE red_right = red_right_upper * (dy) + red_right_lower * (1-dy);
            BYTE red_result = red_left * (1-dx) + red_right * (dx);

            BYTE green_left = green_left_upper * (dy) + green_left_lower * (1-dy);
            BYTE green_right = green_right_upper * (dy) + green_right_lower * (1-dy);
            BYTE green_result = green_left * (1-dx) + green_right * (dx);

            BYTE blue_left = blue_left_upper * (dy) + blue_left_lower * (1-dy);
            BYTE blue_right = blue_right_upper * (dy) + blue_right_lower * (1-dy);
            BYTE blue_result = blue_left * (1-dx) + blue_right * (dx);

            
            PIXEL final {blue_result, green_result, red_result};
            PIXEL orig = get_color(largerimagedata, x, y, w1, h1);
            
            // out[i] = (final.r + orig.r)/2;
            // out[i + 1] = (final.g + orig.g)/2;
            // out[i + 2] = (final.b + orig.b)/2;
            float inv = 1.0f - ratio;
            out[i + 0] = (orig.b * inv + final.b * ratio); 
            out[i + 1] = (orig.g * inv + final.g * ratio); 
            out[i + 2] = (orig.r * inv + final.r * ratio); 
        }
    }
}

int main(int argc, char *argv[]) {
    timeval time_initial;
    gettimeofday(&time_initial, 0);
    if (argc != 6) return 1;

    std::string imgname1 = argv[1];
    std::string imgname2 = argv[2];
    float ratio = atof(argv[3]);
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    int processes = atoi(argv[4]);
    if (processes> 4) processes = 4;
    if (processes<1) processes = 1;
    std::string output = argv[5];

    FILE *inputfile1 = fopen(imgname1.c_str(), "rb");
    FILE *inputfile2 = fopen(imgname2.c_str(), "rb");
    FILE *outfile = fopen(output.c_str(), "wb");
    
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
  // ---- allocate image buffers (unchanged) ----
    BYTE *largerimagedata  = (BYTE *)mmap(NULL, size1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    BYTE *smallerimagedata = (BYTE *)mmap(NULL, size2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // fseek(inputfile1, bfh.bfOffBits, SEEK_SET);
    fread(largerimagedata, size1, 1, inputfile1);

    // fseek(inputfile2, bfh2.bfOffBits, SEEK_SET);
    fread(smallerimagedata, size2, 1, inputfile2);

    if (size2 > size1) {
        // swap headers
        BITMAPFILEHEADER tmpbfh = bfh;  bfh = bfh2;  bfh2 = tmpbfh;
        BITMAPINFOHEADER tmpbih = bih;  bih = bih2;  bih2 = tmpbih;

        // swap geometry & sizes
        int t;
        t = w1; w1 = w2; w2 = t;
        t = h1; h1 = h2; h2 = t;
        t = size1; size1 = size2; size2 = t;

        // swap pixel buffers so "largerimagedata" really holds the larger image
        BYTE* tb = largerimagedata; largerimagedata = smallerimagedata; smallerimagedata = tb;
    }

    // ---- allocate OUT AFTER potential swap, using the (possibly new) size1 ----
    BYTE *out = (BYTE *)mmap(NULL, size1, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // ---- rest stays the same ----
    int currentmax = (h1) / processes;
    int currentstart = 0;
    int rows_per = h1 / processes;

    for (int q = 0; q < processes; q++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (size1 > size2) {
                call_merge(smallerimagedata, largerimagedata, ratio, w1, h1, currentstart, currentmax, w2, h2, out);
            } else {
                call_merge(largerimagedata, smallerimagedata, ratio, w2, h2, currentstart, currentmax, w1, h1, out);
            }
            _exit(0);
        }
        currentstart += rows_per;
        currentmax   += rows_per;
        if (q == processes - 2) currentmax = h1;
    }
    while (wait(0) != -1);

    // write headers from (possibly swapped) image1 and its pixel data
    fwrite(&bfh.bfType, 2, 1, outfile);
    fwrite(&bfh.bfSize, 4, 1, outfile);
    fwrite(&bfh.bfReserved1, 2, 1, outfile);
    fwrite(&bfh.bfReserved2, 2, 1, outfile);
    fwrite(&bfh.bfOffBits, 4, 1, outfile);
    fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, outfile);
    fwrite(out, size1, 1, outfile);

    // right now this makes an exact copy of img1
    
    // similar to lab 3 using mmap for sharing
    // FILE *inputfile2 = fopen(imgname2.c_str(), "rb");
    timeval time_final;
    gettimeofday(&time_final, 0);
    long ms = time_final.tv_usec - time_initial.tv_usec;
    ms += (time_final.tv_sec - time_initial.tv_sec) * 1000000; // convert to microseconds
    cout << ms << " microseconds" << endl;
    cout << "Output: " << output << endl;
    return 0;
}