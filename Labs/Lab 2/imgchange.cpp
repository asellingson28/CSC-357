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

int main() {
    
    return 0;
}