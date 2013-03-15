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

#include <stdlib.h>
#include "floyd_steinberg_dither.h"

#define plus_truncate_uchar(a, b) \
    if (((int)(a)) + (b) < 0) \
        (a) = 0; \
    else if (((int)(a)) + (b) > 255) \
        (a) = 255; \
    else \
        (a) += (b);


static
unsigned char FindNearestColor(RGBTriple color, RGBPalette palette) {
    int i, distanceSquared, minDistanceSquared, bestIndex = 0;
    minDistanceSquared = 255*255 + 255*255 + 255*255 + 1;
    for (i=0; i<palette.size; i++) {
        int Rdiff = ((int)color.R) - palette.table[i].R;
        int Gdiff = ((int)color.G) - palette.table[i].G;
        int Bdiff = ((int)color.B) - palette.table[i].B;
        distanceSquared = Rdiff*Rdiff + Gdiff*Gdiff + Bdiff*Bdiff;
        if (distanceSquared < minDistanceSquared) {
            minDistanceSquared = distanceSquared;
            bestIndex = i;
        }
    }
    return bestIndex;
}

#define compute_disperse(channel) \
error = ((int)(currentPixel->channel)) - palette.table[index].channel; \
if (x + 1 < image.width) { \
    plus_truncate_uchar(image.pixels[(x+1) + (y+0)*image.width].channel, (error*7) >> 4); \
} \
if (y + 1 < image.height) { \
    if (x - 1 > 0) { \
        plus_truncate_uchar(image.pixels[(x-1) + (y+1)*image.width].channel, (error*3) >> 4); \
    } \
    plus_truncate_uchar(image.pixels[(x+0) + (y+1)*image.width].channel, (error*5) >> 4); \
    if (x + 1 < image.width) { \
        plus_truncate_uchar(image.pixels[(x+1) + (y+1)*image.width].channel, (error*1) >> 4); \
    } \
}

PalettizedImage FloydSteinbergDither(RGBImage image, RGBPalette palette)

{
    PalettizedImage result;
    result.width = image.width;
    result.height = image.height;
    result.pixels = malloc(sizeof(unsigned char) * result.width * result.height);

    {
    int x, y;
    for(y = 0; y < image.height; y++) {
        for(x = 0; x < image.width; x++) {
        RGBTriple* currentPixel = &(image.pixels[x + y*image.width]);
        unsigned char index = FindNearestColor(*currentPixel, palette);
	result.pixels[x + y*result.width] = index;

        {
	    int error;
	    compute_disperse(R);
	    compute_disperse(G);
	    compute_disperse(B);
	}
        }
    }
    }
    return result;
}

