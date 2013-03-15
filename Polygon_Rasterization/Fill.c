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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Fill.h"

static void InitDDA(Iter iter, int xs, int xe, int ys, int ye, int cy, int w)
{
	/* Pre-condition: ye > ys */
	div_t q = div(iter->dx = xe - xs, iter->dy = ye - ys);
	iter->y    = ys;
	iter->ye   = ye;
	iter->x    = xs;
	iter->xe   = xe;
	iter->err  = iter->dy >> 1;
	iter->rem  = abs(q.rem);
	iter->quot = q.quot;
	iter->sx   = (xs < xe ? 1 : -1);

	/* Vertical clipping */
	if (cy > ys)
	{
		q = div((cy - ys) * iter->dx, iter->dy);
		iter->x   = xs + q.quot;
		iter->y   = cy;
		iter->err = iter->dy - q.rem + (iter->dy >> 1);
	}

	/* horizontal clipping */
	iter->xc = xs = iter->x;
	iter->yre = iter->ye;
	iter->yse = 0;
	iter->sync = 1;
	if (xs >= w) /* starts clipped (right) */
	{
		if (iter->xe >= w) iter->quot = iter->rem = 0;
		else iter->yre = iter->ye, iter->ye = ys + ((w-xs) * iter->dy + iter->dx - 1) / iter->dx, iter->state = 2;
		iter->sync = 0;
		iter->xc = w;
	}
	else if (xs < 0) /* starts clipped (left) */
	{
		if (iter->xe < 0) iter->quot = iter->rem = 0;
		else iter->yre = iter->ye, iter->ye = ys + (-xs * iter->dy + iter->dx - 1) / iter->dx, iter->state = 1;
		iter->sync = iter->xc = 0;
	}
	if (xe >= w) /* ends clipped (right) */
	{
		ys += (w - xs) * iter->dy / iter->dx;
		if (iter->state) iter->yse = ys;
		else iter->ye = ys, iter->state = 3;
	}
	else if (xe < 0) /* ends clipped (left) */
	{
		ys += xs * iter->dy / -iter->dx;
		if (iter->state) iter->yse = ys;
		else iter->ye = ys, iter->state = 4;
	}

}

static inline void IterDDA(Iter iter)
{
	iter->y ++;
	iter->x += iter->quot;
	iter->err -= iter->rem;
	if (iter->err <= 0)
		iter->x += iter->sx, iter->err += iter->dy;

	if (iter->sync)
		iter->xc = iter->x;
}

/* DDA has been clipped, change its internal state and start next phase */
static int ClipDDA(Iter iter, int w)
{
	if (iter->state > 0) /* at least one more phase */
	{
		if (iter->y < iter->ye) return 0;
		int flg = iter->state - 1;

		iter->ye = iter->yre;
		iter->state = 0;
		if (flg & 2) /* ends clipped */
		{
			iter->quot = iter->rem = iter->sync = 0;
			iter->xc = (flg & 1 ? 0 : w);
		}
		else
		{
			iter->sync = 1;
			iter->xc = iter->x;
			if (iter->yse > 0) /* Phase 3 */
				iter->ye = iter->yse, iter->yse = 0, iter->state = (flg & 1) + 3;
		}
	}
	return ISDDAEND(iter);
}

static int CmpDDA(const void * item1, const void * item2)
{
	Iter dda1 = (Iter) item1, dda2 = (Iter) item2;
	int d = dda1->y - dda2->y;
	if (d) return d;
	d = dda1->x - dda2->x;
	if (d < 0) return -1;
	if (d == 0) { /* same point - compare slope */
		d = dda1->quot - dda2->quot;
		if (d == 0) {
			d = dda1->sx - dda2->sx;
			if (d == 0) {
				if (dda1->dx < 0) d = dda2->rem - dda1->rem;
				else              d = dda1->rem - dda2->rem;
			}
		}
	}
	return d;
}

static void IterDDAS(Iter iter)
{
	iter->x   += iter->quot << BPP;
	iter->y   += 1<<BPP;
	iter->err -= iter->rem << BPP;
	if (iter->err <= 0) {
		div_t res = div(iter->dy - iter->err, iter->dy);
		if (iter->sx < 0) iter->x -= res.quot;
		else iter->x += res.quot;
		iter->err = iter->dy - res.rem;
	}
	if (iter->sync)
		iter->xc = iter->x;
}

