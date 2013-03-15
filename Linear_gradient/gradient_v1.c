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

#include <stdlib.h>
#include <SDL/SDL.h>
#include "gradient_v1.h"

typedef struct Iter_t          Iter;

struct Iter_t
{
	int x, y, xe, ye;
	int dx, dy, err, sx, sy, oldy;
};


void InitDDA(Iter * iter, int xs, int xe, int ys, int ye)
{
	/* Pre-condition: xe > xs >= 0 */
	div_t q = div(iter->dy = ye - ys, xe);
	iter->y   = ys;
	iter->ye  = ye;
	iter->x   = xs;
	iter->xe  = xe;
	iter->err = xe >> 1;
	iter->dx  = abs(q.rem);
	iter->sx  = q.quot;
	iter->sy  = (ys < ye ? 1 : -1);
	iter->oldy = -1;
	if (xs > 0)
	{
		q = div(xs * iter->dy + (xe >> 1), xe);
		iter->y   = ys + q.quot;
		iter->err = xe - q.rem;
	}
}

static inline void IterDDA(Iter * iter)
{
	iter->oldy = iter->y;
	iter->x ++;
	iter->y += iter->sx;
	iter->err -= iter->dx;
	if (iter->err < 0) iter->y += iter->sy, iter->err += iter->xe;
}
#define ISDDAEND(iter) ((iter).x >= (iter).xe)

static void DrawScanline(Image img, int x, int y, ColorStop cs, int count, int max);


int LinearGradient(Image img, int x1, int y1, int x2, int y2, ColorStop cs, int count)
{
	int adj, opp, g, i;

	/* Check for sane parameters */
	if (x1 == x2 && y1 == y2) return 0;
	if (count < 2) return 0;

	adj = x2 - x1;
	opp = y2 - y1;
	g = (adj*adj+opp*opp) / adj;

	/* Adjust range of cs.dist for each color from [0, 100] to [0, g] */
	for (i = 0; i < count; i ++)
	{
		int dist;
		if (i == 0) dist = 0; else
		if (i == count-1) dist = g; else
		if (cs[i].dist == 0) dist = i * g / (count-1);
		else dist = cs[i].dist * g / 100;
		cs[i].dist = dist;
	}

	Iter pos;
	InitDDA(&pos, 0, y1, x1 + y1 * opp / adj, x1);
	while (pos.x < img->height)
	{
		DrawScanline(img, pos.y, pos.x, cs, count, g);
		IterDDA(&pos);
	}

	return 1;
}

#define SETU32(dst, src)      * (uint32_t *) (dst) = * (uint32_t *) (src)


/* Draw scanline 'y', starting gradient at column 'x', over 'max' pixels */
static void DrawScanline(Image img, int x, int y, ColorStop cs, int count, int max)
{
	DATA8 p = img->bitmap + img->stride * y;
	ColorStop c = cs;
	Iter r, g, b, a;
	int i, w = img->width;

	if (x > 0) {
		/* Set first iter->x px to color c0 */
		uint8_t rgb[4];
		memcpy(rgb, c->rgba, 4);
		for (i = x; i > 0; SETU32(p, rgb), p += 4, i --);
	}

	/* Skip first -x px */
	if (x < 0) {
		for (c ++, i = 1; -x > c->dist && i < count; c ++, i ++);
		if (i < count) x += c[-1].dist;
		i = -x; x = 0;
	}
	else i = 0, c++;

	/* Draw gradient */
	do {
		int len = c->dist - c[-1].dist;
		InitDDA(&r, i, len, c[-1].rgba[3], c->rgba[3]);
		InitDDA(&g, i, len, c[-1].rgba[2], c->rgba[2]);
		InitDDA(&b, i, len, c[-1].rgba[1], c->rgba[1]);
		InitDDA(&a, i, len, c[-1].rgba[0], c->rgba[0]);

		do {
			p[3] = r.y; IterDDA(&r);
			p[2] = g.y; IterDDA(&g);
			p[1] = b.y; IterDDA(&b);
			p[0] = a.y; IterDDA(&a); x ++; p += img->bpp;
			if (ISDDAEND(r)) {
				if (c->dist == max) {
					/* Keep using the last color, up to the end of line */
					r.sx = r.err = r.dx = 0;
					g.sx = g.err = g.dx = 0;
					b.sx = b.err = b.dx = 0;
					a.sx = a.err = a.dx = 0;
					r.xe = 1<<30;
					if (x < w) continue;
				}
				else c ++;
				break;
			}
		}
		while (x < w);
	} while (x < w);
}

int main (int argc, char * argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	atexit(SDL_Quit);

	SDL_Surface * screen = SDL_SetVideoMode(500, 400, 32, SDL_HWSURFACE|SDL_ANYFORMAT);
	if (! screen) return 1;

	struct Image_t img = {
		.bitmap = screen->pixels, .stride = screen->pitch, .width = screen->w, .height = screen->h,
		.bpp = screen->format->BytesPerPixel
	};

	/* Simple red to blue gradient */
	#define  NBCOL     2
	struct ColorStop_t colors[NBCOL] = {{{255, 0, 0}}, {{0, 20, 255}}};
	int i;

	/* We need to convert colors into native format */
	for (i = 0; i < NBCOL; i ++)
	{
		DATA8    rgba = colors[i].rgba;
		uint32_t col = SDL_MapRGBA(screen->format, rgba[0], rgba[1], rgba[2], rgba[3]);
		SETU32(colors[i].rgba, &col);
	}

	LinearGradient(&img, 50, 100, 450, 350, colors, NBCOL);
	SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);

	while (1)
	{
		SDL_Event event;
		while (SDL_WaitEvent(&event))
		{
			switch (event.type) {
			case SDL_QUIT: return 0;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) return 0;
			}
		}
	}
	return 0;
}

