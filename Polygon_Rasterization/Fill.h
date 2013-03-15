/* Copyright (c) 2013 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Polygon_Rasterization_(C)?action=history&offset=20130112034554

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

Retrieved from: http://en.literateprograms.org/Polygon_Rasterization_(C)?oldid=18805
*/

#ifndef POLY_FILL_H
#define POLY_FILL_H

#include <stdint.h>

typedef struct Image_t *       Image;
typedef uint8_t *              DATA8;
typedef uint16_t *             DATA16;

#define BPP        4
#define VALUES     (1 << BPP)
#define MASK       (VALUES-1)
#define EPSILON    (1/256.)
#define NEWPATH    (0xfffff << BPP)
#define FTOI(v)    (((int)(v) << BPP) | (int) ((v) * (1 << BPP)))
#ifndef	MIN
#define	MIN(a, b)          ((a) < (b) ? (a) : (b))
#endif
#ifndef	MAX
#define	MAX(a, b)          ((a) > (b) ? (a) : (b))
#endif

struct Image_t
{
	int   width, height, bpp;
	int   stride;
	DATA8 bitmap;
};

int FillPolygon(Image img, int * pts, int cnt, uint32_t col);

typedef struct Iter_t *        Iter;

struct Iter_t
{
	int     x, xc, y, err, sx, rem, quot;
	int     dx, dy, xe, ye, yre, yse;
	uint8_t state, sync;
};

#define ISDDAEND(iter) ((iter)->y >= (iter)->ye)


int FillPolygonFaster(Image img, int * pts, int cnt, uint32_t col);
#endif