int FillPolygon(Image img, int * pts, int cnt, uint32_t col)
{
	uint8_t rgba[4];

	if (cnt < 6 || img == NULL) return 0;
	rgba[0] = col & 0xff; col >>= 8;
	rgba[1] = col & 0xff; col >>= 8;
	rgba[2] = col & 0xff;
	rgba[3] = col >> 8;

	Iter    dda, v;
	int     i, yend, vcnt, w;
	DATA16  scanline;

	w = img->width;
	scanline = calloc(cnt * sizeof *dda + w * 2 + 2, 1);
	if (scanline == NULL) return 0;
	dda = (Iter) (scanline + w + 1);

	for (i = 0, yend = 0, v = dda; i < cnt; i += 2)
	{
		int * sec = i + 2 < cnt ? pts + i + 2 : pts;
		if (sec[0] == NEWPATH) sec = pts;
		if (pts[i] == NEWPATH) {
			pts += ++i; cnt -= i; i = -2;
			continue;
		}
		if (pts[i+1] == sec[1] || (pts[i+1] < 0 && sec[1] < 0)) continue; /* Horizontal line = skip */
		if (pts[i+1] < sec[1])
			InitDDA(v, pts[i], sec[0], pts[i+1], sec[1], MAX(pts[i+1], 0), w << BPP);
		else
			InitDDA(v, sec[0], pts[i], sec[1], pts[i+1], MAX(sec[1], 0), w << BPP);

		if (v->yre > yend) yend = v->yre; v ++;
	}
	if (yend > (img->height<<BPP))
		yend = img->height<<BPP;

	qsort(dda, vcnt = v - dda, sizeof *dda, CmpDDA);

	DATA8 p;
	int   minX, maxX, j, y, ptrcnt;
	int * ptr;

	/* Some variables have already been declared in the segment sorting block */
	for (y = dda->y, ptrcnt = i = 0, minX = w, maxX = 0, p = img->bitmap + (y >> BPP) * img->stride, ptr = NULL; y < yend; )
	{
		Iter iter;
		int  ymax, depth;

		/* Count how many DDAs starts at y and compute ymax */
		for (iter = dda, depth = 0, ymax = dda->ye; depth < vcnt && iter->y == y; depth ++, i ++, iter ++) {
			if (ymax > iter->ye) ymax = iter->ye;
		}
		if (depth < vcnt && ymax > iter->y) ymax = iter->y;
		if (ymax > yend) ymax = yend;
		if (depth == 0) break;
		/* Need to sort again, if DDA have been introduced */
		qsort(dda, depth, sizeof *dda, CmpDDA);

		if (ptr == NULL || ptrcnt < depth)
			ptr = alloca((ptrcnt = (depth+7)&~7) * sizeof *ptr);

		while (y < ymax)
		{
			/* Sort x coordinates */
			for (iter = dda, cnt = 0; cnt < depth; iter ++)
			{
				int x = iter->xc;
				for (j = cnt - 1; j >= 0 && ptr[j] > x; j --); j ++;
				if (j < cnt) memmove(ptr + j + 1, ptr + j, (cnt-j) * sizeof *ptr);
				ptr[j] = x; cnt ++; IterDDA(iter);
			}

			/* Count pixels covered (alpha) */
			for (j = 0; j < cnt; j += 2)
			{
				int x0 = ptr[j]   >> BPP;
				int x1 = ptr[j+1] >> BPP;
				scanline[x0] += VALUES - (ptr[j] & MASK);
				if (minX > x0) minX = x0;
				scanline[x1] += (x0 == x1 ? (ptr[j+1] & MASK) - VALUES : ptr[j+1] & MASK);
				if (maxX < x1) maxX = x1;
				for (x0 ++; x0 < x1; scanline[x0++] += VALUES);
			}
			y ++;

			if ((y & MASK) == 0 || y >= yend)
			{
				/* Convert alpha into RGB */
				DATA16 s;
				DATA8  d;
				if (maxX == w) maxX --;
				for (s = scanline + minX, d = p + minX * img->bpp; minX <= maxX; s++, d += img->bpp, minX ++)
				{
					/*
					 * Note: is you want to take into account the alpha component (rgba[3]) of the polygon color,
					 * replace "v" with the following expression:
					 * uint16_t v = *s * rgba[3] / 255;
					 */
					uint16_t v = *s;
					if (v == 256) d[0] = rgba[0], d[1] = rgba[1], d[2] = rgba[2];
					else d[0] = (rgba[0] * *s + d[0] * (VALUES*VALUES - *s)) >> (BPP*2),
					     d[1] = (rgba[1] * *s + d[1] * (VALUES*VALUES - *s)) >> (BPP*2),
					     d[2] = (rgba[2] * *s + d[2] * (VALUES*VALUES - *s)) >> (BPP*2);
				}
				memset(scanline, 0, w<<1);
				p += img->stride; minX = w; maxX = 0;
			}

		}
		/* One or more DDAs have stopped: remove them from list */
		for (iter = dda, j = 0; j < depth; ) {
			if (ClipDDA(iter, w<<BPP))
				memmove(iter, iter+1, (vcnt - j - 1) * sizeof *iter), vcnt --, depth --;
			else
				iter ++, j ++;
		}
	}
	free(scanline);

	return 1;
}

