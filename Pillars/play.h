#pragma once

#include <SDL/SDL.h>
#include <cmath>
#include "Actor.h"
#include "Particles.h"

static SDL_Surface* rend = 0;
static long winTime = 0;
static bool isPlayingAudio = false;

void DrawPlay(SDL_Surface* screen);
void UpdatePlay(long time);

void DrawBoard(SDL_Surface* screen);
void DrawHud(SDL_Surface* screen);
void DrawFx(SDL_Surface* screen);
void DrawLine(SDL_Surface* screen, unsigned int color);
void DrawEyes(SDL_Surface* target, int x, int y, Direction direction, int frame);

void CreateMap();
void BombExplode(int position);

inline float dist(float a, float b)
{
	return sqrt(a * a + b * b);
}

inline int addUp(int upper, int lower)
{
	int n = lower;
	int dif = upper - lower;
	
	if (dif % 10 >= 1)
		n += 1;
	if (dif % 100 >= 10)
		n += 10;
	if (dif % 1000 >= 100)
		n += 100;
	if (dif % 10000 >= 1000)
		n += 1000;
	if (dif % 100000 >= 10000)
		n += 10000;
	if (dif % 1000000 >= 100000)
		n += 100000;
	if (dif % 10000000 >= 1000000)
		n += 1000000;

	return n;
}
