/* Copyright (c) 2013 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Linear_gradient_(C)?action=history&offset=20120503193709

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

Retrieved from: http://en.literateprograms.org/Linear_gradient_(C)?oldid=18540
*/

#ifndef LINEAR_GRADIENT_H
#define LINEAR_GRADIENT_H

typedef struct Image_t *       Image;
typedef uint8_t *              DATA8;

struct Image_t
{
	int   width, height, bpp;
	int   stride;
	DATA8 bitmap;
};

typedef struct ColorStop_t *   ColorStop;

struct ColorStop_t
{
	uint8_t rgba[4];
	int     dist;
};

typedef struct Iter_t          Iter;

struct Iter_t
{
	int x, y, xe, ye;
	int dx, dy, err, sx, sy, oldy;
};


typedef struct GradState_t     GradState;

struct GradState_t
{
	Iter r, g, b, a, pos;
	int step, dir, err;
	ColorStop c;
};

int LinearGradient(Image img, int x1, int y1, int x2, int y2, ColorStop cs, int count);

#ifndef SETU32
#define SETU32(dst, src)      * (uint32_t *) (dst) = * (uint32_t *) (src)

#endif
#ifndef swap
#define swap(a, b) do { int temp = a; a = b; b = temp; } while(0)
#endif

#endif
