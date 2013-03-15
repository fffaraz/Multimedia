/* Copyright (c) 2013 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Image_downsampling_(C)?action=history&offset=20120403140701

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

Retrieved from: http://en.literateprograms.org/Image_downsampling_(C)?oldid=18185
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "DownScale.h"

static void InitDDA(Iter * iter, int xs, int xe, int ys, int ye)
{
	/* Pre-condition: xe > xs >= 0 */
	div_t q = div(iter->dy = ye - ys, xe);
	iter->y   = ys;
	iter->ye  = ye;
	iter->x   = xs;
	iter->xe  = xe;
	iter->err = xe;
	iter->dx  = abs(q.rem);
	iter->sx  = q.quot;
	iter->sy  = (ys < ye ? 1 : -1);
	if (xs > 0)
		iter->y   = ye-(iter->dy * (xe-xs) + iter->sy * iter->err)/xe,
		iter->err = (iter->err-iter->dx*xs) % xe + xe;
}

static inline void IterDDA(Iter * iter)
{
	iter->x ++;
	iter->y += iter->sx;
	iter->err -= iter->dx;
	if (iter->err <= 0)
		iter->y += iter->sy, iter->err += iter->xe;
}

#define	BITS         8
#define	VALUES       (1 << BITS)

int ImageDownScale(Image src, Image ret)
{
    DATA32 sum;
    int    i, y, w, h, chan, sz, surf, xerr;

    DATA8 out = ret->bitmap;
    DATA8 in  = src->bitmap;
    Iter  ypos, xpos, nerr;

    sz = src->width * src->bpp;
    if (ret->width == 0 || ret->height == 0) return 0;
    if (ret->width > src->width || ret->height > src->height) return 0;
    if (ret->width == src->width && ret->height == src->height) {
        for (y = ret->height; y > 0; y --, in += src->stride, out += ret->stride)
            memcpy(out, in, sz);
        return 1;
    }

    w    = ret->width;
    h    = ret->height;
    chan = src->bpp;
    sum  = calloc(sz, sizeof *sum);
    surf = (unsigned long long) src->width * src->height * VALUES / (w * h);
    y    = 0;

    if (sum == NULL) return 0;
    memset(&nerr, 0, sizeof nerr); nerr.err = 1;
    InitDDA(&ypos, 0, h, 0, src->height);
    InitDDA(&xpos, 0, w, 0, src->width); xerr = xpos.dx > 0;

    while (ypos.x < ypos.xe)
    {
        DATA8 d = out, p;
        int x, yerr;
        int total[4], tmp;
        DATA32 s;
        IterDDA(&ypos);
        yerr = (ypos.xe - ypos.err) * VALUES / ypos.xe;

        while (y < ypos.y)
        {
            for (p = in, i = sz, s = sum; i > 0; *s++ += (*p<<BITS), p++, i --);
            y ++; in += src->stride;
        }

        InitDDA(&xpos, 0, w, 0, src->width); x = 0;      IterDDA(&xpos);
        InitDDA(&nerr, 0, xpos.xe, 0, VALUES * xpos.dx); IterDDA(&nerr);
        memset(total, 0, sizeof total);
        if (yerr > 0)
        {
            #define MAX_255(ptr, val) { int z = val; *ptr = (z >= 255 ? 255 : z); ptr ++; }
            for (p = in, i = ret->width, s = sum; i > 0; ) {
                if (x < xpos.y) {
                    /* Vertical error compensation */
                    switch (chan) {
                    case 4: tmp = *p * yerr; total[3] += tmp + *s; *s = (*p<<BITS) - tmp; s ++; p ++;
                    case 3: tmp = *p * yerr; total[2] += tmp + *s; *s = (*p<<BITS) - tmp; s ++; p ++;
                            tmp = *p * yerr; total[1] += tmp + *s; *s = (*p<<BITS) - tmp; s ++; p ++;
                    case 1: tmp = *p * yerr; total[0] += tmp + *s; *s = (*p<<BITS) - tmp; s ++; p ++;
                    }
                    x ++;
                } else {
                    int err = nerr.y & (VALUES-1);
                    if (xerr == 0 || err == 0) {
                        switch (chan) {
                        case 4: MAX_255(d, (total[3] + (surf>>1)) / surf);
                        case 3: MAX_255(d, (total[2] + (surf>>1)) / surf);
                                MAX_255(d, (total[1] + (surf>>1)) / surf);
                        case 1: MAX_255(d, (total[0] + (surf>>1)) / surf);
                        }
                        memset(total, 0, sizeof total);
                    } else {
                        int k;
                        /* Vertical and horizontal error compensation */
                        for (k = chan-1; k >= 0; k --, p ++, s++) {
                            tmp = *p * yerr;
                            int tmp2 = tmp * err >> BITS;
                            int right = *s * err >> BITS;
                            MAX_255(d, (total[k] + tmp2 + (surf>>1) + right) / surf);
                            total[k] = *s - right + tmp - tmp2;
                            *s = (*p<<BITS) - tmp;
                        }
                        x++;
                    }
                    IterDDA(&nerr);
                    IterDDA(&xpos); i --;
                }
            }
            y ++; in += src->stride;
        }
        else /* No vertical error (maybe horizontal) */
        {
            for (i = ret->width, s = sum; i > 0; ) {
                if (x < xpos.y) {
                    /* No error compensation */
                    switch (chan) {
                    case 4: total[3] += *s; s ++;
                    case 3: total[2] += *s; s ++;
                            total[1] += *s; s ++;
                    case 1: total[0] += *s; s ++;
                    }
                    x ++;
                } else {
                    int err = nerr.y & (VALUES-1);
                    if (xerr == 0 || err == 0) {
                        switch (chan) {
                        case 4: MAX_255(d, (total[3] + (surf>>1)) / surf);
                        case 3: MAX_255(d, (total[2] + (surf>>1)) / surf);
                                MAX_255(d, (total[1] + (surf>>1)) / surf);
                        case 1: MAX_255(d, (total[0] + (surf>>1)) / surf);
                        }
                    } else {
                        /* Horizontal error compensation */
                        switch (chan) {
                        case 4: tmp = *s * err >> BITS; MAX_255(d, (total[3] + tmp + (surf>>1)) / surf); *s -= tmp; s++;
                        case 3: tmp = *s * err >> BITS; MAX_255(d, (total[2] + tmp + (surf>>1)) / surf); *s -= tmp; s++;
                                tmp = *s * err >> BITS; MAX_255(d, (total[1] + tmp + (surf>>1)) / surf); *s -= tmp; s++;
                        case 1: tmp = *s * err >> BITS; MAX_255(d, (total[0] + tmp + (surf>>1)) / surf); *s -= tmp; s++;
                        }
                        s -= chan;
                    }
                    IterDDA(&nerr);
                    IterDDA(&xpos); i --;
                    memset(total, 0, sizeof total);
                }
            }
            #undef MAX_255
            memset(sum, 0, sz<<2);
        }
        out += ret->stride;
    }
    free(sum);
    return 1;
}
