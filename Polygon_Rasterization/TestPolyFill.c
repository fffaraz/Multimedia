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
#include <SDL/SDL.h>
#include <math.h>
#include "Fill.h"

#define WINSZ     500
#define NB_LINES  50

static Uint32 push_ticks(Uint32 interval, void *param)
{
	SDL_Event event = {.type = SDL_USEREVENT};
	SDL_PushEvent(&event);
	return interval;
}

int main (int argc, char * argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
		return 1;

	SDL_Surface * screen = SDL_SetVideoMode(WINSZ, WINSZ, 24, SDL_HWSURFACE|SDL_ANYFORMAT);
	if (! screen) return 1;

	struct Image_t img = {
		.bitmap = screen->pixels, .stride = screen->pitch, .width = screen->w, .height = screen->h,
		.bpp = screen->format->BytesPerPixel
	};

	float a = 0;
	int   i, pause = 0;
	int   pts[NB_LINES * 6 + 1], * p, * s;

	SDL_AddTimer(50, push_ticks, NULL);

	while (1)
	{
		SDL_Event event;
		while (SDL_WaitEvent(&event))
		{
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) return 0;
				if (event.key.keysym.sym == SDLK_SPACE) pause = !pause;
				break;
			case SDL_USEREVENT:
				if (pause) continue;
				/* Background */
				memset(img.bitmap, 194, img.stride * img.height);
				/* Gear */
				for (i = 0, p = pts, s = pts + NB_LINES*4, *s++ = NEWPATH; i < NB_LINES; i ++, p += 4, s += 2)
				{
					double ca = a + M_PI * 2 * i / NB_LINES, sa, x;
					sa = sin(ca);
					ca = cos(ca);
					x = WINSZ/2 + ca * (WINSZ/3-45); p[(i&1)<<1] = FTOI(x);
					x = WINSZ/2 + sa * (WINSZ/3-45); p[((i&1)<<1)+1] = FTOI(x);
					x = WINSZ/2 + ca * (WINSZ/3-30); p[2-((i&1)<<1)] = FTOI(x);
					x = WINSZ/2 + sa * (WINSZ/3-30); p[3-((i&1)<<1)] = FTOI(x);
					x = WINSZ/2 + ca * (WINSZ/3-60); s[0] = FTOI(x);
					x = WINSZ/2 + sa * (WINSZ/3-60); s[1] = FTOI(x);
				}
				FillPolygonFaster(&img, pts, sizeof pts/sizeof pts[0], SDL_MapRGB(screen->format, 0x7f, 0x06, 0x00));
				a += M_PI/1000;
				SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
				break;
			case SDL_QUIT: return 0;
			}
		}
	}
	return 0;
}

#ifdef WIN32
#include <windows.h>
int WINAPI WinMain (HINSTANCE instance,
                    HINSTANCE previnst,
                    LPSTR args,
                    int wndState)
{
	return main(0, NULL);
}
#endif
