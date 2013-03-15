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
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include "gradient_general.h"

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

static void DrawScanline(Image img, GradState * state, int max);
static void AntiAlias(Image img, int err, int pos);
static void AntiAliasV(Image img, int err, int pos, DATA8 src);

int LinearGradient(Image img, int x1, int y1, int x2, int y2, ColorStop cs, int count)
{
	int adj, opp, g, i, horz, inc;
	ColorStop colors, c;
	GradState state;

	/* Check for sane parameters */
	if (x1 == x2 && y1 == y2) return 0;
	if (count < 2) return 0;

	adj = x2 - x1;
	opp = y2 - y1;
	horz = abs(adj) >= abs(opp);
	state.dir = img->bpp;
	if (! horz) { swap(opp, adj); swap(x1, y1); swap(x2, y2); }
	if (opp * adj < 0) {
		state.dir = - state.dir;
		i = horz ? img->width : img->height;
		x1 = i - x1; x2 = i - x2;
		adj = x2 - x1;
	}
	g = (adj*adj+opp*opp) / adj;
	c = colors = alloca(sizeof *c * count);
	inc = 1;
	state.c = c;
	state.step = count;
	if (g < 0) c += count-1, inc = -1, c->dist = abs(g);
	else colors->dist = 0;

	/* Adjust range of cs.dist for each color from [0, 100] to [0, g] */
	for (i = 0; i < count; i ++, c += inc)
	{
		int dist;
		SETU32(c->rgba, cs[i].rgba);
		if (i == 0) dist = 0; else
		if (i == count-1) dist = g; else
		if (cs[i].dist == 0) dist = i * g / (count-1);
		else dist = cs[i].dist * g / 100;
		if (inc > 0)
		{
			if (dist < 0) dist = -dist;
		} else {
			if (g < 0) dist -= g;
			else dist = g - dist;
		}
		c->dist = dist;
	}

	/* Draw the gradient: init DDA to scan perpendicular values */
	InitDDA(&state.pos, 0, y1, x1 + y1 * opp / adj, x1);
	state.pos.err = state.pos.xe;
	state.err = state.dir * state.pos.sy;
	if (horz)
	{
		int err[2], pos = 0, error;
		while (state.pos.x < img->height)
		{
			if (state.err < 0) error = (state.pos.err << 16) / state.pos.xe;
			else               error = ((state.pos.xe - state.pos.err) << 16) / state.pos.xe;
			err[state.pos.x&1] = error;

			DrawScanline(img, &state, g);
			if (state.pos.x > 0)
				AntiAlias(img, error = err[(state.pos.x-1)&1], pos), pos += img->stride;
			IterDDA(&state.pos);
		}
	}
	else /* iterate over vertical axis */
	{
		int   chan = img->bpp, max = img->width, error, pos;
		Image imgtmp = malloc(sizeof *img + img->height * chan * 2);
		DATA8 line;
		if (imgtmp == NULL) return 0;

		memcpy(imgtmp, img, sizeof *img);
		imgtmp->bitmap = line = (DATA8) (imgtmp+1);
		imgtmp->width  = img->height;
		imgtmp->height = img->width;
		imgtmp->stride = img->height * chan;
		pos = 0;
		do {
			if (state.err < 0) error = (state.pos.err << 16) / state.pos.xe;
			else               error = ((state.pos.xe - state.pos.err) << 16) / state.pos.xe;
			DrawScanline(imgtmp, &state, g);
			AntiAliasV(img, error, pos, line); pos += chan;
			if (state.pos.x > 0)
				memcpy(imgtmp->bitmap, line = imgtmp->bitmap + imgtmp->stride, imgtmp->stride);
			IterDDA(&state.pos); state.pos.x = 1; max --;
		}
		while (max > 0);
		free(imgtmp);
	}
	return 1;
}

static void AntiAlias(Image img, int err, int pos)
{
	DATA8 p = img->bitmap + pos;
	int   c = img->bpp;
	int   i = img->width-1;
	if (err == 0) return;
	for (p += i*c, c = -c; i > 0; i --, p += c) {
		p[0] = ((65536 - err) * p[0] + err * p[c]   + 65536/2) >> 16;
		p[1] = ((65536 - err) * p[1] + err * p[c+1] + 65536/2) >> 16;
		p[2] = ((65536 - err) * p[2] + err * p[c+2] + 65536/2) >> 16;
		p[3] = ((65536 - err) * p[3] + err * p[c+3] + 65536/2) >> 16;
	}
}

