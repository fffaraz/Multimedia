/* Copyright (c) 2013 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Floyd-Steinberg_dithering_(C)?action=history&offset=20121129211153

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Floyd-Steinberg_dithering_(C)?oldid=18738
*/

#include <stdio.h>
#include <stdlib.h>
#include "floyd_steinberg_dither.h"

int main(int argc, char* argv[]) {
    int x, y;
    RGBImage image;
    RGBPalette palette;
    PalettizedImage result;
    FILE * raw_in, * raw_out;

    image.width = 100;
    image.height = 145;
    image.pixels = malloc(sizeof(RGBTriple) *
                   image.width * image.height);

    raw_in = fopen(argv[1], "rb");
    for(y = 0; y < image.height; y++) {
        for(x = 0; x < image.width; x++) {
            fread(&image.pixels[x + y*image.width],
                  3, 1, raw_in);
        }
    }
    fclose(raw_in);

    palette.size = 16;
    palette.table = malloc(sizeof(RGBTriple) * 16);
    palette.table[0].R = 149;
    palette.table[0].G = 91;
    palette.table[0].B = 110;
    palette.table[1].R = 176;
    palette.table[1].G = 116;
    palette.table[1].B = 137;
    palette.table[2].R = 17;
    palette.table[2].G = 11;
    palette.table[2].B = 15;
    palette.table[3].R = 63;
    palette.table[3].G = 47;
    palette.table[3].B = 69;
    palette.table[4].R = 93;
    palette.table[4].G = 75;
    palette.table[4].B = 112;
    palette.table[5].R = 47;
    palette.table[5].G = 62;
    palette.table[5].B = 24;
    palette.table[6].R = 76;
    palette.table[6].G = 90;
    palette.table[6].B = 55;
    palette.table[7].R = 190;
    palette.table[7].G = 212;
    palette.table[7].B = 115;
    palette.table[8].R = 160;
    palette.table[8].G = 176;
    palette.table[8].B = 87;
    palette.table[9].R = 116;
    palette.table[9].G = 120;
    palette.table[9].B = 87;
    palette.table[10].R = 245;
    palette.table[10].G = 246;
    palette.table[10].B = 225;
    palette.table[11].R = 148;
    palette.table[11].G = 146;
    palette.table[11].B = 130;
    palette.table[12].R = 200;
    palette.table[12].G = 195;
    palette.table[12].B = 180;
    palette.table[13].R = 36;
    palette.table[13].G = 32;
    palette.table[13].B = 27;
    palette.table[14].R = 87;
    palette.table[14].G = 54;
    palette.table[14].B = 45;
    palette.table[15].R = 121;
    palette.table[15].G = 72;
    palette.table[15].B = 72;

    result = FloydSteinbergDither(image, palette);

    raw_out = fopen(argv[2], "wb");
    for(y = 0; y < result.height; y++) {
        for(x = 0; x < result.width; x++) {
            fwrite(&palette.table[result.pixels[x + y*image.width]],
                   3, 1, raw_out);
        }
    }
    fclose(raw_out);

    return 0;
}