static int DrawTriangle(DATA16 p, Iter iter, int left)
{
	DATA16 s;
	int x0 = iter->xc, x  = x0>>BPP, xp = iter->x, ret; IterDDAS(iter);
	int x1 = iter->xc, xe = x1>>BPP, v, oldv = VALUES, sz = xp - iter->x;

	#define SETVAL(s, v)   if (left) *s += v; else *s -= v
	#define SURF           (VALUES*VALUES)

	if (left == 0)
		for (ret = MAX(x, xe), s = p + ret; s >= p && *s == 0; *s-- = SURF);
	else
		ret = MIN(x, xe);
	s = p + x;
	if (x == xe)
	{
		v = SURF - (((x0 & MASK) + (x1 & MASK)) << (BPP-1));
		SETVAL(s, v);
	}
	else if (x < xe)
	{
		int sv;
		do {
			xp = (x0 + VALUES) & ~MASK;
			v = ((xp - x1) << BPP) / sz;
			if (oldv == VALUES) sv = (VALUES - v) * (xp - x0) >> 1;
			else                sv = SURF - ((v + oldv) << (BPP-1));
			SETVAL(s, sv);
			x ++; s ++; oldv = v; x0 = xp; xp += VALUES;
		}
		while (x < xe);
		sv = SURF - (v * (x1 - x0) >> 1);
		SETVAL(s, sv);
	}
	else
	{
		int sv;
		xp = x0 & ~MASK;
		do
		{
			v = ((xp - x1) << BPP) / sz;
			if (oldv == VALUES) sv = SURF - ((VALUES - v) * (x0 - xp) >> 1);
			else                sv = (v + oldv) << (BPP-1);
			SETVAL(s, sv);
			x --; s --; oldv = v; x0 = xp; xp -= VALUES;
		}
		while (x > xe);
		sv = v * (x0 - x1) >> 1;
		SETVAL(s, sv);
	}
	#undef SETVAL
	return ret;
}

