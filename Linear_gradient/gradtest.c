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

#include <SDL/SDL.h>
#include <math.h>
#include "gradient_general.h"

#define WINW   500
#define WINH   400
#define NBCOL  2
static struct ColorStop_t colors[NBCOL] = {{{255, 0, 0}}, {{20, 20, 255}}};
SDL_Surface * screen;
int angle;

static Uint32 push_ticks(Uint32 interval, void *param)
{
	SDL_Event event = {.type = SDL_USEREVENT};
	SDL_PushEvent(&event);
	return interval;
}

static void reset(Image img)
{
	angle ++;

	int x = cos(angle * M_PI / 180) * 220;
	int y = sin(angle * M_PI / 180) * 220;
	memset(img->bitmap, 0, img->stride * img->height);
	LinearGradient(img, WINW/2-x, WINH/2-y, WINW/2+x, WINH/2+y, colors, NBCOL);
	SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
}

int main (int argc, char * argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
		return 1;

	atexit(SDL_Quit);

	screen = SDL_SetVideoMode(WINW, WINH, 32, SDL_HWSURFACE|SDL_ANYFORMAT);
	if (! screen) return 1;

	struct Image_t img = {
		.bitmap = screen->pixels, .stride = screen->pitch, .width = screen->w, .height = screen->h,
		.bpp = screen->format->BytesPerPixel
	};

	int i;

	/* We need to convert colors into native format */
	for (i = 0; i < NBCOL; i ++)
	{
		DATA8    rgba = colors[i].rgba;
		uint32_t col = SDL_MapRGBA(screen->format, rgba[0], rgba[1], rgba[2], rgba[3]);
		SETU32(colors[i].rgba, &col);
	}

	SDL_AddTimer(10, push_ticks, NULL);

	while (1)
	{
		SDL_Event event;
		while (SDL_WaitEvent(&event))
		{
			switch (event.type) {
			case SDL_USEREVENT: reset(&img); break;
			case SDL_QUIT: return 0;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) return 0;
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