/* Copy scanline into column and apply anti-aliasing */
static void AntiAliasV(Image img, int err, int pos, DATA8 src)
{
	DATA8 p = img->bitmap + pos;
	int   c = img->stride, inc = - img->bpp;
	int   i = img->height-1;
	for (p += i*c, c = -c, src += i * -inc; i > 0; i --, p += c, src += inc) {
		p[0] = ((65536 - err) * src[0] + err * src[inc]   + 65536/2) >> 16;
		p[1] = ((65536 - err) * src[1] + err * src[inc+1] + 65536/2) >> 16;
		p[2] = ((65536 - err) * src[2] + err * src[inc+2] + 65536/2) >> 16;
		p[3] = ((65536 - err) * src[3] + err * src[inc+3] + 65536/2) >> 16;
	}
	SETU32(p, src);
}

/* Draw scanline 'pos.x', starting gradient at column 'pos.y', over 'max' pixels */
static void DrawScanline(Image img, GradState * state, int max)
{
	DATA8 p = img->bitmap + img->stride * state->pos.x;
	ColorStop c = state->c;
	int i, diff, w = img->width, chan = img->bpp;
	int x = state->pos.y;

	if (max < 0) x += max, max = -max;
	if (state->pos.sy < 0 && state->pos.err > 0) x --;

	/* Horizontal predictor */
	if (state->pos.x > 0)
	{
		int size;
		diff = state->pos.y - state->pos.oldy;
		w    = abs(diff);
		size = (img->width - w) * chan;
		if (state->dir < 0) diff = - diff;
		if (diff < 0) {
			memcpy(p, p - img->stride + chan, size);
			p += (img->width - 1) * chan;
		}
		else memcpy(p + chan * diff, p - img->stride, size);
		/* No differences between this line and the previous one */
		if (w == 0) return;
		x = 0; chan = state->dir;
	}
	else /* First line */
	{
		chan = state->dir;
		if (chan < 0) p += (img->width - 1) * -chan;
		if (x > 0)
			/* Set first iter->x px to color c0 */
			for (i = x; i > 0; SETU32(p, c->rgba), p += chan, i --);

		/* Skip first -x px */
		if (x < 0) {
			for (c ++, i = state->step-1; -x > c->dist && i > 0; c ++, i --);
			x += c[-1].dist;
			i = -x; x = 0;
		}
		else i = 0, c++;

		diff = c->dist - c[-1].dist;
		InitDDA(&state->r, i, diff, c[-1].rgba[3], c->rgba[3]);
		InitDDA(&state->g, i, diff, c[-1].rgba[2], c->rgba[2]);
		InitDDA(&state->b, i, diff, c[-1].rgba[1], c->rgba[1]);
		InitDDA(&state->a, i, diff, c[-1].rgba[0], c->rgba[0]);
	}

	/* Draw gradient */
	do {
		p[3] = state->r.y; IterDDA(&state->r);
		p[2] = state->g.y; IterDDA(&state->g);
		p[1] = state->b.y; IterDDA(&state->b);
		p[0] = state->a.y; IterDDA(&state->a); x ++; p += chan;
		if (ISDDAEND(state->r)) {
			if (c->dist == max) {
				/* Keep using the last color, up to the end of line */
				state->r.sx = state->r.err = state->r.dx = 0;
				state->g.sx = state->g.err = state->g.dx = 0;
				state->b.sx = state->b.err = state->b.dx = 0;
				state->a.sx = state->a.err = state->a.dx = 0;
				state->r.xe = 1<<30;
				if (x < w) continue; else break;
			}
			else c ++;
			/* Next color stop */
			diff = c->dist - c[-1].dist;
			InitDDA(&state->r, 0, diff, c[-1].rgba[3], c->rgba[3]);
			InitDDA(&state->g, 0, diff, c[-1].rgba[2], c->rgba[2]);
			InitDDA(&state->b, 0, diff, c[-1].rgba[1], c->rgba[1]);
			InitDDA(&state->a, 0, diff, c[-1].rgba[0], c->rgba[0]);
		}
	} while (x < w);
	state->c = c;
}