int FillPolygonFaster(Image img, int * pts, int cnt, uint32_t col)
{
	uint8_t rgba[4];

	if (cnt < 6 || img == NULL) return 0;
	rgba[0] = col & 0xff; col >>= 8;
	rgba[1] = col & 0xff; col >>= 8;
	rgba[2] = col & 0xff;
	rgba[3] = col >> 8;

	Iter    dda, v;
	int     i, yend, vcnt, w;
	DATA16  scanline;

	w = img->width;
	scanline = calloc(cnt * sizeof *dda + w * 2 + 2, 1);
	if (scanline == NULL) return 0;
	dda = (Iter) (scanline + w + 1);

	for (i = 0, yend = 0, v = dda; i < cnt; i += 2)
	{
		int * sec = i + 2 < cnt ? pts + i + 2 : pts;
		if (sec[0] == NEWPATH) sec = pts;
		if (pts[i] == NEWPATH) {
			pts += ++i; cnt -= i; i = -2;
			continue;
		}
		if (pts[i+1] == sec[1] || (pts[i+1] < 0 && sec[1] < 0)) continue; /* Horizontal line = skip */
		if (pts[i+1] < sec[1])
			InitDDA(v, pts[i], sec[0], pts[i+1], sec[1], MAX(pts[i+1], 0), w << BPP);
		else
			InitDDA(v, sec[0], pts[i], sec[1], pts[i+1], MAX(sec[1], 0), w << BPP);

		if (v->yre > yend) yend = v->yre; v ++;
	}
	if (yend > (img->height<<BPP))
		yend = img->height<<BPP;

	qsort(dda, vcnt = v - dda, sizeof *dda, CmpDDA);

	DATA8 p;
	int   minX, maxX, j, y;

	/* Some variables have already been declared in the segment sorting block */
	for (y = dda->y, i = 0, minX = w, maxX = 0, p = img->bitmap + (y >> BPP) * img->stride; y < yend; )
	{
		Iter iter;
		int  ymax, depth;

		/* Count how many DDAs starts at y and compute ymax */
		for (iter = dda, depth = 0, ymax = dda->ye; depth < vcnt && iter->y == y; depth ++, i ++, iter ++) {
			if (ymax > iter->ye) ymax = iter->ye;
		}
		if (depth < vcnt && ymax > iter->y) ymax = iter->y;
		if (ymax > yend) ymax = yend;
		if (depth == 0) break;
		/* Need to sort again, if DDA have been introduced */
		qsort(dda, depth, sizeof *dda, CmpDDA);

		while (y < ymax)
		{
			if ((y & MASK) == 0 && y + VALUES < ymax)
			{
				/* Compute 16 scanlines at once */
				for (iter = dda, j = 0; j < depth; j += 2, iter ++)
				{
					/* Left part */
					int x = DrawTriangle(scanline, iter, 1); iter ++;
					if (minX > x) minX = x;

					/* Right part */
					x = DrawTriangle(scanline, iter, 0);
					if (maxX < x) maxX = x;
				}
				y += VALUES;
			}
			else /* One at a time */
			{
				for (j = 0, iter = dda; j < depth; j += 2)
				{
					int xc0 = iter->xc, x0 = xc0 >> BPP; IterDDA(iter); iter ++;
					int xc1 = iter->xc, x1 = xc1 >> BPP; IterDDA(iter); iter ++;
					scanline[x0] += VALUES - (xc0 & MASK);
					if (minX > x0) minX = x0;
					scanline[x1] += (x0 == x1 ? (xc1 & MASK) - VALUES : xc1 & MASK);
					if (maxX < x1) maxX = x1;
					for (x0 ++; x0 < x1; scanline[x0++] += VALUES);
				}
				y ++;
			}

			if ((y & MASK) == 0 || y >= yend)
			{
				/* Convert alpha into RGB */
				DATA16 s;
				DATA8  d;
				if (maxX == w) maxX --;
				for (s = scanline + minX, d = p + minX * img->bpp; minX <= maxX; s++, d += img->bpp, minX ++)
				{
					/*
					 * Note: is you want to take into account the alpha component (rgba[3]) of the polygon color,
					 * replace "v" with the following expression:
					 * uint16_t v = *s * rgba[3] / 255;
					 */
					uint16_t v = *s;
					if (v == 256) d[0] = rgba[0], d[1] = rgba[1], d[2] = rgba[2];
					else d[0] = (rgba[0] * *s + d[0] * (VALUES*VALUES - *s)) >> (BPP*2),
					     d[1] = (rgba[1] * *s + d[1] * (VALUES*VALUES - *s)) >> (BPP*2),
					     d[2] = (rgba[2] * *s + d[2] * (VALUES*VALUES - *s)) >> (BPP*2);
				}
				memset(scanline, 0, w<<1);
				p += img->stride; minX = w; maxX = 0;
			}
		}
		/* One or more DDAs have stopped: remove them from list */
		for (iter = dda, j = 0; j < depth; ) {
			if (ClipDDA(iter, w<<BPP))
				memmove(iter, iter+1, (vcnt - j - 1) * sizeof *iter), vcnt --, depth --;
			else
				iter ++, j ++;
		}
	}
	free(scanline);

	return 1;
}
