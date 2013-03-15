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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "DownScale.h"

#define WINW    500
#define WINH    500

int main(int nb, char * argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	atexit(SDL_Quit);

	SDL_Surface * screen = SDL_SetVideoMode(WINW, WINH, 32, SDL_HWSURFACE|SDL_ANYFORMAT);
	if (! screen) return 1;

	SDL_WM_SetCaption("Image down sampling", NULL);

	char * file = nb > 1 ? argv[1] : "Rings1_small.png";
	SDL_Surface * img = IMG_Load(file);

	if (img == NULL) { perror(file); return 1; }

	struct Image_t source = {
		.bitmap = img->pixels, .stride = img->pitch, .width = img->w, .height = img->h,
		.bpp = img->format->BytesPerPixel
	};
	int w = 300;
	SDL_Surface * tmp = NULL;

	memset(screen->pixels, 0xff, screen->pitch * screen->h);
	SDL_Rect rect = {.x = (screen->w - img->w) >> 1, .y = (screen->h - img->h) >> 1};
	SDL_BlitSurface(img, NULL, screen, &rect);
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	while (1)
	{
		SDL_Event event;
		while (SDL_WaitEvent(&event))
		{
			int inc = 0;
			switch (event.type) {
			case SDL_QUIT: return 0;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE: return 0;
				case SDLK_LEFT: case SDLK_UP: inc = -2; break;
				case SDLK_RIGHT: case SDLK_DOWN: inc = 2; break;
				default: continue;
				}
				if (inc)
				{
					w += inc;
					if (w > img->w) { w = img->w; continue; }
					if (w < 1) { w = 1; continue; }
					int h = w * img->h / img->w;
					h /= 2;
					if (h < 1) h = 1;

//					fprintf(stderr, "new dimension = %dx%d\n", w, h);

					if (w != img->w && h != img->h)
					{
						/* Cannot use ImageReduce() directly on screen: pixel format differs */
						tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, img->format->BitsPerPixel,
							img->format->Rmask, img->format->Gmask, img->format->Bmask, img->format->Amask);

						struct Image_t dest = {
							.bitmap = tmp->pixels, .stride = tmp->pitch, .width = tmp->w, .height = tmp->h,
							.bpp = tmp->format->BytesPerPixel
						};

						ImageDownScale(&source, &dest);
					}
					else tmp = img;

					SDL_Palette * old = tmp->format->palette;
					tmp->format->palette = img->format->palette;
					memset(screen->pixels, 0xff, screen->pitch * screen->h);
					rect.x = (screen->w - tmp->w) >> 1;
					rect.y = (screen->h - tmp->h) >> 1;
					SDL_BlitSurface(tmp, NULL, screen, &rect);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
					tmp->format->palette = old;
					if (tmp != img) SDL_FreeSurface(tmp);
				}
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
