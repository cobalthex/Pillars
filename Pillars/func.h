#pragma once

#include <SDL/SDL.h>

inline SDL_Rect Rect(int x, int y, int w, int h)
{
	SDL_Rect r = {x, y, w, h};
	return r;
}

static int szInt = sizeof(int);

inline void DrawDot(SDL_Surface* s, unsigned int color, int x, int y)
{
	if (x >= 0 && y >= 0 && x < s->w && y < s->h)
		((unsigned int*)s->pixels)[y * s->pitch / szInt + x] = color;
}
void DrawLine(SDL_Surface* s, unsigned int color, int x, int y, int length, bool horizontal);
void DrawImg(SDL_Surface* s, unsigned char* img, unsigned char pal[][3], unsigned int x, unsigned int y, unsigned int px, unsigned int py, unsigned int pw, unsigned int ph, unsigned int iw);
void DrawString(SDL_Surface* s, char* string, int x, int y, bool useLargeFont);
void DrawNums(SDL_Surface* s, unsigned int n, int x, int y);

inline void Error(const char* err, bool fatal)
{
	printf("Error: %s", err);
	if (fatal)
	{
		SDL_Quit();
		exit(1);
	}
}